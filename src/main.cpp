#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <unordered_set>
#include <deque>

using uint = unsigned int;

#define ACTION_FILL "FILL"
#define ACTION_ERASE "ERASE"

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
    uint x;
    uint y;
    uint size;

    Operation(std::string _action, uint _x, uint _y, uint _size = 1) : action(_action), x(_x), y(_y), size(_size) {}

    friend std::ostream &operator<<(std::ostream &os, const Operation &o)
    {
        os << o.action << "," << o.x << "," << o.y;
        if (o.action == ACTION_FILL)
            os << "," << o.size;
        os << std::endl;
        return os;
    }
};

int **GetBiggestSquares(int **pixels, const uint width, const uint height, const uint offsetX = 0, const uint offsetY = 0)
{
    int **out = new int *[height];

    for (uint y = 0; y < height; y++)
    {
        out[y] = new int[width];
        for (uint x = 0; x < width; x++)
            out[y][x] = INT_MAX;
    }

    for (uint y = 0; y < height; y++)
        out[y][width - 1] = pixels[y + offsetY][width - 1 + offsetX] > 0 ? 1 : 0;

    for (uint x = 0; x < width; x++)
        out[height - 1][x] = pixels[height - 1 + offsetY][x + offsetX] > 0 ? 1 : 0;

    for (int y = height - 2; y >= 0; y--)
    {
        for (int x = width - 2; x >= 0; x--)
        {
            std::cout << x << std::endl;
            if (pixels[y + offsetY][x + offsetX] == 1)
                out[y][x] = std::min({out[y][x + 1], out[y + 1][x], out[y + 1][x + 1]}) + 1;
            else
                out[y][x] = 0;
        }
    }
    return out;
}

void PrintOperations(const std::string &output, const std::vector<Operation> &operations)
{
    std::ofstream outputFile(output);
    for (const auto &op : operations)
        outputFile << op;
    outputFile.close();
}

void PrintDebug(const std::string &output, int **pixels, uint width, uint height)
{
    std::ofstream outputFile(output);
    for (uint y = 0; y < height; y++)
    {
        for (uint x = 0; x < width; x++)
            outputFile << pixels[y][x];
        outputFile << std::endl;
    }
    outputFile.close();
}
/*
void OptiSquares(int **in, const uint width, const uint height)
{

    for (uint y = 0; y < height - 1; y++)
    {
        for (uint x = 0; x < width - 1; x++)
        {
            int p = in[y][x];
            if (p > 0)
            {
                bool stop = false;
                uint sizeIncrease = 0;
                while (!stop)
                {
                    bool candidate = true;
                    for (uint j = y; j < y + p + sizeIncrease; j++)
                    {
                        if (in[j][x + p + sizeIncrease] <= 0)
                        {
                            candidate = false;
                            break;
                        }
                    }
                    if (candidate)
                    {
                        for (uint i = x; i < x + p + sizeIncrease; i++)
                        {
                            if (in[i + p + sizeIncrease][y] <= 0)
                            {
                                candidate = false;
                                break;
                            }
                        }
                    }
                    if (candidate)
                        sizeIncrease++;
                    else
                        stop = true;
                }
                if (sizeIncrease > 0)
                {
                    int **newSquares = GetBiggestSquares(in, sizeIncrease + p, sizeIncrease + p, x, y);

                    for (uint j = 0; j < p + sizeIncrease; j++)
                    {
                        for (uint i = 0; i < p + sizeIncrease; i++)
                        {
                            in[y + j][x + i] = newSquares[j][i];
                        }
                    }
                }
            }
        }
    }
}*/
std::vector<Operation> CalcOperations(int **pixels, uint width, uint height)
{
    std::unordered_set<Coord> explored;
    std::vector<Operation> operations;

    for (uint y = 0; y < height; y++)
    {
        for (uint x = 0; x < width; x++)
        {
            if (explored.find(Coord(x, y)) != explored.end())
                continue;
            int nb = pixels[y][x];
            explored.insert(Coord(x, y));
            if (nb > 0)
            {
                for (uint j = y; j < y + nb; j++)
                    for (uint i = x; i < x + nb; i++)
                        explored.insert(Coord(i, j));

                operations.push_back(Operation(ACTION_FILL, x, y, nb));
            }
        }
    }

    return operations;
}
std::vector<Operation> CalcOperationsV2(int **pixels, uint width, uint height)
{
    std::unordered_set<Coord> explored;
    std::vector<Operation> operations;

    for (uint y = height-1; y >=0; y--)
    {
        for (uint x = width -1; x >=0; x--)
        {
            if (explored.find(Coord(x, y)) != explored.end())
                continue;
            int nb = pixels[y][x];
            explored.insert(Coord(x, y));
            if (nb > 0)
            {
                for (uint j = y; j < y + nb; j++)
                    for (uint i = x; i < x + nb; i++)
                        explored.insert(Coord(i, j));

                operations.push_back(Operation(ACTION_FILL, x, y, nb));
            }
        }
    }

    return operations;
}
void ParseFile(char *inputFile)
{
    std::ifstream file(inputFile);
    std::ostringstream ss;
    ss << file.rdbuf();
    const std::string &s = ss.str();
    std::deque<char> chars(s.begin(), s.end());

    std::string size;
    char c = chars[0];
    while (c != '\n' && chars.size() > 0)
    {
        chars.pop_front();
        size += c;
        c = chars[0];
    }
    chars.pop_front();

    auto delimSize = size.find(',');
    uint width = std::stoi(size.substr(0, delimSize));
    uint height = std::stoi(size.substr(delimSize + 1));

    int **pixels = new int *[height];
    uint i = 0;
    for (uint y = 0; y < height; y++)
    {
        pixels[y] = new int[width];
        for (uint x = 0; x < width; x++)
        {
            pixels[y][x] = chars[i++] == '#' ? 1 : 0;
        }
        i++;
    }
    int **biggestSquares = GetBiggestSquares(pixels, width, height);
    PrintDebug("debug.txt", biggestSquares, width, height);
    OptiSquares(biggestSquares, width, height);
    PrintDebug("debug_2.txt", biggestSquares, width, height);
    const auto &operations = CalcOperations(biggestSquares, width, height);
    PrintOperations("output_0.txt", operations);

    for (uint y = 0; y < height; y++)
        delete[] pixels[y];
    delete[] pixels;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Input file must be specified." << std::endl;
        return -1;
    }

    ParseFile(argv[1]);
}