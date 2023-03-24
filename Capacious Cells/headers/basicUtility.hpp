#pragma once
#include "libraries.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

//screen related
float Pixels(float pixels)
{
    return round(pixels * (3 + 3 * resolution));
}

//transformative (math)
float HypotenuseLength(float a, float b)
{
    return sqrt(pow(a, 2) + pow(b, 2));
}
Vector2 HypotenuseCoordinates(Vector2 point, float hypotenuse, float angle)
{
    return { point.x + cos(angle) * hypotenuse, point.y + sin(angle) * hypotenuse };
}

//misc.
fs::path Path(const char* folder, const char* name, const char* extention, int iterator = -1)
{
    fs::path path;
    for (int i = 0; i < strlen(folder); i++)
        path += folder[i];
    if(folder[6] == 'a') //checks if folder is the assets folder which is the only one with resolution folders
        for (int i = 0; i < strlen(resolutionPath[resolution]); i++)
            path += resolutionPath[resolution][i];
    for (int i = 0; i < strlen(name); i++)
        path += name[i];
    if (iterator > -1)
        path += std::to_string(iterator);
    for (int i = 0; i < strlen(extention); i++)
        path += extention[i];
    return path;
}