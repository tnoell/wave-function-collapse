#include "Grid.hpp"

#include <cmath>
#include <ctime>

#include <unordered_set>

Grid::Grid(std::vector<Tile>& tiles, int height, int width):
    tiles(tiles),
    height(height),
    width(width),
    randGen(std::time(0))
{
    std::bitset<MAX_TILES> bits;
    for (uint i = 0; i < tiles.size(); i++)
    {
        bits.set(i); //all tiles can be possible at first
    }
    fields.resize(height * width, bits);
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
            int iTile = selectFromField((*this)[y][x], [](Tile t) {return true;});
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
    float sumWeight = 0;
    float sumWeightLogWeight = 0;
    forEachInField(field, [&](Tile t) {
        sumWeight += t.weight;
        sumWeightLogWeight += t.weight * log(t.weight);
    });
    return log(sumWeight) - sumWeightLogWeight / sumWeight;
}

int Grid::collapseOne()
{
    int iMaxEntropy = -1;
    float maxEntropy = 0;
    for (uint i = 0; i < fields.size(); i++)
    {
        float entropy = calculateEntropy(fields[i]);
        //std::cout << fields[i] << " entropy: " << entropy << '\n';
        if (entropy > maxEntropy)
        {
            maxEntropy = entropy;
            iMaxEntropy = i;
        }
    }
    if (iMaxEntropy != -1)
    {
        std::cout << "collapsing field " << iMaxEntropy << std::endl;
        collapseField(fields[iMaxEntropy]);
    }
    return iMaxEntropy;
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
            dirtyPositions.insert(getIndex(pos.get(top)));
            dirtyPositions.insert(getIndex(pos.get(left)));
            dirtyPositions.insert(getIndex(pos.get(right)));
            dirtyPositions.insert(getIndex(pos.get(bottom)));
        }
    }
}

bool Grid::updateField(Position pos)
{
    if ((*this)[pos.y][pos.x].count() == 1)
    { return false; }
    std::bitset<MAX_TILES> before = (*this)[pos.y][pos.x];
    (*this)[pos.y][pos.x] = combinedEdgeMask(pos.get(top), bottom) & combinedEdgeMask(pos.get(right), left)
    & combinedEdgeMask(pos.get(left), right) & combinedEdgeMask(pos.get(bottom), top);
    //if (before != (*this)[pos.y][pos.x]) std::cout << pos << " before:\n" << before << ", after:\n" << (*this)[pos.y][pos.x] << "\n";
    if ((*this)[pos.y][pos.x].none())
    {
        throw std::string("contradiction encountered");
    }
    return before != (*this)[pos.y][pos.x];
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
    std::bitset<MAX_TILES> field = (*this)[pos.y][pos.x];
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

std::vector<std::bitset<MAX_TILES>>::iterator Grid::operator[](std::size_t y) // to access individual elements by doing grid[y][x]
{
    //std::cout << "accessing row starting at " << std::distance( fields.begin(), fields.begin() + y * width ) << '\n';
    return fields.begin() + y * width;
}