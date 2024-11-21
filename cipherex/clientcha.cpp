#include <openssl/evp.h>
#include <vector>
#include <iostream>
#include <fstream>

// ChaCha20 복호화 함수
std::vector<unsigned char> decryptChaCha20(const std::vector<unsigned char>& encryptedData, 
                                           const std::vector<unsigned char>& key, 
                                           const std::vector<unsigned char>& nonce) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Failed to create context!" << std::endl;
        return {};
    }

    std::vector<unsigned char> decryptedData(encryptedData.size());

    // 복호화 초기화
    if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr, key.data(), nonce.data()) != 1) {
        std::cerr << "Failed to initialize decryption!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int outLen;
    if (EVP_DecryptUpdate(ctx, decryptedData.data(), &outLen, encryptedData.data(), encryptedData.size()) != 1) {
        std::cerr << "Decryption failed!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);
    return decryptedData;
}

int main() {
    // 저장된 키와 nonce 읽기
    std::vector<unsigned char> key(32, 0);
    std::vector<unsigned char> nonce(12, 0);

    std::ifstream keyFile("key_nonce.bin", std::ios::binary);
    keyFile.read(reinterpret_cast<char*>(key.data()), key.size());
    keyFile.read(reinterpret_cast<char*>(nonce.data()), nonce.size());
    keyFile.close();

    // 암호화된 데이터 읽기
    std::ifstream encryptedFile("encrypted.bin", std::ios::binary);
    std::vector<unsigned char> encryptedData((std::istreambuf_iterator<char>(encryptedFile)), 
                                              std::istreambuf_iterator<char>());
    encryptedFile.close();

    // 복호화
    auto decryptedData = decryptChaCha20(encryptedData, key, nonce);
    if (!decryptedData.empty()) {
        std::cout << "Decryption successful. Decrypted data: ";
        for (unsigned char byte : decryptedData) {
            std::cout << byte;
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Decryption failed!" << std::endl;
    }

    return 0;
}
