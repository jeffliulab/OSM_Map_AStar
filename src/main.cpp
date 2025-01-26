// main.cpp
#include "osm_parser.hpp"
#include "graph.hpp"
#include "visualizer.hpp"
#include <iostream>

int main() {
   try {
       OSMParser parser("resources/map.osm");
       parser.parse();
       Graph graph(parser);

       // 选择两个固定的节点作为起点和终点
       // long long start_id = 61172421;  // 北纬: 42.4054370, 西经: 71.1258370
       // long long end_id = 61172482;    // 北纬: 42.4040290, 西经: 71.1175103

        long long start_id = 1097255634;  // Tufts Garage
        long long end_id = 1479769463;  // Torbert Macdonald Park

       auto path = graph.findPath(start_id, end_id);
       if (path.empty()) {
           std::cerr << "No path found between the specified points." << std::endl;
           return 1;
       }

       Visualizer vis(parser);
       vis.drawMap();
       vis.drawPath(path);
       vis.save("output/path.png");
       vis.show();

   } catch (const std::exception& e) {
       std::cerr << "Error: " << e.what() << std::endl;
       return 1;
   }

   return 0;
}