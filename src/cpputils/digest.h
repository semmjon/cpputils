//
// Created by sgeist on 25.02.22.
//

#ifndef CPPUTILS_DIGEST_H
#define CPPUTILS_DIGEST_H

#include <vector>
#include <algorithm>
#include "hash-library/crc32.h"
#include "hash-library/md5.h"
#include "hash-library/sha1.h"
#include "hash-library/sha256.h"
#include "hash-library/keccak.h"
#include "hash-library/sha3.h"
#include "hash-library/hmac.h"

namespace digest {
    std::string hex_to_string(const std::string& hex);
    template<typename HashMethod>
    std::string apply_hmac(std::vector<std::string> & data, std::string key);
    std::string simple_hmac(const std::vector<std::string> &data, const std::string& key, std::string algo);
}

#endif //CPPUTILS_DIGEST_H
