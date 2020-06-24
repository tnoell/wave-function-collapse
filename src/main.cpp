#include "math.h"
#include "stdlib.h"
#include "time.h"
#include <iostream>
#include <array>
#include <vector>
#include <stack>
#include <functional>
#include <memory>

#include <algorithm>
#include <chrono>

#include "Grid.hpp"
#include "Image.hpp"
#include "Tile.hpp"


using namespace std;
using namespace std::chrono;


// ostream& operator<<(ostream& os, const std::vector<Object> vec)
// {
//     os << '{';
//     for (uint i = 0; i < vec.size(); i++)
//     {
//         if ((i + 1) < vec.size())
//         {
//             os << vec[i] << ", ";
//         }
//         else
//         {
//             os << vec[vec.size() - 1];
//         }
//     }

//     os << '}';
//     return os;
// }

std::vector<Tile> createTiles(int argc, char *argv[])
{
    const int tileCount = 7;
    std::vector<std::unique_ptr<Tile>> tilesPtrs;
    
    tilesPtrs.push_back(std::unique_ptr<Tile>(new Tile("tiles/GreenT.png", "GreenT")));
    tilesPtrs.push_back(std::unique_ptr<Tile>(new Tile("tiles/GreenCurve.png", "GreenCurve")));
    tilesPtrs.push_back(std::unique_ptr<Tile>(new Tile("tiles/BlueT.png", "BlueT")));
    tilesPtrs.push_back(std::unique_ptr<Tile>(new Tile("tiles/BlueStraight.png", "BlueStraight")));
    tilesPtrs.push_back(std::unique_ptr<Tile>(new Tile("tiles/BlueCross.png", "BlueCross")));
    tilesPtrs.push_back(std::unique_ptr<Tile>(new Tile("tiles/Crossing.png", "Crossing")));
    tilesPtrs.push_back(std::unique_ptr<Tile>(new Tile("tiles/Clear.png", "Clear", 5)));
    std::array<int, tileCount> tileRotations = {3, 3, 3, 1, 0, 1, 0};
    
    std::array<bool, tileCount> enabledTiles;
    for (unsigned int i = 0; i < enabledTiles.size(); i++)
    {
        enabledTiles[i] = true;
    }
    
    if (argc >= 5)
    {
        std::ifstream tileset;
        tileset.open(argv[4], std::ifstream::in);
        
        for (unsigned int i = 0; i < enabledTiles.size(); i++)
        {
            enabledTiles[i] = tileset.get() == 'y';
        }
    }
    
    for (int i = tilesPtrs.size() - 1; i >= 0; i--)
    {
        if (enabledTiles[i])
        {
            tilesPtrs[i]->addRotations(tilesPtrs, tileRotations[i]);
        }
        else
        {
            tilesPtrs.erase(tilesPtrs.begin() + i);
        }
    }
    
    Tile::matchTiles(tilesPtrs);
    
    std::vector<Tile> tiles;
    tiles.reserve(tilesPtrs.size());
    for (auto& tilePtr : tilesPtrs)
    {
        tiles.push_back(Tile(*tilePtr));
    }
    
    return tiles;
}


int main(int argc, char *argv[])
{
    // Image image;
    // image.copyImage();
    int width = 30, height = 30;
    int runs = 1;
    
    if (argc >= 3)
    {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    if (argc >= 4)
    {
        runs = atoi(argv[3]);
    }
    
    auto tiles = createTiles(argc, argv);
    
    std::chrono::milliseconds totalTime(0);
    try
    {
        for (int i = 0; i < runs; i++)
        {
            auto start = high_resolution_clock::now();
            
            Grid grid(tiles, width, height);
            grid.run();
            
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            totalTime += duration;
            std::cout  << "Took " << duration.count() << " milliseconds\n\n" << std::endl;
        }
        std::cout << "Total time: " << totalTime.count() << ", average: " << totalTime.count() / (float) runs << "\n";
    }
    catch(std::string s)
    {
        std::cerr << s << '\n';
    }
}