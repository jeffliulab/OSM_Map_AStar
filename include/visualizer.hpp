#pragma once
#include "graph.hpp"
#include <opencv2/opencv.hpp>

class Visualizer {
public:
    Visualizer(const OSMParser& parser, int width = 1200, int height = 800);
    void drawMap();
    void drawPath(const std::vector<long long>& path_nodes);
    void show();
    void save(const std::string& filename);

private:
    const OSMParser& parser;
    cv::Mat image;
    double min_lat, max_lat, min_lon, max_lon;
    int width, height;

    cv::Point2i toImageCoords(double lat, double lon);
    void initializeBounds();
};