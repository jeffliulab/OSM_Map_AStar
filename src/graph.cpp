#include "graph.hpp"
#include <cmath>
#include <queue>
#include <limits>
#include <algorithm>

Graph::Graph(const OSMParser& parser) {
   const auto& osm_nodes = parser.getNodes();
   const auto& ways = parser.getWays();

   for (const auto& way : ways) {
       for (size_t i = 0; i < way.node_refs.size() - 1; i++) {
           long long from_id = way.node_refs[i];
           long long to_id = way.node_refs[i + 1];
           
           const Node& from_node = osm_nodes.at(from_id);
           const Node& to_node = osm_nodes.at(to_id);
           
           double weight = calculateWeight(from_node, to_node, way.type);

           if (nodes.find(from_id) == nodes.end()) {
               nodes[from_id] = GraphNode{from_id, from_node.lat, from_node.lon};
           }
           if (nodes.find(to_id) == nodes.end()) {
               nodes[to_id] = GraphNode{to_id, to_node.lat, to_node.lon};
           }

           nodes[from_id].neighbors.push_back({to_id, weight});
           nodes[to_id].neighbors.push_back({from_id, weight});
       }
   }
}

double Graph::calculateWeight(const Node& from, const Node& to, const std::string& way_type) {
   const double R = 6371000;
   const double lat1 = from.lat * M_PI / 180.0;
   const double lat2 = to.lat * M_PI / 180.0;
   const double dLat = (to.lat - from.lat) * M_PI / 180.0;
   const double dLon = (to.lon - from.lon) * M_PI / 180.0;

   double a = sin(dLat/2) * sin(dLat/2) +
              cos(lat1) * cos(lat2) *
              sin(dLon/2) * sin(dLon/2);
   double c = 2 * atan2(sqrt(a), sqrt(1-a));
   double distance = R * c;

   double type_multiplier = 1.0;
   if (way_type == "motorway") type_multiplier = 0.5;
   else if (way_type == "primary") type_multiplier = 0.7;
   else if (way_type == "secondary") type_multiplier = 0.9;
   else if (way_type == "residential") type_multiplier = 1.2;

   return distance * type_multiplier;
}

double Graph::calculateHeuristic(const GraphNode& current, const GraphNode& goal) {
   const double R = 6371000;
   const double lat1 = current.lat * M_PI / 180.0;
   const double lat2 = goal.lat * M_PI / 180.0;
   const double dLat = (goal.lat - current.lat) * M_PI / 180.0;
   const double dLon = (goal.lon - current.lon) * M_PI / 180.0;

   double a = sin(dLat/2) * sin(dLat/2) +
              cos(lat1) * cos(lat2) *
              sin(dLon/2) * sin(dLon/2);
   double c = 2 * atan2(sqrt(a), sqrt(1-a));
   return R * c;
}

std::vector<long long> Graph::findPath(long long start_id, long long end_id) {
    using PriorityQueue = std::priority_queue<  // 添加 
    std::pair<double, long long>,
    std::vector<std::pair<double, long long>>,
    std::greater<>
>;
    PriorityQueue pq;

   std::unordered_map<long long, double> g_score;
   std::unordered_map<long long, long long> came_from;

   for (const auto& node : nodes) {
       g_score[node.first] = std::numeric_limits<double>::infinity();
   }

   g_score[start_id] = 0;
   pq.push({0, start_id});

   const auto& goal_node = nodes.at(end_id);

   while (!pq.empty()) {
       long long current = pq.top().second;
       pq.pop();

       if (current == end_id) {
           std::vector<long long> path;
           while (current != start_id) {
               path.push_back(current);
               current = came_from[current];
           }
           path.push_back(start_id);
           std::reverse(path.begin(), path.end());
           return path;
       }

       for (const auto& [neighbor, weight] : nodes[current].neighbors) {
           double tentative_g_score = g_score[current] + weight;

           if (tentative_g_score < g_score[neighbor]) {
               came_from[neighbor] = current;
               g_score[neighbor] = tentative_g_score;
               const auto& current_node = nodes.at(neighbor);
               double f_score = tentative_g_score + calculateHeuristic(current_node, goal_node);
               pq.push({f_score, neighbor});
           }
       }
   }

   return {};
}