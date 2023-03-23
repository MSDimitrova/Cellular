#pragma once
#include "libraries.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

//transformative
bool DirectionalSimilarity(Vector2 current, Vector2 past, Vector2 similar)
{
    if ((current.x - past.x) / abs(current.x - past.x) == similar.x && (current.y - past.y) / abs(current.y - past.y) == similar.y)
        return 1;
    return 0;
}

//screen related
float Pixels(float pixels)
{
    return pixels * (3 + 3 * resolution);
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