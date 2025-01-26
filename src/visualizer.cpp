#include "visualizer.hpp"

Visualizer::Visualizer(const OSMParser& parser, int width, int height) 
   : parser(parser), width(width), height(height) {
   image = cv::Mat(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
   initializeBounds();
}

void Visualizer::initializeBounds() {
   const auto& nodes = parser.getNodes();
   min_lat = min_lon = std::numeric_limits<double>::max();
   max_lat = max_lon = std::numeric_limits<double>::lowest();

   for (const auto& [_, node] : nodes) {
       min_lat = std::min(min_lat, node.lat);
       max_lat = std::max(max_lat, node.lat);
       min_lon = std::min(min_lon, node.lon);
       max_lon = std::max(max_lon, node.lon);
   }
}

cv::Point2i Visualizer::toImageCoords(double lat, double lon) {
   int x = static_cast<int>((lon - min_lon) / (max_lon - min_lon) * width);
   int y = height - static_cast<int>((lat - min_lat) / (max_lat - min_lat) * height);
   return cv::Point2i(x, y);
}

void Visualizer::drawMap() {
   const auto& nodes = parser.getNodes();
   const auto& ways = parser.getWays();

   // 绘制所有道路
   for (const auto& way : ways) {
       for (size_t i = 0; i < way.node_refs.size() - 1; i++) {
           const auto& node1 = nodes.at(way.node_refs[i]);
           const auto& node2 = nodes.at(way.node_refs[i + 1]);

           cv::Point2i p1 = toImageCoords(node1.lat, node1.lon);
           cv::Point2i p2 = toImageCoords(node2.lat, node2.lon);

           // 根据道路类型设置颜色
           cv::Scalar color(200, 200, 200); // 默认灰色
           if (way.type == "motorway") color = cv::Scalar(0, 0, 255);
           else if (way.type == "primary") color = cv::Scalar(0, 165, 255);
           else if (way.type == "secondary") color = cv::Scalar(0, 255, 255);

           cv::line(image, p1, p2, color, 1);
       }
   }
}

void Visualizer::drawPath(const std::vector<long long>& path_nodes) {
   const auto& nodes = parser.getNodes();

   // 绘制路径
   for (size_t i = 0; i < path_nodes.size() - 1; i++) {
       const auto& node1 = nodes.at(path_nodes[i]);
       const auto& node2 = nodes.at(path_nodes[i + 1]);

       cv::Point2i p1 = toImageCoords(node1.lat, node1.lon);
       cv::Point2i p2 = toImageCoords(node2.lat, node2.lon);

       // 使用粗线绘制路径
       cv::line(image, p1, p2, cv::Scalar(255, 0, 0), 3);
   }

   // 标记起点和终点
   if (!path_nodes.empty()) {
       const auto& start = nodes.at(path_nodes.front());
       const auto& end = nodes.at(path_nodes.back());

       cv::Point2i start_point = toImageCoords(start.lat, start.lon);
       cv::Point2i end_point = toImageCoords(end.lat, end.lon);

       cv::circle(image, start_point, 8, cv::Scalar(0, 255, 0), -1);
       cv::circle(image, end_point, 8, cv::Scalar(0, 0, 255), -1);
   }
}

void Visualizer::show() {
   cv::imshow("OSM Path Planning", image);
   cv::waitKey(0);
}

void Visualizer::save(const std::string& filename) {
   cv::imwrite(filename, image);
}