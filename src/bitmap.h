#pragma once

#include "image.h"
#include <fstream>

class Bitmap
{
private:
    Image<RGB> image;

public:
    Bitmap(Image<RGB> img) : image(img) {}

    void Export(const std::string &fileName)
    {
        std::ofstream f;
        f.open(fileName, std::ios::out | std::ios::binary);

        if (!f.is_open())
        {
            std::cerr << "File " << fileName << " couldn't be opened." << std::endl;
            return;
        }

        uint width = image.width();
        uint height = image.height();

        unsigned char bmpPad[3] = {0, 0, 0};
        const int paddingAmount = ((4 - (width * 3) % 4) % 4);

        const int fileHeaderSize = 14;
        const int informationHeaderSize = 40;
        const int fileSize = fileHeaderSize + informationHeaderSize + width * height * 3 + paddingAmount * height;

        unsigned char fileHeader[fileHeaderSize];

        // File type
        fileHeader[0] = 'B';
        fileHeader[1] = 'M';
        // File size
        fileHeader[2] = fileSize;
        fileHeader[3] = fileSize >> 8;
        fileHeader[4] = fileSize >> 16;
        fileHeader[5] = fileSize >> 24;
        // Reserved 1 (Not used)
        fileHeader[6] = 0;
        fileHeader[7] = 0;
        // Reserved 2 (Not used)
        fileHeader[8] = 0;
        fileHeader[9] = 0;
        // Pixel data offset
        fileHeader[10] = fileHeaderSize + informationHeaderSize;
        fileHeader[11] = 0;
        fileHeader[12] = 0;
        fileHeader[13] = 0;

        unsigned char informationHeader[informationHeaderSize];

        // Header size
        informationHeader[0] = informationHeaderSize;
        informationHeader[1] = 0;
        informationHeader[2] = 0;
        informationHeader[3] = 0;
        // Image width
        informationHeader[4] = width;
        informationHeader[5] = width >> 8;
        informationHeader[6] = width >> 16;
        informationHeader[7] = width >> 24;
        // Image height
        informationHeader[8] = height;
        informationHeader[9] = height >> 8;
        informationHeader[10] = height >> 16;
        informationHeader[11] = height >> 24;
        // Planes
        informationHeader[12] = 1;
        informationHeader[13] = 0;
        // Bits per pixel (RGB)
        informationHeader[14] = 24;
        informationHeader[15] = 0;
        // Compression (No compression)
        informationHeader[16] = 0;
        informationHeader[17] = 0;
        informationHeader[18] = 0;
        informationHeader[19] = 0;
        // Image size (No compression)
        informationHeader[20] = 0;
        informationHeader[21] = 0;
        informationHeader[22] = 0;
        informationHeader[23] = 0;
        // X pixels per meter (Not specified)
        informationHeader[24] = 0;
        informationHeader[25] = 0;
        informationHeader[26] = 0;
        informationHeader[27] = 0;
        // Y pixels per meter (Not specified)
        informationHeader[28] = 0;
        informationHeader[29] = 0;
        informationHeader[30] = 0;
        informationHeader[31] = 0;
        // Total colors (Color palette not used)
        informationHeader[32] = 0;
        informationHeader[33] = 0;
        informationHeader[34] = 0;
        informationHeader[35] = 0;
        // Important colors (Generally ignored)
        informationHeader[36] = 0;
        informationHeader[37] = 0;
        informationHeader[38] = 0;
        informationHeader[39] = 0;

        f.write(reinterpret_cast<char *>(fileHeader), fileHeaderSize);
        f.write(reinterpret_cast<char *>(informationHeader), informationHeaderSize);

        for (uint y = 0; y < height; y++)
        {
            for (uint x = 0; x < width; x++)
            {
                RGB rgb = image(y, x);
                unsigned char r = (unsigned char)rgb.red;
                unsigned char g = (unsigned char)rgb.green;
                unsigned char b = (unsigned char)rgb.blue;

                unsigned char color[] = {b, g, r};
                f.write(reinterpret_cast<char *>(color), 3);
            }
            f.write(reinterpret_cast<char *>(bmpPad), paddingAmount);
        }

        f.close();
    }
};