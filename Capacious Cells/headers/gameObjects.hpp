#pragma once
#include "libraries.hpp"

struct Prefab
{
    std::vector<Texture2D> sprite;
    json data;

    void Setup(const char* name)
    {
        if (fs::exists(Path(assetsFolder, name, pngExtention)))
            sprite.push_back(LoadTexture(Path(assetsFolder, name, pngExtention).string().c_str()));
        else
            for (int i = 0; i < 63; i++)
                if (fs::exists(Path(assetsFolder, name, pngExtention, i)))
                    sprite.push_back(LoadTexture(Path(assetsFolder, name, pngExtention, i).string().c_str()));
                else
                    break;
        if (fs::exists(Path(prefabsFolder, name, jsonExtention)))
        {
            std::ifstream f(Path(prefabsFolder, name, jsonExtention));
            data = json::parse(f);
        }
    }
} spike, canon, bristles, tail;

struct GameObject
{
    int frameSprite = 0, animationFPS = 0; //set animation frames
    int frameScreen = 1, frameCurrent = 1; //running frames on screen
    float rotation = 0;
    Vector2 pos = CENTER;
    Vector2 size = { 10, 10 };
    std::vector<Texture2D*> sprite;
    Texture2D* currentSprite = &missingTexture;

    void Animate()
    {
        currentSprite = sprite[frameSprite];
        frameScreen++;
        if (frameScreen == 61)
            frameScreen = 1;
        if((frameScreen > animationFPS * frameCurrent) || frameScreen == 1)
        {
            frameSprite++;
            frameCurrent++;
        }
        if (frameCurrent == 60 / animationFPS + 1)
            frameCurrent = 1;
        if (frameSprite == sprite.size())
            frameSprite = 0;
    }

    void UpdateSprite(Texture2D* _sprite = &dummyTexture, int _i = 0)
    {
        if (_sprite != &dummyTexture)
            if (sprite.size() > _i)
                sprite[_i] = _sprite;
            else
                sprite.push_back(_sprite);
        size.x = sprite[_i]->width;
        size.y = sprite[_i]->height;
        currentSprite = sprite[_i];
    }
};

struct Equipment : GameObject
{
    bool boostType = 0; //dmg or speed
    int lvl = 1;
    int slot = 0;
    int cost = 0;
    int boost = 0;
    std::string name = "none"; //determines kind of equipment

    void UpdatePos(GameObject& parent, int rotationIndex)
    {
        float prepRotation = parent.rotation;
        if (prepRotation < 0)
            prepRotation += 360;
        if (slot % 2)
            prepRotation *= -1;
        Vector2 prepMargin = cellMargin[slot];
        if(slot % 2)
        {
            switch (rotationIndex)
            {
            case 0:
                prepMargin.x = prepMargin.x - prepRotation * prepMargin.x / 90;
                break;
            case 2:
                prepMargin.x = prepRotation * prepMargin.x / 90;
                break;
            case 4:
                prepMargin.x = prepMargin.x - prepRotation * prepMargin.x / 90;
                break;
            case 6:
                prepMargin.x = prepRotation * prepMargin.x / 90;
                break;
            }
        }
        else
        {
            switch (rotationIndex)
            {
            case 0:
                prepMargin.y -= (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            case 1:
                prepMargin.y -= (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            case 2:
                prepMargin.y += (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            case 3:
                prepMargin.y += (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            case 4:
                prepMargin.y -= (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            case 5:
                prepMargin.y -= (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            case 6:
                prepMargin.y += (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            case 7:
                prepMargin.y += (prepRotation - rotationIndex * 45) * prepMargin.y / 45;
                break;
            }
        }
        prepRotation /= 57.29578f;

        Vector2 prepPos = { parent.pos.x + prepMargin.x + directionPos[slot * 2].x * size.x / 2,
            parent.pos.y + prepMargin.y + directionPos[slot * 2].y * size.x / 2 };
        
        pos = { ((prepPos.x - parent.pos.x) * cos(prepRotation)) - ((parent.pos.y - prepPos.y) * sin(prepRotation)) + parent.pos.x,
            parent.pos.y - ((parent.pos.y - prepPos.y) * cos(prepRotation)) + ((prepPos.x - parent.pos.x) * sin(prepRotation)) };

        rotation = parent.rotation + slot * 90;
    }

    void Setup(Prefab& prefab)
    {
        //assign prefab values to this's values
        if (prefab.data.contains("animationFPS"))
            animationFPS = prefab.data["animationFPS"];
        if (prefab.data.contains("boostType"))
            boostType = 1;
        cost = prefab.data["cost"];
        boost = prefab.data["boost"];
        name = prefab.data["name"];

        //reduce size of this's sprite array to size of prefab sprite array to be assigned
        if (sprite.size() > prefab.sprite.size())
            for (int i = sprite.size(); i > prefab.sprite.size(); i--)
                sprite.pop_back();

        //assign prefab sprite array to this's sprite array
        for (int i = 0; i < prefab.sprite.size(); i++)
            UpdateSprite(&prefab.sprite[i], i);
    }
};

struct Cell : GameObject
{
    int maxHp = 150;
    int hp = maxHp;
    int speed = 5;
    int movementIndex;
    int rotationIndex;
    Equipment equipment[4];
};

struct Food : GameObject
{
    int type = 0; //dmg, speed, evo points or recipe
};

Prefab* prefabs[] = { &spike, &canon, &bristles, &tail };
Cell player;