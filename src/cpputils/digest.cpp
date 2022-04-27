#include "digest.h"

namespace digest {

    std::string hex_to_string(const std::string& hex) {
        std::vector<char> bytes;

        for (unsigned int i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            char byte = (char) strtol(byteString.c_str(), nullptr, 16);
            bytes.push_back(byte);
        }

        return {bytes.begin(), bytes.end()};
    }

    template <typename HashMethod>
    std::string apply_hmac(std::vector<std::string> data, std::string key)
    {

        key = hmac<HashMethod>(key, data[0]);

//         skip first and iterate over each object
        for(const std::string& item : data){
            key = hmac<HashMethod>(key, hex_to_string(item));
        };

        return(key);
    }

    std::string simple_hmac(const std::vector<std::string>& data, const std::string& key, std::string algo = "SHA256")
    {
        std::vector<std::string> algorithms = {
                "SHA256",
                "SHA1" ,
                "SHA3",
                "CRC32",
                "Keccak",
                "MD5" ,
        };

        std::transform(algo.begin(),
                       algo.end(),
                       algo.begin(),
                       ::toupper);

        switch(std::find(algorithms.begin(),
                         algorithms.end(),algo
        )-algorithms.begin()) {
            case 0 : {
                return apply_hmac<SHA256>(data, key);
            } break;
            case 1 : {
                return apply_hmac<SHA1>(data, key);
            } break;
//            case 2 : {
//                return apply_hmac<SHA3>(data, key);
//            } break;
//            case 3 : {
//                return apply_hmac<CRC32>(data, key);
//            } break;
//            case 4 : {
//                return apply_hmac<Keccak>(data, key);
//            } break;
            default:
                return apply_hmac<MD5>(data, key);
        }
    }

}
