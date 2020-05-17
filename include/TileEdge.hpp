#ifndef TILEEDGE_H
#define TILEEDGE_H

#include <string>
#include <array>
#include <vector>

#include <png++/png.hpp>

#include "EdgeDirection.hpp"

class TileEdge
{
private:
    const std::vector<png::rgb_pixel> pixels; //always from left to right or top to bottom
    static std::vector<TileEdge> existingEdges;
    int existingEdgeIndex = -1;
public:
    TileEdge(const std::vector<png::rgb_pixel>& pixels);
    static std::array<TileEdge, 4> getEdges(png::image<png::rgb_pixel> image);
    bool operator==(const TileEdge&) const;
};
bool operator==(const png::rgb_pixel& a, const png::rgb_pixel& b);
bool operator!=(const png::rgb_pixel& a, const png::rgb_pixel& b);

#endif // TILEEDGE_H