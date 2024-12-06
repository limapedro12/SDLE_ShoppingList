#ifndef CONSISTENT_HASHING_HPP
#define CONSISTENT_HASHING_HPP

#include <string>
#include <unordered_map>
#include <map>
#include <functional>

class Node;

class ConsistentHashRing {
private:
    std::map<size_t, Node*> ring;
    size_t hashFunction(const std::string& key);

public:
    void addNode(Node* node);
    Node* getNode(const std::string& key);
};

class Node {
public:
    std::string id;
    std::unordered_map<std::string, std::string> data;

    Node(std::string id);
};

#endif