#!/bin/sh

clang-tidy -header-filter=.* ./ src/main.cxx src/TCP/*.cxx src/TCP/*.hxx  src/HTTP/*.cxx src/HTTP/*.hxx

