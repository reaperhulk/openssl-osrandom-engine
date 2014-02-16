#!/bin/bash
clang -c -fpic -I/usr/local/opt/openssl/include e_urandom.c
clang -shared -L/usr/local/opt/openssl/lib e_urandom.o -lcrypto -o e_urandom.dylib
