#include "Tile.hpp"


//using namespace png;

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




png::rgb_pixel operator+(const png::rgb_pixel& a, const png::rgb_pixel& b)
{
    return png::rgb_pixel(a.red + b.red, a.blue + b.blue, a.green + b.green);
}

Tile::Tile(const std::string path, std::string name, int weight):
    image(path),
    name(name),
    edges(TileEdge::getEdges(image)),
    weight(weight)
{
    image.write("tiles/generated/" + name + ".png");
}

Tile::Tile(Image image, std::string name, int weight):
    image(image),
    name(name),
    edges(TileEdge::getEdges(image)),
    weight(weight)
{
    image.write("tiles/generated/" + name + ".png");
}

void Tile::matchTiles(std::vector<Tile>& tiles)
{
    if (tiles.size() > MAX_TILES)
    {
        throw std::string("More tiles (") + std::to_string(tiles.size()) + ") than slots in the bitset, need to adjust the constant in Tile.hpp";
    }
    for (uint i = 0; i < tiles.size(); i++)
    {
        for (uint j = i; j < tiles.size(); j++)
        {
            for (uint edge = 0; edge < 4; edge++)
            {
                uint oppositeEdge = 3 - edge;
                if (tiles[i].edges[edge] == tiles[j].edges[oppositeEdge])
                {
                    tiles[i].edgeMasks[edge].set(j);
                    tiles[j].edgeMasks[oppositeEdge].set(i);
                }
            }
        }
    }
}


void Tile::addRotations(std::vector<Tile>& tiles, int rotations, bool mirror) const
{
    assert(rotations > 0 && rotations < 4);
    Image lastRotation = image;
    for (int i = 0; i < rotations; i++)
    {
        Image rotatedImage(image.get_height(), image.get_width()); //switch width and height
        for (uint x = 0; x < image.get_width(); x++)
        {
            for (uint y = 0; y < image.get_height(); y++)
            {
                rotatedImage[x][image.get_height() - y - 1] = lastRotation[y][x];
            }
        }
        tiles.push_back(Tile(rotatedImage, name + "Rotated" + std::to_string(i+1), weight));
        lastRotation = rotatedImage;
    }
    if (mirror)
    {
        Image mirror(image.get_height(), image.get_width());
        for (uint x = 0; x < image.get_width(); x++)
        {
            for (uint y = 0; y < image.get_height(); y++)
            {
                mirror[y][x] = image[y][image.get_height() - x];
            }
        }
        Tile mirrorTile(mirror, name + "Mirrored", weight);
        tiles.push_back(mirrorTile);
        mirrorTile.addRotations(tiles, rotations, false);
    }
}
    
const std::bitset<MAX_TILES>& Tile::getEdgeMask(EdgeDirection edge) const
{
    return edgeMasks[edge];
}


void Tile::drawGrid(std::vector<std::vector<const Tile*>>& tiles)
{
    Image gridImage(tiles[0][0]->image.get_width() * tiles[0].size(), tiles[0][0]->image.get_height() * tiles.size());
    for (uint gridRow = 0; gridRow < tiles.size(); gridRow++)
    {
        for (uint gridColumn = 0; gridColumn < tiles[gridRow].size(); gridColumn++)
        {
            const Image& tileImage = tiles[gridRow][gridColumn]->image;
            uint gridRowPixel = gridRow * tileImage.get_height();
            uint gridColumnPixel = gridColumn * tileImage.get_width();
            for (uint tileRow = 0; tileRow < tileImage.get_height(); tileRow++)
            {
                for (uint tileColumn = 0; tileColumn < tileImage.get_width(); tileColumn++)
                {
                    uint y = gridRowPixel + tileRow;
                    uint x = gridColumnPixel + tileColumn;
                    gridImage[y][x] = tileImage[tileRow][tileColumn];
                }
            }
        }
    }
    gridImage.write("result.png");
}
    
// void Tile::setEdgeMask(EdgeDirection edge)
// {
//     return edgeMasks[edge];
// }