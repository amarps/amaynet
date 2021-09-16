#!/bin/sh

clang-tidy -header-filter=.* src/TCP/*.cxx src/HTTP/*.cxx include/*.hxx include/TCP/*.hxx include/HTTP/*.hxx
