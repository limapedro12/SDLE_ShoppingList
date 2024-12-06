#include "consistentHashing.hpp"
#include "md5.cpp"

Node::Node(std::string id) : id(id) {}

size_t ConsistentHashRing::hashFunction(const std::string& key) {
    md5 encrypter;
    std::string hashString = encrypter.encrypt(key);

    // Convert the first 16 characters (8 bytes) of the hash string to a size_t
    size_t hashValue = 0;
    for (size_t i = 0; i < sizeof(size_t) * 2 && i < hashString.size(); i += 2) {
        hashValue = (hashValue << 8) | std::stoi(hashString.substr(i, 2), nullptr, 16);
    }

    return hashValue;
}

void ConsistentHashRing::addNode(Node* node) {
    size_t hash = hashFunction(node->id);
    ring[hash] = node;
}

Node* ConsistentHashRing::getNode(const std::string& key) {
    size_t hash = hashFunction(key);
    auto it = ring.upper_bound(hash);
    if (it == ring.end()) {
        return ring.begin()->second;
    }
    return it->second;
}