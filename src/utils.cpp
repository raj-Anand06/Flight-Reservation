#include "utils.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // Compute hash
    SHA256(reinterpret_cast<const unsigned char*>(input.data()),
           input.size(), hash);

    // Convert to hex string
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(hash[i]);
    }
    return oss.str();
}
