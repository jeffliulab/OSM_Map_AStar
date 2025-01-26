#pragma once
#include "osm_parser.hpp"
#include <queue>
#include <limits>

struct GraphNode {
    long long id;
    double lat, lon;
    std::vector<std::pair<long long, double>> neighbors; // <neighbor_id, weight>
};

class Graph {
public:
    Graph(const OSMParser& parser);
    std::vector<long long> findPath(long long start_id, long long end_id);

private:
    std::unordered_map<long long, GraphNode> nodes;
    double calculateWeight(const Node& from, const Node& to, const std::string& way_type);
    double calculateHeuristic(const GraphNode& current, const GraphNode& goal);

};