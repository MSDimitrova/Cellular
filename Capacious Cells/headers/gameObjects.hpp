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

    void UpdateAnimation(Prefab& prefab)
    {
        //reduce size of this's sprite array to size of prefab sprite array to be assigned
        if (sprite.size() > prefab.sprite.size())
            for (int i = sprite.size(); i > prefab.sprite.size(); i--)
                sprite.pop_back();

        //assign prefab sprite array to this's sprite array
        for (int i = 0; i < prefab.sprite.size(); i++)
            UpdateSprite(&prefab.sprite[i], i);
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
        //Vector2 prepMargin = { sqrt(pow(cellMargin[slot].x, 2) + pow(cellMargin[slot].y, 2)) , sqrt(pow(cellMargin[slot].x, 2) + pow(cellMargin[slot].y, 2))};
        Vector2 prepMargin = cellMargin[slot];

        if (prepRotation < 0)
            prepRotation += 360;

        if (slot % 2)
        {
            prepRotation *= -1;
            prepRotation -= round(atan2(prepMargin.y, prepMargin.x)) * 3.6;
            prepMargin.x = 0;
        }
        else
        {
            prepRotation -= round(atan2(prepMargin.x, prepMargin.y));
            prepMargin.y = 0;
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

        UpdateAnimation(prefab);
    }
};

struct Cell : GameObject
{
    int id = -1;
    int maxHp = 150;
    int hp = maxHp;
    int speed = 0;
    int movementIndex;
    int rotationIndex;
    Equipment equipment[4];

    void Setup(Prefab& prefab, int _id = -1)
    {
        if (_id != -1)
            id = _id;
        pos.x = CENTER.x + enemyPos[id].x;
        pos.y = CENTER.y + enemyPos[id].y;

        //assign prefab values to this's values
        speed = prefab.data["speed"];
        id = prefab.data["id"];
        maxHp = prefab.data["maxHp"];
        hp = maxHp;

        UpdateAnimation(prefab);
    }
};

struct Food : GameObject
{
    int type = 0; //dmg, speed, evo points or recipe
};

Prefab* prefabPart[] = { &spike, &canon, &bristles, &tail }; //0 - spike, 1 - canon, 2 - bristles, 3 - tail

Prefab prefabEnemy[prefabEnemies];
Cell enemy[enemies];
std::vector<Cell*> enemyOnScreen;

Cell player;