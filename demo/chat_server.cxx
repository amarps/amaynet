#include "TCP/TCPListener.hxx"

#include <csignal>
#include <iostream>

volatile sig_atomic_t quit = 0;
AMAYNET::TCPListener *server;

void DropAllConnections();
void SendToAllConnections(std::vector<char>);
void HandleConnections();
void sig_handler(int);

void DropAllConnections()
{
  server->ConnectionBegin();
  for (; !server->IsConnectionEnd(); server->NextConnection()) {
    server->DropConnection();
  }
}

void SendToAllConnections(std::vector<char> buffer)
{
  server->ConnectionBegin();
  for (; !server->IsConnectionEnd(); server->NextConnection()) {
    if (!server->CurrentConnection()->Send(buffer.data(), buffer.size())) { //
      server->DropConnection();
    }
  }
}

void HandleConnections()
{
  server->ConnectionBegin();
  for (; !server->IsConnectionEnd(); server->NextConnection()) {
    if (server->IsConnectionReady()) {
      auto buffer = server->CurrentConnection()->Recv();
      std::string message = "Client ";
      message.append(std::to_string(server->CurrentConnection()->GetFD()));
      message.append(": ");
      if (buffer[0] == '!') { // client request disconect
        message.append("(Disconected)");
        server->DropConnection();
        std::cout << message << std::endl;
        if (server->IsConnectionEnd())
          break;
        buffer.erase(buffer.begin());
      }
      buffer.insert(buffer.begin(), message.begin(), message.end());
      SendToAllConnections(buffer);
      if (server->IsConnectionEnd()) // connection reach end while processing
                                     // client
        break;
    }
  }
}

void sig_handler(int)
{
  quit = 1;
  DropAllConnections();
  server->Close();
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cout << "Usage: server port" << std::endl;
    return 1;
  }

  signal(SIGINT, &sig_handler);
  server = new AMAYNET::TCPListener(argv[1]);
  server->Listen();
  while (!quit) {
    try {
      auto client = server->Accept();
      if (client)
        std::cout << "new connection at file descriptor " << client->GetFD()
                  << std::endl;
    } catch (std::runtime_error &) {
      break;
    }
    HandleConnections();
  }
  delete server;
  std::cout << "quit success" << std::endl;
  return 0;
}
