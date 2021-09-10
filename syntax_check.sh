#!/bin/sh

clang-tidy -header-filter=.* src/main.cxx src/TCP/*.cxx include/*.hxx include/TCP/*.hxx src/HTTP/*.cxx include/HTTP/*.hxx

