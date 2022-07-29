#pragma once

#include "image.h"

#include <unordered_map>
#include <functional>
#include <cassert>
#include <iterator>

template <typename T>
class Optimizer
{
private:
    Image<T> image;

    std::unordered_map<Rotation, Image<int>> listBiggestSquares;

public:
    Optimizer(const Image<T> &img) : image(img) {}

    std::vector<Operation> Optimize(const std::function<int(T)> &transformToInt)
    {
        listBiggestSquares[ROTATION_0] = FindBiggestSquares(image, transformToInt);
        listBiggestSquares[ROTATION_90] = FindBiggestSquares(Image<T>::RotateImage(image, ROTATION_90), transformToInt);
        listBiggestSquares[ROTATION_180] = FindBiggestSquares(Image<T>::RotateImage(image, ROTATION_180), transformToInt);
        listBiggestSquares[ROTATION_270] = FindBiggestSquares(Image<T>::RotateImage(image, ROTATION_270), transformToInt);

        std::vector<Operation> operations;
        const auto &image = listBiggestSquares[ROTATION_0];
        Image<int> tmpImage = image; // To avoid testing the same squares over and over again

        std::unordered_set<Coord> foundCorners;
        for (uint y = 0; y < image.height(); y++)
        {
            for (uint x = 0; x < image.width(); x++)
            {
                if ((x == image.width() - 1 && y == 0) || (x == 0 && y == 0) || (x == image.width() - 1 && y == image.height() - 1) || (x == 0 && y == image.height() - 1))
                    std::cout << "";
                Rotation rotation;

                if (tmpImage(y, x) == 0)
                    continue;

                int xP = x == 0 ? 0 : image(y, x - 1);
                int yP = y == 0 ? 0 : image(y - 1, x);
                int xN = x == image.width() - 1 ? 0 : image(y, x + 1);
                int yN = y == image.height() - 1 ? 0 : image(y + 1, x);

                if (yP == 0 && xP == 0) // Corner rot 0
                    rotation = ROTATION_0;
                else if (yP == 0 && xN == 0) // Corner rot 90
                    rotation = ROTATION_270;
                else if (yN == 0 && xN == 0) // Corner rot 180
                    rotation = ROTATION_180;
                else if (yN == 0 && xP == 0) // Corner rot 270
                    rotation = ROTATION_90;
                else
                    continue;

                Rotation invRot = Utils::GetRotationFromValue(360 - Utils::GetRotationValue(rotation));
                Coord coord = image.RotateCoord(Coord(y, x), rotation);
                const int nb = listBiggestSquares[rotation](coord.y, coord.x);
                assert(nb > 0);
                Coord squareStartPos = image.GetSquareStartInRotation(Coord(x, y), invRot, nb);
                assert(squareStartPos.x + nb - 1 < image.width() && squareStartPos.y + nb - 1 < image.height());
                operations.push_back(Operation(ACTION_FILL, squareStartPos, nb));

                // Erasing the square found in tmpImage
                for (uint j = squareStartPos.y; j < squareStartPos.y + nb; j++)
                    for (uint i = squareStartPos.x; i < squareStartPos.x + nb; i++)
                    {
                        foundCorners.insert(Coord(i, j));
                        tmpImage(j, i) = 0;
                    }
            }
        }

        std::vector<Operation> bestFound;
        size_t bestSizeFound = UINT_MAX;
        for (const auto &it : listBiggestSquares)
        {
            const auto &op = GetOperationsFromBiggestSquares(it.second, it.first, foundCorners);
            if (op.size() < bestSizeFound)
            {
                bestSizeFound = op.size();
                bestFound = op;
            }
        }
        operations.insert(operations.end(), bestFound.begin(), bestFound.end());
        return operations;
    }
/*
    static std::vector<Operation> GetOperationsFirstPassOnCorners(Image<int> &image, Rotation rotation)
    {
        std::unordered_set<Coord> foundSquares;
        std::vector<Operation> operations;
        for (uint y = 0; y < image.height(); y++)
        {
            for (uint x = 0; x < image.width(); x++)
            {
                Coord c = image.RotateCoord(Coord(x, y), rotation);
                int nb = image(y, x);
                if (nb > 0 && ((y == 0 && x == 0) || (x > 0 && y > 0 && image(y - 1, x) == 0 && image(y, x - 1) == 0))) // Corner
                {
                    for (uint j = y; j < y + nb; j++)
                        for (uint i = x; i < x + nb; i++)
                            foundSquares.insert(Coord(j, i));
                    Coord coord = image.RotateCoordSquareSize(Coord(x, y), rotation, nb);
                    operations.push_back(Operation(ACTION_FILL, coord, nb));
                }
            }
        }
        for (auto &coord : foundSquares)
            image(coord.x, coord.y) = 0;
        return operations;
    }*/

    static std::vector<Operation> GetOperationsFromBiggestSquares(const Image<int> &image, const Rotation &rotation, const std::unordered_set<Coord> &toIgnore)
    {
        std::vector<Operation> operations;
        std::unordered_set<Coord> explored;
        for (uint y = 0; y < image.height(); y++)
        {
            for (uint x = 0; x < image.width(); x++)
            {
                Coord c = Coord(x, y);
                int nb = image(y, x);
                if (nb > 0)
                {
                    Rotation invRot = Utils::GetRotationFromValue(360 - Utils::GetRotationValue(rotation));
                    if (toIgnore.find(image.RotateCoord(Coord(y, x), invRot)) != toIgnore.end())
                        continue;
                    if (explored.find(c) != explored.end())
                        continue;
                    explored.insert(c);
                    for (uint j = y; j < y + nb; j++)
                        for (uint i = x; i < x + nb; i++)
                            explored.insert(Coord(i, j));

                    Coord coord = image.RotateCoordSquareSize(Coord(x, y), rotation, nb);
                    operations.push_back(Operation(ACTION_FILL, coord, nb));
                }
            }
        }

        return operations;
    }
    static Image<int> FindBiggestSquares(const Image<T> &image, const std::function<int(T)> &transformToInt)
    {
        Image<int> dst(image.width(), image.height(), INT_MAX);

        for (uint y = 0; y < image.height(); y++)
            dst(y, image.width() - 1) = transformToInt(image(y, image.width() - 1)) > 0 ? 1 : 0;

        for (uint x = 0; x < image.width(); x++)
            dst(image.height() - 1, x) = transformToInt(image(image.height() - 1, x)) > 0 ? 1 : 0;

        for (int y = image.height() - 2; y >= 0; y--)
        {
            for (int x = image.width() - 2; x >= 0; x--)
            {
                if (transformToInt(image(y, x)) == 1)
                    dst(y, x) = std::min({dst(y, x + 1), dst(y + 1, x), dst(y + 1, x + 1)}) + 1;
                else
                    dst(y, x) = 0;
            }
        }
        return dst;
    }
};