#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <tinyxml2.h>

struct Node {
    long long id;
    double lat, lon;
    std::string type;  // highway type if exists
    bool is_intersection;
};

struct Way {
    long long id;
    std::vector<long long> node_refs;
    std::string type;  // highway, primary, secondary, etc.
    double weight_multiplier;  // Based on road type
};

class OSMParser {
public:
    explicit OSMParser(const std::string& filename);
    void parse();
    
    const std::unordered_map<long long, Node>& getNodes() const { return nodes; }
    const std::vector<Way>& getWays() const { return ways; }
    
private:
    std::string filename;
    std::unordered_map<long long, Node> nodes;
    std::vector<Way> ways;
    
    void parseNode(tinyxml2::XMLElement* node);
    void parseWay(tinyxml2::XMLElement* way);
    double getWeightMultiplier(const std::string& type);
};