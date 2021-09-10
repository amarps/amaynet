#include "TCPListener.hxx"
namespace AMAYNET {

#define C_CONN TCPListener::Connection
  
  C_CONN::Connection(TCP *_server)
    :server(_server)
  {
    _iter = _conn_list.end();
  }
  
  C_CONN::~Connection() {
    for (auto *it : _conn_list) {
      delete it;
    }
  }

  std::forward_list<TCP*>::iterator C_CONN::iter() const
  {
    return _iter;
  }

  std::forward_list<TCP*>::iterator C_CONN::end()
  {
    return _conn_list.end();
  }

  void C_CONN::push_back(TCP *val)
  {
    if (_conn_list.empty()) {
      _conn_list.push_front(val);
      _iter = _last_element = _conn_list.begin();
      _iter_prev = _conn_list.before_begin();
      return;
    }
    
    _last_element = _conn_list.insert_after(_last_element, val);
  }
  
  void C_CONN::Next()
  {
    if(connection_droped) {
      connection_droped =false;
      return;
    }
    ++_iter;
    ++_iter_prev;
  }

  void C_CONN::Drop()
  {
    if(_iter == end()) {
      return;
}
    delete *_iter;
    if (_iter == _last_element) {
      _iter = _last_element = _conn_list.erase_after(_iter_prev);
    } else {
      _iter = _conn_list.erase_after(_iter_prev);
    }
    connection_droped = true;
  }

  bool C_CONN::Ready() {
    return FD_ISSET(data()->GetFD(), &_reads);
  }

  fd_set C_CONN::Wait() {
    FD_ZERO(&_reads);
    FD_SET(server->GetFD(), &_reads);
    int max_socket = server->GetFD();

    if(_conn_list.empty()) {
      return _reads;
}
      
    for (auto *conn_it : _conn_list) {
      FD_SET(conn_it->GetFD(), &_reads);
      if (conn_it->GetFD() > max_socket) {
	max_socket = conn_it->GetFD();
}
    }
      
    if (select(max_socket + 1, &_reads, 0, 0, 0) < 0) {
      perror("Error ");
      throw std::runtime_error("Select Error");
    }

    return _reads;
  }
  
  void C_CONN::MoveToBegin()
  {
    _iter = _conn_list.begin();
    _iter_prev = _conn_list.before_begin();
  }

  TCP *C_CONN::data() { return *_iter; }

} // namespace AMAYNET
