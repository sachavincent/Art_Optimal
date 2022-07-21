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

int **GetBiggestSquares(char **in, const uint width, const uint height)
{
    int **out = new int *[height];
    uint i = 0;

    for (uint y = 0; y < height; y++)
    {
        out[y] = new int[width];
        out[y][0] = in[i][0] == '#' ? 1 : 0;
    }

    for (uint x = 0; x < width; x++)
        out[0][x] = in[0][x] == '#' ? 1 : 0;

    /* Construct other entries of S[][]*/
    for (uint i = 1; i < height; i++)
    {
        for (uint j = 1; j < width; j++)
        {
            if (in[i][j] == '#')
                out[i][j] = std::min({out[i][j - 1], out[i - 1][j],
                                      out[i - 1][j - 1]}) +
                            1; // better of using min in case of arguments more than 2
            else
                out[i][j] = 0;
        }
    }
    return out;
}
void PrintOperations(const std::string &output, const std::vector<Operation> &operations)
{
    std::ofstream outputFile(output);
    for (const auto &op : operations)
        outputFile << op;
}

std::vector<Operation> CalcOperations(int **pixels, uint width, uint height)
{
    std::unordered_set<Coord> explored;
    std::vector<Operation> operations;

    for (int y = height - 1; y >= 0; y--)
    {
        for (int x = width - 1; x >= 0; x--)
        {
            if(explored.find(Coord(x, y)) != explored.end())
                continue;
            int nb = pixels[y][x];
            explored.insert(Coord(x, y));
            if (nb > 0)
            {
                for (int j = y; j >= y - nb; j--)
                    for (int i = x; i >= x - nb; i--)
                        explored.insert(Coord(i, j));

                operations.push_back(Operation(ACTION_FILL, y - nb, x - nb, nb));
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
    int **biggestSquares = GetBiggestSquares(pixels, width, height);
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