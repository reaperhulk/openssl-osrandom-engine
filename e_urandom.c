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


static int rand_bytes(unsigned char *buffer, int size);
static int rand_status(void);

/* Constants used when creating the ENGINE */
static const char *engine_id= "urandom";
static const char *engine_name = "urandom engine";

static RAND_METHOD urand_rand = {
    NULL,
    rand_bytes,
    NULL,
    NULL,
    rand_bytes,
    rand_status,
};

static int bind_helper(ENGINE *e)
    {
    if(!ENGINE_set_id(e, engine_id) ||
            !ENGINE_set_name(e, engine_name) ||
            !ENGINE_set_RAND(e, &urand_rand))
        return 0;
    return 1;
    }

static int rand_bytes(unsigned char *buffer, int size) {
    int fd;
    ssize_t n;

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
        return 0;

    while (0 < size) {
        do {
            n = read(fd, buffer, (size_t)size);
        } while (n < 0 && errno == EINTR);
        if (n <= 0) {
            return 0;
            break;
        }
        buffer += n;
        size -= n;
    }
    close(fd);
    return 1;
}

static int rand_status(void) {
    return 1;
}

#ifndef OPENSSL_NO_DYNAMIC_ENGINE
static int bind_fn(ENGINE *e, const char *id) {
    if(!bind_helper(e))
        return 0;
    return 1;
}
IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(bind_fn)
#endif /* OPENSSL_NO_DYNAMIC_ENGINE */
