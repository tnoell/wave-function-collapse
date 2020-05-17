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


int main()
{
    // Image image;
    // image.copyImage();
    auto start = high_resolution_clock::now();
    Tile tile("tiles/T.png", "T-Shape");
    std::vector<Tile> tiles = {tile};
    tile.addRotations(tiles);
    Tile::matchTiles(tiles);
    Grid grid(tiles, 30, 30);
    try
    {
        grid.run();
    }
    catch(std::string s)
    {
        std::cerr << s << '\n';
    }
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout  << "Took " << duration.count() << " milliseconds\n\n" << std::endl;
}