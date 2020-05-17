#include "TileEdge.hpp"

using namespace png;

std::vector<TileEdge> TileEdge::existingEdges;

TileEdge::TileEdge(const std::vector<png::rgb_pixel>& pixels):
pixels(pixels)
{
    bool match = false;
    for (uint iEdge = 0; iEdge < existingEdges.size(); iEdge++)
    {
        match = true;
        for (uint i = 0; i < pixels.size(); i++)
        {
            if (pixels[i] != existingEdges[iEdge].pixels.at(i))
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            existingEdgeIndex = iEdge;
            break;
        }
    }
    if (!match)
    {
        existingEdgeIndex = existingEdges.size();
        existingEdges.push_back(*this);
    }
}

std::array<TileEdge, 4> TileEdge::getEdges(png::image<png::rgb_pixel> image)
{
    std::vector<png::rgb_pixel> edgePixels[4];
    for (uint x = 0; x < image.get_width(); x++)
    {
        edgePixels[top].push_back(image[0][x]);
        edgePixels[bottom].push_back(image[image.get_height() - 1][x]);
    }

    for (uint y = 0; y < image.get_width(); y++)
    {
        edgePixels[left].push_back(image[y][0]);
        edgePixels[right].push_back(image[y][image.get_width() - 1]);
    }
    std::array<TileEdge, 4> edges = {edgePixels[0], edgePixels[1], edgePixels[2], edgePixels[3]};
    return edges;
}

bool TileEdge::operator==(const TileEdge& other) const
{
    return existingEdgeIndex == other.existingEdgeIndex;
}


bool operator==(const rgb_pixel& a, const rgb_pixel& b)
{
    return a.red == b.red && a.blue == b.blue && a.green == b.green;
}

bool operator!=(const rgb_pixel& a, const rgb_pixel& b)
{
    return !(a == b);
}