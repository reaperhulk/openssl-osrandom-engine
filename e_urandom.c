// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <unistd.h>
#include <fcntl.h>

#include <openssl/engine.h>


static int urandom_rand_bytes(unsigned char *buffer, int size);
static int urandom_rand_status(void);
static int urandom_init(ENGINE *e);
static int urandom_finish(ENGINE *e);

static int urandom_fd;

/* Constants used when creating the ENGINE */
static const char *urandom_engine_id= "urandom";
static const char *urandom_engine_name = "urandom engine";


static RAND_METHOD urandom_rand = {
    NULL,
    urandom_rand_bytes,
    NULL,
    NULL,
    urandom_rand_bytes,
    urandom_rand_status,
};

static int urandom_bind_helper(ENGINE *e) {
    if(!ENGINE_set_id(e, urandom_engine_id) ||
            !ENGINE_set_name(e, urandom_engine_name) ||
            !ENGINE_set_RAND(e, &urandom_rand) ||
            !ENGINE_set_init_function(e, urandom_init) ||
            !ENGINE_set_finish_function(e, urandom_finish))

        return 0;
    return 1;
}

static int urandom_rand_bytes(unsigned char *buffer, int size) {
    ssize_t n;
    while (0 < size) {
        do {
            n = read(urandom_fd, buffer, (size_t)size);
        } while (n < 0 && errno == EINTR);
        if (n <= 0) {
            return 0;
            break;
        }
        buffer += n;
        size -= n;
    }
    return 1;
}

static int urandom_rand_status(void) {
    return 1;
}

static int urandom_init(ENGINE *e) {
    urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd > 0) {
        return 1;
    } else {
        return 0;
    }
}

static int urandom_finish(ENGINE *e) {
    int n;
    do {
        n = close(urandom_fd);
    } while (n < 0 && errno == EINTR);
    if (n < 0) {
        return 0;
    } else {
        return 1;
    }
}

#ifndef OPENSSL_NO_DYNAMIC_ENGINE
static int urandom_bind_fn(ENGINE *e, const char *id) {
    if(!urandom_bind_helper(e))
        return 0;
    return 1;
}
IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(urandom_bind_fn)
#endif /* OPENSSL_NO_DYNAMIC_ENGINE */
