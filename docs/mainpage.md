Home                        {#mainpage}
============

this page also available at Documentation/html/index.html

## Introduction
Amaynet is a c++ library to help make a connection on bsd socket.\n
amaynet is created for my network programming learning purposes.\n
Currently, amaynet just support TCP connection like tcpclient and tcplistener.\n
there is also httpserver derived from tcplistener.\n
amaynet use [select](https://man7.org/linux/man-pages/man2/select.2.html) for monitoring file descriptor.\n
tcplistener implemented as synchronous blocking singlethreaded and use forward list for storing connection.\n
another implementation scenario will be added in the later version if everything goes well.\n

## How to Build
How to build. \n
clone and go to directory
```shell

    git clone git@gitlab.com:amarpanjis/amayhttps.git && cd amaynet
	
```

create build directory
```shell

	mkdir build && cd build
	
```

generate build system using cmake and compile it 
```shell

	cmake .. && cmake --build
	
```
	
build complete if there is no error.\n\n

## installing
after generating build directory you can simply type
```shell
	cmake --install .
```

to choose installation location you can use --prefix like this
```shell
	cmake --install . --prefix "/path/to/installdir"
```

## uninstalling
to uninstall just run this command inside build directory
```shell
	xargs rm < install_manifest.txt
```

cmake option flag
+ BUILD_DOC\n
  if `BUILD_DOC` is ON documentation will be generated at <build_directory>/Documentation/. default value ON

## Example daytime client and server

### example client
TCPClient takes 2 arguments hostname and port.\n
when TCPClient object is created it automatically connect to given hostname and port.\n
TCPClient will return an empty object if it fail to connect.\n
you can check the object's file descriptor by using GetFD Method to check if the connection is success.\n
```cpp
#include <iostream>
#include <string>
#include "amaynet/TCP/TCPClient.hxx"

int main(int argc, char *argv[]) {
  if(argc < 3) {
    std::cerr << "Usage: client 127.0.0.1 8080" << std::endl;
    return 1;
  }
  auto hostname = argv[1];
  auto port = argv[2];

  AMAYNET::TCPClient client(hostname, port);
  if(!client.GetFD()) { // fd will be 0 if client failed to connect
    std::cerr << "Failed to connect to "
	      << hostname << ":"
	      << port
	      << std::endl;
    return 1;
  }

  while (!client.Ready()); // wait until client ready to recv
  
  auto recv_buf = client.Recv(); // client recv will return vector<char>

  // remove extra data from buffer
  auto recv_str = std::string(recv_buf.data());
  auto end = recv_str.find_first_of("\r\n");
  recv_str = recv_str.substr(0, end);

  // print received message to the console
  std::cout << recv_str << std::endl;
  
  return 0;
}
```

### example server

Unlike TCPClient, TCPListener just need port to initialize and will run on localhost.\n
TCPListener stores all client in forward list you can use CurrentConnection method to get current iterator of list.\n
The Accept() method push new connection to back(next) of current connection iterator to list. \n
every connection operation on TCPListner is O(1) \n
- DropConnection  drop current connection and and call nextconnection\n
- NextConnection  move to next connection\n
- ConnectionBegin move connection to the beginning\n

```cpp
#include <iostream>
#include <ctime>
#include "amaynet/TCP/TCPListener.hxx"

int main(int argc, char *argv[]) {
  if(argc < 2) {
    std::cout << "Usage: server port" << std::endl;
    return 1;
  }
  auto port = argv[1];

  AMAYNET::TCPListener server(port);
  server.Listen();
  
  server.Accept(); // blocking

  std::time_t now;
  std::time(&now);

  std::string now_str = std::ctime(&now);
  now_str.append("\r\n");
  
  if(server.CurrentConnection()->Send(now_str.c_str())) {
    server.DropConnection();
  }
  
  return 0;
}
```


### Build with g++
TCP class use libssl so you need to link libssl and libcrypto along with libamaynet.
```shell

    g++ server.cc -lamaynet -lssl -lcrypto -o client
    g++ client.cc -lamaynet -lssl -lcrypto -o client
	
```

For more example take a look at demo/
