#pragma once

#include <vector>
#include "utils.h"

template <typename T>
class Image
{
private:
    std::vector<std::vector<T>> data;

public:
    Image() = default;

    Image(uint w, uint h)
    {
        data.resize(h);
        for (uint i = 0; i < h; i++)
            data[i].resize(w);
    }

    Image(const Image &copy) : data(copy.data) {}

    Image(uint w, uint h, T defaultVal) : data(h, std::vector<T>(w, defaultVal)) {}

    Image(const std::vector<std::vector<T>> &_data) : data(_data) {}
    Image(std::vector<std::vector<T>> &&_data) : data(std::move(_data)) {}

    const std::vector<std::vector<T>> &GetData() const { return data; }

    T &operator()(uint y, uint x)
    {
        return data[y][x];
    }

    const T &operator()(uint y, uint x) const
    {
        return data[y][x];
    }

    inline uint height() const { return data.size(); }
    inline uint width() const { return data[0].size(); }

    Coord GetSquareStartInRotation(const Coord &coord, const Rotation &rot, uint squareSize) const
    {
        int rotation = Utils::GetRotationValue(rot);
        switch (rot)
        {
        case ROTATION_0:
            return Coord(coord.x, coord.y);
            break;
        case ROTATION_90:
            return Coord(coord.x - squareSize + 1, coord.y);
            break;
        case ROTATION_180:
            return Coord(coord.x - squareSize + 1, coord.y - squareSize + 1);
            break;
        case ROTATION_270:
            return Coord(coord.x, coord.y - squareSize + 1);
            break;
        default:
            std::cerr << "Rotation inconnue" << std::endl;
            exit(1);
        }
    }
    
    Coord RotateCoordSquareSize(const Coord &coord, const Rotation &rotation, uint squareSize) const
    {
        switch (rotation)
        {
        case ROTATION_0:
            return Coord(coord.x, coord.y);
            break;
        case ROTATION_90:
            return Coord(coord.y, width() - coord.x - squareSize);
            break;
        case ROTATION_180:
            return Coord(width() - coord.x - squareSize, height() - coord.y - squareSize);
            break;
        case ROTATION_270:
            return Coord(height() - coord.y - squareSize, coord.x);
            break;
        default:
            std::cerr << "Rotation inconnue" << std::endl;
            exit(1);
        }
    }

    Coord RotateCoord(const Coord &coord, const Rotation &rot) const
    {
        int rotation = Utils::GetRotationValue(rot);
        uint newHeight = rotation % 180 == 0 ? height() : width();
        uint newWidth = rotation % 180 == 0 ? width() : height();
        switch (rot)
        {
        case ROTATION_0:
            return Coord(coord.y, coord.x);
            break;
        case ROTATION_90:
            return Coord(newWidth - coord.x - 1, coord.y);
            break;
        case ROTATION_180:
            return Coord(newWidth - coord.y - 1, newHeight - coord.x - 1);
            break;
        case ROTATION_270:
            return Coord(coord.x, newHeight - coord.y - 1);
            break;
        default:
            std::cerr << "Rotation inconnue" << std::endl;
            exit(1);
        }
    }

    static Image<T> RotateImage(Image<T> src, Rotation rot)
    {
        int rotation = Utils::GetRotationValue(rot);
        uint newHeight = rotation % 180 == 0 ? src.height() : src.width();
        uint newWidth = rotation % 180 == 0 ? src.width() : src.height();

        Image<T> res(newWidth, newHeight);

        for (uint y = 0; y < newHeight; y++)
        {
            for (uint x = 0; x < newWidth; x++)
            {
                T val;
                if (rotation == 0)
                    val = src(y, x);
                if (rotation == 90)
                    val = src(newWidth - x - 1, y);
                else if (rotation == 180)
                    val = src(newHeight - y - 1, newWidth - x - 1);
                else if (rotation == 270)
                    val = src(x, newHeight - y - 1);
                res(y, x) = val;
            }
        }

        return res;
    }

};