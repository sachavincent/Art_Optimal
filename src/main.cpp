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

void PrintOperations(const std::string &output, const std::vector<Operation> &operations)
{
    std::ofstream outputFile(output);
    for (const auto &op : operations)
        outputFile << op;
}

std::vector<Operation> CalcOperations(char **pixels, uint width, uint height)
{
    std::unordered_set<Coord> explored;
    std::vector<Operation> operations;
    for (uint x = 0; x < width; x++)
    {
        for (uint y = 0; y < height; y++)
        {
            Coord coord(x, y);
            if (explored.find(coord) != explored.end())
                continue;
            char c = pixels[y][x];
            if (c == '#')
            {
                uint size = 1;
                uint i = 1;
                std::unordered_set<Coord> exploredTmp;
                while (x + i < width && y + i < height)
                {
                    std::unordered_set<Coord> exploredTmp2;
                    for (uint cy = y; cy <= y + i; cy++)
                    {
                        for (uint cx = x; cx <= x + i; cx++)
                        {
                            if (explored.find({cx, cy}) != explored.end())
                                goto end;
                            exploredTmp2.insert({cx, cy});

                            if (pixels[cy][cx] != '#')
                                goto end;
                        }
                    }
                    for (const auto &c : exploredTmp2)
                        exploredTmp.insert(c);
                    size++;

                    i++;
                }
            end:
                for (const auto &c : exploredTmp)
                    explored.insert(c);
                operations.push_back(Operation(ACTION_FILL, x, y, size));
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

    char **pixels = new char *[height];
    uint i = 0;
    for (uint y = 0; y < height; y++)
    {
        pixels[y] = new char[width];
        for (uint x = 0; x < width; x++)
        {
            pixels[y][x] = chars[i++];
        }
        i++;
    }

    const auto &operations = CalcOperations(pixels, width, height);
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