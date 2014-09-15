
#ifndef _COMMON_H_
#define _COMMON_H_
#include <openssl/hmac.h>

extern bool VERBOSE;
const int BUF_SIZE = 1024 << 2;
const int MAX_SIZE = 1 << 29;
// prefix size
const int PRE_SIZE = sizeof(int) + EVP_MAX_MD_SIZE;
// does not work in C++ 11
// const char key[16] = { 0xfa, 0xe2, 0x01, 0xd3, 0xba, 0xa9, 0x9b, 0x28, 0x72, 0x61, 0x5c, 0xcc, 0x3f, 0x28, 0x17, 0x0e };

const char key[] = "jdsafjoiwiofhaefshf";

const int KEY_LEN = sizeof(key);

#endif

















