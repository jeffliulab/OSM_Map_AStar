#include "osm_parser.hpp"
#include <cmath>

OSMParser::OSMParser(const std::string& filename) : filename(filename) {}

void OSMParser::parse() {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.c_str());

    auto root = doc.FirstChildElement("osm");
    for (auto element = root->FirstChildElement(); element; element = element->NextSiblingElement()) {
        std::string elementType = element->Name();
        if (elementType == "node") {
            parseNode(element);
        } else if (elementType == "way") {
            parseWay(element);
        }
    }
}

void OSMParser::parseNode(tinyxml2::XMLElement* node) {
    Node n;
    n.id = node->Int64Attribute("id");
    n.lat = node->DoubleAttribute("lat");
    n.lon = node->DoubleAttribute("lon");
    n.is_intersection = false;

    for (auto tag = node->FirstChildElement("tag"); tag; tag = tag->NextSiblingElement("tag")) {
        if (std::string(tag->Attribute("k")) == "highway") {
            n.type = tag->Attribute("v");
            break;
        }
    }
    
    nodes[n.id] = n;
}

void OSMParser::parseWay(tinyxml2::XMLElement* way) {
    Way w;
    w.id = way->Int64Attribute("id");
    
    // 解析way的类型和权重
    for (auto tag = way->FirstChildElement("tag"); tag; tag = tag->NextSiblingElement("tag")) {
        if (std::string(tag->Attribute("k")) == "highway") {
            w.type = tag->Attribute("v");
            w.weight_multiplier = getWeightMultiplier(w.type);
            break;
        }
    }
    
    // 只处理道路
    if (w.type.empty()) return;

    // 收集节点引用
    for (auto nd = way->FirstChildElement("nd"); nd; nd = nd->NextSiblingElement("nd")) {
        long long ref = nd->Int64Attribute("ref");
        w.node_refs.push_back(ref);
        nodes[ref].is_intersection = true;
    }
    
    ways.push_back(w);
}

double OSMParser::getWeightMultiplier(const std::string& type) {
    // 设置道路类型权重
    if (type == "motorway") return 0.5;
    if (type == "trunk") return 0.6;
    if (type == "primary") return 0.7;
    if (type == "secondary") return 0.8;
    if (type == "tertiary") return 0.9;
    if (type == "residential") return 1.0;
    if (type == "service") return 1.2;
    if (type == "footway") return 2.0;
    return 1.0;
}