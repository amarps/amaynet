#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "TCP/TCPClient.hxx"

void signal_handler(int);

class ClientChat
{
public:
  ClientChat() {
    DelayCheckChat(0.1);
  }
  ClientChat(const ClientChat &)=delete;
  ClientChat operator=(const ClientChat &)=delete;
  ~ClientChat() {
    Interrupt();
  }

  bool Connect(unsigned port, const std::string &hostname) {
    _client = std::make_unique<AMAYNET::TCPClient>(hostname, std::to_string(port));
    _connection_valid = _client->GetFD() != -1;
    return _connection_valid;
  }

  void RunIOProcess() {
    if(!_connection_valid) return;
    _istream_handler = std::thread([this]{IstreamProcess();});
    _ostream_handler = std::thread([this]{OstreamProcess();});
    _history_update_handler = std::thread([this]{HistoryUpdateProcess();});
  }

  // set delay before check if there is new chat
  void DelayCheckChat(float val) {
    _history_check_delay = std::chrono::duration<float>(val);
  }

  // get delay before check if there is new chat
  float DelayCheckChat() {
    return _history_check_delay.count();
  }

  void Interrupt()
  {
    char quitmsg[] = "!";
    if (_client) _client->Send(quitmsg, 1);
    _need_to_quit = true;
    _history_updated = true;
    _history_condition.notify_all();
    if(_istream_handler.joinable() &&
       std::this_thread::get_id() != _istream_handler.get_id())
      _istream_handler.join();
    if(_ostream_handler.joinable())
      _ostream_handler.join();
    if(_history_update_handler.joinable())
      _history_update_handler.join();

    _client.reset();
  }

  bool NeedToQuit() const { return _need_to_quit; }

private:
  std::unique_ptr<AMAYNET::TCPClient> _client;
  std::vector<std::vector<char>> _history; // shared between threads
  bool _history_updated = false;
  std::string _input;
  volatile bool _need_to_quit = false;
  volatile bool _connection_valid = false;
  
  size_t _max_history_len = 50;
  std::chrono::duration<float> _history_check_delay; // to check client is ready to read data

  std::thread _istream_handler;
  std::thread _ostream_handler;
  std::thread _history_update_handler;
  std::mutex  _history_mutex;
  std::condition_variable _history_condition;

  void OstreamProcess() {
    while(!_need_to_quit) {
      std::unique_lock<std::mutex> guard(_history_mutex);
      _history_condition.wait(guard, [this]{return _history_updated;});
      if(_need_to_quit)
	break;
      _history_updated = false;
      _input.clear();
      std::cout << "\033[2J" << std::endl; // clear screen
      for (auto chat = _history.begin(); chat != _history.end(); ++chat) {
	std::cout << chat->data() << std::endl;
      }
      std::cout << "> " << std::flush;
    }
  }

  void IstreamProcess() {
    std::cout << "\033[2J" // clear screen
	      << "> " << std::flush;
    while(!_need_to_quit) {
      std::getline(std::cin, _input);

      if (std::cin.eof()) {
	Interrupt();
	break;
      }
      if (!_input.empty()) {
	/// add ;;; to indicate the end of the message
	_input.append(";;;");
        if (!_client->Send(_input)) {
	  Interrupt();
	  break;
	}
      }
    }
  }

  void HistoryUpdateProcess() {
    while(!_need_to_quit) {
      if(_client->Ready()){
	auto buffer = _client->Recv();
        if (GetValidMessage(buffer)) {
          _history.push_back(std::vector<char>(buffer.begin(), buffer.end()));
	  _history_updated = true;
	  _history_condition.notify_one();
        }
      }
      std::this_thread::sleep_for(_history_check_delay);
    }
  }

  bool GetValidMessage(std::vector<char> &buffer) {
    auto buffer_str = std::string(buffer.begin(), buffer.end());
    static auto start_with = std::string("Client");
    if (buffer_str.substr(0, start_with.size()) != start_with)
      return false;

    auto end_pos = buffer_str.find(";;;");
    if(end_pos == std::string::npos)
      return false;
    
    buffer = std::vector<char>(buffer_str.begin(), buffer_str.begin()+end_pos);
    buffer.push_back('\0');
    return true;
  }
};

volatile sig_atomic_t g_want_to_quit;

void signal_handler(int)
{
  g_want_to_quit = 1;
}

int main(int argc, char *argv[])
{
  if(argc < 3) {
    std::cerr << "Usage: client 127.0.0.1 8080" << std::endl;
    return 1;
  }
  unsigned port;
  auto hostname = argv[1];
  try {
    port = std::stoul(argv[2]);
  } catch (std::invalid_argument) {
    std::cerr << "Invalid port\n"
                 "Usage: client 127.0.0.1 8080"
	      << std::endl;
    return 1;
  }
  ClientChat chat_client;
  
  if(!chat_client.Connect(port, hostname)) {
    std::cerr << "Failed to connect to " << hostname << ":" << port
	      << "\nCheck your connection." << std::endl;
    return 1;
  }

  std::signal(SIGINT, signal_handler);
  chat_client.RunIOProcess();

  while (!chat_client.NeedToQuit()) { // Ctrl-D quit
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if(g_want_to_quit) {  // Ctrl-c <Enter> quit
      chat_client.Interrupt();
    }
  }

  std::cout << "Connection closed ok. " << hostname << ":" << port << std::endl;
};
