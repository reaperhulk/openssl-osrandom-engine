## OpenSSL urandom engine

This is a very simple engine that can replace the OpenSSL CSPRNG with /dev/urandom. Why would anyone want to do this? Well, you can avoid reseed after fork issues that many applications run into when consuming OpenSSL. Additionally, if you believe it's easier to make mistakes with seeding userland CSPRNGs then bypassing OpenSSL's in favor of the host OS makes sense.

###Caveats

If you prefer the OpenSSL CSPRNG over /dev/urandom then this is not the engine for you. Otherwise the current version of this engine has the following issues:

* No persistent urand file descriptor. Performance will suffer.
* No Windows support
* Brand new, use at your own risk.

#### Compiling

You'll need a compiler (clang/gcc) and OpenSSL.

```
clang -c -fpic e_urandom.c
clang -shared -lcrypto -o e_urandom.so
```

If you're compiling against an alternate OpenSSL (like one from homebrew) it might look more like this:

```
clang -c -fpic -I/usr/local/opt/openssl/include e_urandom.c
clang -shared -L/usr/local/opt/openssl/lib e_urandom.o -lcrypto -o e_urandom.so
```

#### Using It

Load the engine and set it as the default RAND (in C):

```c
ENGINE *e = ENGINE_by_id("dynamic");
if (!ENGINE_ctrl_cmd_string(e, "SO_PATH", "/path/to/e_urandom.so", 0)) {
  printf("error");
}
if (!ENGINE_ctrl_cmd_string(e, "ID", "urandom", 0)) {
  printf("error");
}
if (!ENGINE_ctrl_cmd_string(e, "LOAD", null, 0)) {
  printf("error");
}
if (!ENGINE_set_default_RAND(e)) {
  printf("error");
}
```
