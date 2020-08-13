#include "Position.hpp"

#include <iostream>

Position Position::get(const EdgeDirection& dir) const
{
    int newX = x;
    int newY = y;
    switch(dir)
    {
    case top :
        newY--;
        break;
    case left :
        newX--;
        break;
    case right : 
        newX++;
        break;
    case bottom :
        newY++;
        break;
    }
    return Position{newX, newY};
}

std::ostream& operator<<(std::ostream& os, const Position& pos)
{
    os << "[x: " << std::to_string(pos.x) << ", y: " << std::to_string(pos.y) << "]";
    return os;
}