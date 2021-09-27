Home                        {#mainpage}
============

this page also available at Documentation/html/index.html

## Introduction
Amaynet is a c++ library to help make an unix socket connection in a simple way.\n
currently amaynet just support to use TCP connection like tcpclient and tcplistener.\n
there is also httpserver derived from tcplistener.\n
amaynet use [select](https://man7.org/linux/man-pages/man2/select.2.html) for monitoring file descriptor. tcplistener use forward list for storing connection.\n
maybe in the future i will make more scenario for another type of connection.\n

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

## Tutorial
Tutorial make simple client server chat.
```cpp

	/* file: chat.cc */

    #include "TCP/TCPListener.hxx"
    #include "TCP/TCPClient.hxx"

	#include <iostream>

	int main(int argc, char *argv[]) {
		if (argc < 2) {
		    std::cout << "client usage: chat 127.0.0.1 8080\n"
			          << "server usage: chat 8080\n"
					  << std::endl;
	        return 1;
        }
	}

```

### Build with g++
```shell

    g++ chat.cc -lamaynet -lssl -lcrypto
	
```
For more example take a look at demo/
