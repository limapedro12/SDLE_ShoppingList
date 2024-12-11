#include "consistentHashing.hpp"
#include "md5.cpp"
#include <unordered_set>

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

void ConsistentHashRing::addNode(Node* node, int virtualNodes) {
    for (int i = 0; i < virtualNodes; ++i) {
        std::string virtualNodeID = node->id + "#" + std::to_string(i);
        size_t hash = hashFunction(virtualNodeID);
        ring[hash] = node;
    }
}

bool ConsistentHashRing::isEmpty() {
    return ring.empty();
}

Node* ConsistentHashRing::getNode(const std::string& key) {
    size_t hash = hashFunction(key);
    auto it = ring.upper_bound(hash);
    if (it == ring.end()) {
        return ring.begin()->second;
    }
    return it->second;
}

std::vector<Node*> ConsistentHashRing::getPreferenceList(const std::string& key, int replicationFactor) {
    std::vector<Node*> preferenceList;
    std::unordered_set<std::string> uniqueNodes;

    if (ring.empty()) {
        return preferenceList; // No nodes available
    }

    // Adjust replication factor to the number of available nodes if necessary
    int adjustedReplicationFactor = std::min(replicationFactor, static_cast<int>(ring.size()));

    size_t hash = hashFunction(key);
    auto it = ring.upper_bound(hash);
    if (it == ring.end()) it = ring.begin();

    // Collect nodes for preference list
    while (preferenceList.size() < static_cast<size_t>(adjustedReplicationFactor)) {
        Node* node = it->second;
        if (uniqueNodes.find(node->id) == uniqueNodes.end()) {
            preferenceList.push_back(node);
            uniqueNodes.insert(node->id);
        }

        ++it;
        if (it == ring.end()) it = ring.begin(); // Wrap around the ring
    }

    return preferenceList;
}

std::map<size_t, Node*> ConsistentHashRing::getRing() {
    return ring;
}
