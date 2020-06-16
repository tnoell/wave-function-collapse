#include "math.h"
#include "stdlib.h"
#include "time.h"
#include <iostream>
#include <array>
#include <vector>
#include <stack>
#include <functional>

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


int main(int argc, char *argv[])
{
    // Image image;
    // image.copyImage();
    int width = 3, height = 3;
    int runs = 1;
    if (argc >= 3)
    {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    if (argc == 4)
    {
        runs = atoi(argv[3]);
    }
    std::chrono::milliseconds totalTime(0);
    try
    {
        for (int i = 0; i < runs; i++)
        {
            auto start = high_resolution_clock::now();
            
            Tile tile("tiles/T.png", "T-Shape");
            std::vector<Tile> tiles = {tile};
            tile.addRotations(tiles);
            
            // tiles.push_back(Tile("tiles/Curve.png", "Curve"));
            // tiles[tiles.size() - 1].addRotations(tiles);
            
            // tiles.push_back(Tile("tiles/TBlue.png", "TBlue"));
            // tiles[tiles.size() - 1].addRotations(tiles);
            
            // tiles.push_back(Tile("tiles/BlueStraight.png", "BlueStraight"));
            // tiles[tiles.size() - 1].addRotations(tiles, 1);
            
            // tiles.push_back(Tile("tiles/BlueCross.png", "BlueCross"));
            
            // tiles.push_back(Tile("tiles/Crossing.png", "Crossing"));
            // tiles[tiles.size() - 1].addRotations(tiles, 1);
            
            // tiles.push_back(Tile("tiles/Clear.png", "Clear", 5));
            
            Tile::matchTiles(tiles);
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