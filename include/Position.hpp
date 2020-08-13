#ifndef POSITION_H
#define POSITION_H

#include <string>

#include "EdgeDirection.hpp"

struct Position
{
private:
public:
    int x;
    int y;
    Position get(const EdgeDirection& dir) const;
};


std::ostream& operator<<(std::ostream& os, const Position& pos);

#endif // POSITION_H