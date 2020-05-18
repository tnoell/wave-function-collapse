#include "Grid.hpp"

#include <cmath>

#include <chrono>
#include <unordered_set>
#include <limits>

Grid::Grid(std::vector<Tile>& tiles, int height, int width):
    tiles(tiles),
    height(height),
    width(width),
    randGen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
{
    std::bitset<MAX_TILES> bits;
    for (uint i = 0; i < tiles.size(); i++)
    {
        bits.set(i); //all tiles can be possible at first
    }
    fields.resize(height * width, bits);
    entropies.resize(fields.size(), -1);
}

void Grid::run()
{
    int collapsed = collapseOne();
    while (collapsed != -1)
    {
        propagateChanges(getPosition(collapsed));
        collapsed = collapseOne();
    }
    
    std::vector<std::vector<const Tile*>> tiles2d;
    for (int y = 0; y < height; y++)
    {
        tiles2d.push_back(std::vector<const Tile*>());
        for (int x = 0; x < width; x++)
        {
            int iTile = selectFromField(fields[getIndex({x, y})], [](Tile t) {return true;});
            tiles2d[y].push_back(&(tiles[iTile]));
        }
    }
    Tile::drawGrid(tiles2d);
}

Position Grid::getPosition(int i) const
{
    return Position{.x = i % width, .y = i / width};
}

int Grid::getIndex(const Position& pos) const
{
    return pos.y * width + pos.x;
}

void Grid::forEachInField(const std::bitset<MAX_TILES>& field, std::function<void(Tile)> func)
{
    for (uint i = 0; i < tiles.size(); i++)
    {
        if (field[i])
        {
            func(tiles[i]);
        }
    }
}

uint Grid::selectFromField(const std::bitset<MAX_TILES>& field, std::function<bool(Tile)> func)
{
    for (uint i = 0; i < tiles.size(); i++)
    {
        if (field[i])
        {
            if (func(tiles[i]))
            {
                return i;
            }
        }
    }
    throw std::string("No element found. Is this expected? Maybe there is a contradiction?"); //return -1;
}

float Grid::calculateEntropy(const std::bitset<MAX_TILES>& field)
{
    int count = 0;
    float sumWeight = 0;
    float sumWeightLogWeight = 0;
    forEachInField(field, [&](Tile t) {
        count++;
        sumWeight += t.weight;
        sumWeightLogWeight += t.weight * log(t.weight);
    });
    if (count == 1)
    { return std::numeric_limits<float>::infinity(); } //mark entropy as infinity for completed fields, so they don't get picked in CollapseOne
    return log(sumWeight) - sumWeightLogWeight / sumWeight;
}

float Grid::checkEntropy(int i)
{
    if (entropies[i] < 0)
    {
        entropies[i] = calculateEntropy(fields[i]);
    }
    return entropies[i];
}

int Grid::collapseOne()
{
    int iMinEntropy = -1;
    float minEntropy = std::numeric_limits<float>::infinity();
    for (uint i = 0; i < fields.size(); i++)
    {
        float entropy = checkEntropy(i);
        //std::cout << fields[i] << " entropy: " << entropy << '\n';
        if (entropy < minEntropy)
        {
            minEntropy = entropy;
            iMinEntropy = i;
        }
    }
    if (iMinEntropy != -1)
    {
        std::cout << "collapsing field " << iMinEntropy << std::endl;
        collapseField(fields[iMinEntropy]);
        entropies[iMinEntropy] = std::numeric_limits<float>::infinity();
    }
    return iMinEntropy;
}

void Grid::collapseField(std::bitset<MAX_TILES>& field)
{
    int sumWeight = 0;
    forEachInField(field, [&](Tile t) {
        sumWeight += t.weight;
    });
    int rnd = randGen() % sumWeight;
    
    uint iTile = selectFromField(field, [&](Tile t) -> bool {
        if (rnd < t.weight)
        {
            return true;
        }
        rnd -= t.weight;
        return false;
    });
    field.reset();
    field.set(iTile);
    std::cout << "collapsed field to " << tiles[iTile].getName() << std::endl;
}

void Grid::propagateChanges(Position startPos)
{
    // auto hash = [&width](const Position& pos) { return std::hash<uint>{}()}
    std::unordered_set<int> dirtyPositions; //may contain indices out of range
    dirtyPositions.insert(getIndex(startPos.get(top)));
    dirtyPositions.insert(getIndex(startPos.get(left)));
    dirtyPositions.insert(getIndex(startPos.get(right)));
    dirtyPositions.insert(getIndex(startPos.get(bottom)));
    while (!dirtyPositions.empty())
    {
        auto it = dirtyPositions.begin();
        Position pos = getPosition(*it);
        dirtyPositions.erase(it);
        if (inBounds(pos) && updateField(pos))
        {
            entropies[getIndex(pos)] = -1;
            dirtyPositions.insert(getIndex(pos.get(top)));
            dirtyPositions.insert(getIndex(pos.get(left)));
            dirtyPositions.insert(getIndex(pos.get(right)));
            dirtyPositions.insert(getIndex(pos.get(bottom)));
        }
    }
}

bool Grid::updateField(Position pos)
{
    if (fields[getIndex(pos)].count() == 1)
    { return false; }
    std::bitset<MAX_TILES> before = fields[getIndex(pos)];
    fields[getIndex(pos)] = combinedEdgeMask(pos.get(top), bottom) & combinedEdgeMask(pos.get(right), left)
    & combinedEdgeMask(pos.get(left), right) & combinedEdgeMask(pos.get(bottom), top);
    //if (before != fields[getIndex(pos)]) std::cout << pos << " before:\n" << before << ", after:\n" << fields[getIndex(pos)] << "\n";
    if (fields[getIndex(pos)].none())
    {
        throw std::string("contradiction encountered");
    }
    return before != fields[getIndex(pos)];
}

bool Grid::inBounds(Position pos)
{
    return pos.y >= 0 && pos.y < height && pos.x >= 0 && pos.x < width;
}

std::bitset<MAX_TILES> Grid::combinedEdgeMask(Position pos, EdgeDirection edge)
{
    std::bitset<MAX_TILES> mask;
    if (!inBounds(pos))
    {
        mask.set();
        return mask;
    }
    //std::cout << "accessing field " << std::distance(fields.begin(), ((*this)[pos.y]+pos.x)) << '\n';
    std::bitset<MAX_TILES> field = fields[getIndex(pos)];
    forEachInField(field, [&] (Tile t) {
       mask |= t.getEdgeMask(edge); 
       //std::cout << t.getName() << " mask:\n" << t.getEdgeMask(edge) << std::endl;
    });
    //std::cout << pos << " combined edge mask in direction " << edge << ":\n" << mask << std::endl; 
    // for (uint i = 0; i < tiles.size(); i++)
    // {
    //     if (field[i])
    //     {
    //         mask |= tiles[i].getEdgeMask(edge);
    //     }
    // }
    return mask;
}


int Grid::getIndex(Position pos)
{
    return pos.y * width + pos.x;
}


// std::vector<std::bitset<MAX_TILES>>::iterator Grid::operator[](std::size_t y) // to access individual elements by doing grid[y][x]
// {
//     //std::cout << "accessing row starting at " << std::distance( fields.begin(), fields.begin() + y * width ) << '\n';
//     return fields.begin() + y * width;
// }