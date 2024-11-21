#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>
#include <iostream>
#include <fstream>

// ChaCha20 암호화 함수
std::vector<unsigned char> encryptChaCha20(const std::vector<unsigned char>& data, 
                                           const std::vector<unsigned char>& key, 
                                           const std::vector<unsigned char>& nonce) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Failed to create context!" << std::endl;
        return {};
    }

    std::vector<unsigned char> encryptedData(data.size());

    // 암호화 초기화
    if (EVP_EncryptInit_ex(ctx, EVP_chacha20(), nullptr, key.data(), nonce.data()) != 1) {
        std::cerr << "Failed to initialize encryption!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int outLen;
    if (EVP_EncryptUpdate(ctx, encryptedData.data(), &outLen, data.data(), data.size()) != 1) {
        std::cerr << "Encryption failed!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);
    return encryptedData;
}

int main() {
    // 암호화 키 및 nonce 생성 (32바이트 키, 12바이트 nonce)
    std::vector<unsigned char> key(32, 0);
    std::vector<unsigned char> nonce(12, 0);
    RAND_bytes(key.data(), key.size());
    RAND_bytes(nonce.data(), nonce.size());

    // 예제 데이터
    std::vector<unsigned char> data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};

    // 암호화
    auto encryptedData = encryptChaCha20(data, key, nonce);
    if (!encryptedData.empty()) {
        std::cout << "Encryption successful. Saving data to files..." << std::endl;

        // 암호화된 데이터 저장
        std::ofstream encryptedFile("encrypted.bin", std::ios::binary);
        encryptedFile.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
        encryptedFile.close();

        // 키와 nonce 저장
        std::ofstream keyFile("key_nonce.bin", std::ios::binary);
        keyFile.write(reinterpret_cast<const char*>(key.data()), key.size());
        keyFile.write(reinterpret_cast<const char*>(nonce.data()), nonce.size());
        keyFile.close();
    } else {
        std::cerr << "Encryption failed!" << std::endl;
    }

    return 0;
}
