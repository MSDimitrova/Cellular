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
};

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
        tempRotation = parent.rotation;
        tempV2 = cellMargin[slot];

        if (slot % 2)
        {
            tempRotation *= -1;
            tempRotation -= round(atan2(tempV2.y, tempV2.x));
            tempV2.x = 0;
        }
        else
        {
            tempRotation -= round(atan2(tempV2.x, tempV2.y));
            tempV2.y = 0;
        }
        tempRotation /= toDegrees;

        tempPos = { parent.pos.x + tempV2.x + directionPos[slot * 2].x * size.x / 2,
            parent.pos.y + tempV2.y + directionPos[slot * 2].y * size.x / 2 };

        pos = { ((tempPos.x - parent.pos.x) * cos(tempRotation)) - ((parent.pos.y - tempPos.y) * sin(tempRotation)) + parent.pos.x,
            parent.pos.y - ((parent.pos.y - tempPos.y) * cos(tempRotation)) + ((tempPos.x - parent.pos.x) * sin(tempRotation)) };

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
    int speed = 0;
    int maxHp = 150;
    int hp = maxHp;
    int movementIndex = -1;
    int rotationIndex = -1;
    int knockbackFrames = 0;
    float knockbackAngle = 0;
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

    void SetKnockback(int _knockbackFrames, int _knockbackAngle)
    {
        knockbackFrames = _knockbackFrames;
        knockbackAngle = _knockbackAngle;
        speed = Pixels(4);
    }

    bool CheckKnockback()
    {
        if (knockbackFrames > 0)
        {
            pos = HypotenuseCoordinates(pos, speed, knockbackAngle);
            knockbackFrames--;
            if (knockbackFrames == 0)
                speed = Pixels(1);
            else if(ceil(Pixels(speed, 1)) != ceil(knockbackFrames / 4))
                speed--;
            return 1;
        }
        return 0;
    }
};

struct CanonBall : GameObject
{
    bool parent = 0; //0 - player, 1 - enemy
    int speed = 0;

    void MoveCanonBall()
    {
        pos = HypotenuseCoordinates(pos, speed, rotation);
    }
};

struct Food : GameObject
{
    int type = 0; //dmg, speed, evo points or recipe
};

Prefab canonBallPrefab;
std::vector<CanonBall> canonBalls;

Prefab spike, canon, bristles, tail;
Prefab* prefabPart[] = { &spike, &canon, &bristles, &tail, }; //0 - spike, 1 - canon, 2 - bristles, 3 - tail

Prefab prefabEnemy[prefabEnemies];
Cell enemy[enemies];
std::vector<Cell*> enemyOnScreen;

Cell player;