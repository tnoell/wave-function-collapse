#ifndef GRID_H
#define GRID_H

#include <vector>
#include <unordered_set>
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
    std::vector<std::pair<float, uint>> entropies;
    std::vector<bool> dirtyEntropies;
    std::vector<std::array<std::bitset<MAX_TILES>, 4>> combinedEdgeMasks;
    
    Position getPosition(int i) const;
    void forEachInField(const std::bitset<MAX_TILES>& field, const std::function<void(const Tile&)>& func);
    uint selectFromField(const std::bitset<MAX_TILES>& field, const std::function<bool(const Tile&)>& func);
    float calculateEntropy(const std::bitset<MAX_TILES>& field);
    float checkEntropy(int i);
    void clearCache(uint index);
    int collapseOne();
    void collapseField(std::bitset<MAX_TILES>& field);
    void insertNeighbours(std::unordered_set<int>& set, const Position& pos) const;
    void propagateChanges(Position pos);
    int getIndex(const Position& pos) const;
    // int getIndex(Position pos) const;
    bool isValid(const Position& pos) const;
    // float getField(Position pos);
    
public:
    Grid(std::vector<Tile>& tiles, int height, int width);
    Grid(Grid& other) = delete;
    Grid(Grid&& other) = delete;
    Grid& operator=(const Grid&) = delete;
    Grid& operator=(const Grid&&) = delete;
    void run();
    void drawGrid();
    bool updateField(Position pos);
    bool inBounds(Position pos);
    std::bitset<MAX_TILES> combinedEdgeMask(Position pos, EdgeDirection edge);
    std::vector<std::bitset<MAX_TILES>>::iterator operator[](std::size_t i);
};

#endif // GRID_H