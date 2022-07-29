#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include <deque>
#include "bitmap.h"
#include "optimizer.h"

bool CheckResults(const std::vector<Operation> &operations, const std::deque<char> &inputFile, int width, int height, uint &i)
{
    std::vector<char> result(width * height, '*');
    for (const auto &op : operations)
    {
        for (auto y = op.coord.y; y < op.coord.y + op.size; y++)
            for (auto x = op.coord.x; x < op.coord.x + op.size; x++)
                result[y * width + x] = '#';
    }

    for (i = 0; i < result.size(); i++)
    {
        if (result[i] != inputFile[i])
            return false;
    }

    return true;
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

    Image<char> pixels(width, height);
    uint i = 0;
    for (uint y = 0; y < height; y++)
    {
        for (uint x = 0; x < width; x++)
        {
            pixels(y, x) = chars[i++];
        }
        chars.erase(chars.begin() + i);
    }

    auto start = std::chrono::steady_clock::now();

    Optimizer<char> optimizer(pixels);
    const auto &operations = optimizer.Optimize([](char c)
                                                { return c == '#' ? 1 : 0; });
    uint indErr;
    bool work = CheckResults(operations, chars, width, height, indErr);
    std::cerr << "Nb operations : " << operations.size() << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms" << std::endl;
    if (!work)
        std::cout << "Error at " << indErr / height << ", " << indErr % height << std::endl;
    else
        PrintOperations("output_0.txt", operations);

    std::vector<RGB> colors = {{230, 25, 75}, {60, 180, 75}, {255, 225, 25}, {0, 130, 200}, {245, 130, 48}, {145, 30, 180}, {70, 240, 240}, {240, 50, 230}, {210, 245, 60}, {250, 190, 212}, {0, 128, 128}, {220, 190, 255}, {170, 110, 40}, {255, 250, 200}, {128, 0, 0}, {170, 255, 195}, {128, 128, 0}, {255, 215, 180}, {0, 0, 128}, {128, 128, 128}};
    Image<RGB> rgbImage = Utils::DrawOperations(operations, width, height, colors);
    Bitmap bitmap(rgbImage);
    bitmap.Export("test.bmp");
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