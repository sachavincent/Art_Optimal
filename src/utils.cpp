#include "utils.h"

#include "image.h"

Image<RGB> Utils::DrawOperations(const std::vector<Operation> &operations, int width, int height, const std::vector<RGB> &colors);
{
    Image<RGB> image(width, height);

    for (uint i = 0; i < operations.size(); i++)
    {
        const auto &op = operations[i];
        image(op.coord.x, op.coord.y) = colors[i % colors.size()];
    }

    return image;
}