#include <iostream>
#include <string>
#include <openssl/evp.h>

class md5 {
public:
    std::string encrypt(const std::string& input) {
        unsigned char result[EVP_MAX_MD_SIZE];
        unsigned int result_len;

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
        EVP_DigestUpdate(ctx, input.c_str(), input.length());
        EVP_DigestFinal_ex(ctx, result, &result_len);
        EVP_MD_CTX_free(ctx);

        // Convert result to a hexadecimal string
        std::string output;
        char buffer[3];
        for (unsigned int i = 0; i < result_len; i++) {
            sprintf(buffer, "%02x", result[i]);
            output += buffer;
        }
        return output;
    }
};
