#pragma once
#include "libraries.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

//screen related
bool IsOnScreen(Vector2 pos)
{
    if (pos.x > screenCheck[1].x && pos.y > screenCheck[1].y && pos.x < screenCheck[0].x && pos.y < screenCheck[0].y)
        return 1;
    return 0;
}
float Pixels(float pixels, bool opposite = false)
{
    if(opposite)
        return round(pixels / (3 + 3 * resolution));
    return round(pixels * (3 + 3 * resolution));
}

//transformative (math)
int AddIndex(int a, int b, int c = 0)
{
    a += b + c;
    if (a > 7)
        a -= 8;
    else if (a < 0)
        a += 8;
    return a;
}
float AddRotation(float a, float b, int c = 0)
{
    a += b + c;
    if (a > 359)
        a -= 360;
    else if (a < 0)
        a += 360;
    return a;
}
float HypotenuseLength(float a, float b)
{
    return sqrt(pow(a, 2) + pow(b, 2));
}
Vector2 HypotenuseCoordinates(Vector2 point, float hypotenuseLength, float angle)
{
    return { point.x + cos(angle) * hypotenuseLength, point.y + sin(angle) * hypotenuseLength };
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

int randomCoordinate()
{
    static std::default_random_engine rng;

    std::uniform_real_distribution<double> dist(-Pixels(19200, 1), Pixels(19200, 1));
    return floor(dist(rng));
}