#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>
#include <bitset>
#include <memory>

#include <png++/png.hpp>

#include "TileEdge.hpp"

typedef png::image<png::rgb_pixel> Image;

const int MAX_TILES = 64; // increasing this will probably have a major performance impact, since the bitset won't be a single value any more; decreasing won't do anything because even bitset<1> is 8 bytes

class Tile
{
private:
    Image image;
    std::string name;
    std::array<TileEdge, 4> edges;
    std::array<std::bitset<MAX_TILES>, 4> edgeMasks;
    int weight;
    static Tile errorTile;
    static Tile unknownTile;
public:
    
    Tile(std::string path, std::string name, int weight = 1);
    Tile(Image image, std::string name, int weight = 1);
    Tile(const Tile& other);
    Tile& operator=(const Tile& other) = delete;
    // Tile& operator=(const Tile&& other);
    
    static void matchTiles(std::vector<std::unique_ptr<Tile>>& tiles);
    void addRotations(std::vector<std::unique_ptr<Tile>>& tiles, int rotations = 3, bool mirror = false) const;
    const std::bitset<MAX_TILES>& getEdgeMask(EdgeDirection edge) const;
    const std::string& getName() const { return name; }
    int getWeight() const { return weight; }
    static void drawGrid(std::vector<std::vector<const Tile*>>& tiles);
    static const Tile& getErrorTile() { return errorTile; };
    static const Tile& getUnknownTile() { return unknownTile; };
};

#endif // TILE_H