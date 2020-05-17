#ifndef GRID_H
#define GRID_H

#include <vector>
#include <bitset>
#include <functional>
#include <random>

#include "Tile.hpp"
#include "Position.hpp"

class Grid
{
private:
    const std::vector<Tile>& tiles;
    const int height;
    const int width;
    std::mt19937 randGen;
    std::vector<std::bitset<MAX_TILES>> fields;
    
    Position getPosition(int i) const;
    int getIndex(const Position& pos) const;
    void forEachInField(const std::bitset<MAX_TILES>& field, std::function<void(Tile)> func);
    uint selectFromField(const std::bitset<MAX_TILES>& field, std::function<bool(Tile)> func);
    float calculateEntropy(const std::bitset<MAX_TILES>& field);
    int collapseOne();
    void collapseField(std::bitset<MAX_TILES>& field);
    void propagateChanges(Position pos);
    
public:
    Grid(std::vector<Tile>& tiles, int height, int width);
    Grid(Grid& other) = delete;
    Grid(Grid&& other) = delete;
    Grid& operator=(const Grid&) = delete;
    Grid& operator=(const Grid&&) = delete;
    void run();
    bool updateField(Position pos);
    bool inBounds(Position pos);
    std::bitset<MAX_TILES> combinedEdgeMask(Position pos, EdgeDirection edge);
    std::vector<std::bitset<MAX_TILES>>::iterator operator[](std::size_t i);
};

#endif // GRID_H