#!/bin/bash
clang -c -fpic -I/usr/local/opt/openssl/include e_osrandom.c
clang -shared -L/usr/local/opt/openssl/lib e_osrandom.o -lcrypto -o e_osrandom.dylib
