#pragma once

#include <iostream>
#include "rotation.h"

using uint = unsigned int;

#define ACTION_FILL "FILL"
#define ACTION_ERASE "ERASE"

struct RGB
{
    int red, green, blue;
};

struct Coord
{
    uint x;
    uint y;

    Coord(uint _x, uint _y) : x(_x), y(_y) {}

    bool operator==(const Coord &other) const
    {
        return x == other.x && y == other.y;
    }
};

namespace std
{
    template <>
    struct hash<Coord>
    {
        std::size_t operator()(const Coord &c) const noexcept
        {
            return std::hash<int>()(c.x) * 31 + std::hash<int>()(c.y);
        }
    };
}

struct Operation
{
    std::string action;
    Coord coord;
    uint size;

    Operation(const std::string &_action, const Coord _coord, uint _size = 1) : action(_action), coord(_coord), size(_size) {}

    friend std::ostream &operator<<(std::ostream &os, const Operation &o)
    {
        os << o.action << "," << o.coord.x << "," << o.coord.y;
        if (o.action == ACTION_FILL)
            os << "," << o.size;
        os << std::endl;
        return os;
    }
};

template <typename T>
class Image;
class Utils
{
public:
    static Image<RGB> DrawOperations(const std::vector<Operation> &operations, int width, int height, const std::vector<RGB>& colors);

    static int GetRotationValue(Rotation rotation)
    {
        switch (rotation)
        {
        case ROTATION_0:
            return 0;
            break;
        case ROTATION_90:
            return 90;
            break;
        case ROTATION_180:
            return 180;
            break;
        case ROTATION_270:
            return 270;
            break;
        default:
            std::cerr << "Rotation inconnue" << std::endl;
            exit(1);
        }
    }
    static Rotation GetRotationFromValue(int rotation)
    {
        while (rotation < 0)
            rotation += 360;
        rotation = rotation % 360;
        switch (rotation)
        {
        case 0:
            return ROTATION_0;
            break;
        case 90:
            return ROTATION_90;
            break;
        case 180:
            return ROTATION_180;
            break;
        case 270:
            return ROTATION_270;
            break;
        default:
            std::cerr << "Rotation inconnue" << std::endl;
            exit(1);
        }
    }
};