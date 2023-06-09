#pragma once
#include "libraries.hpp"

struct Prefab
{
    std::string name = "none";
    std::vector<Texture2D> sprite;
    json data;

    void Setup(const char* _name)
    {
        if (fs::exists(Path(assetsFolder, _name, pngExtention)))
            sprite.push_back(LoadTexture(Path(assetsFolder, _name, pngExtention).string().c_str()));
        else
            for (int i = 0; i < 63; i++)
                if (fs::exists(Path(assetsFolder, _name, pngExtention, i)))
                    sprite.push_back(LoadTexture(Path(assetsFolder, _name, pngExtention, i).string().c_str()));
                else
                    break;
        if (fs::exists(Path(prefabsFolder, _name, jsonExtention)))
        {
            std::ifstream f(Path(prefabsFolder, _name, jsonExtention));
            data = json::parse(f);
            name = data["name"];
        }
    }
} spike, cannon, bristles, tail, toxin;
Prefab* prefabPart[parts] = { &spike, &cannon, &bristles, &tail, &toxin }; //0 - spike, 1 - cannon, 2 - bristles, 3 - tail, 4 - toxin

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
    bool boostType = 0; //attack or speed
    int lvl = 1;
    int slot = 0;
    int cost = 0;
    int boost = 0;
    std::string name = "none"; //determines kind of equipment

    void UpdatePos(GameObject& parent)
    {
        tempRotation = parent.rotation;
        tempV2 = cellMargin[slot];

        if (slot % 2)
        {
            if (name == "toxin")
                tempV2.y -= directionPos[slot * 2].y * Pixels(5);
            tempRotation *= -1;
            tempRotation -= round(atan2(tempV2.y, tempV2.x));
            tempV2.x = 0;
        }
        else
        {
            if (name == "toxin")
                tempV2.x -= directionPos[slot * 2].x * Pixels(5);
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
        //assign the prefab's values to this equipment's values
        if (prefab.data.contains("animationFPS"))
            animationFPS = prefab.data["animationFPS"];
        if (prefab.data.contains("boostType"))
            boostType = 1;
        boost = prefab.data["boost"];
        name = prefab.data["name"];

        UpdateAnimation(prefab);
    }
} defaultEquipment;

struct Cell : GameObject
{
    bool activeTail = 0;
    int id = -1;
    int speed = Pixels(1);
    int invincibilityFrames = 0;
    int activeTailFrames = 0, cooldownTailFrames = 0;
    int movementIndex = -1, rotationIndex = -1;
    std::string name = "";

    int knockbackFrames = 0;
    float knockbackAngle = 0;

    int maxHp = initialPlayerHp, hp = maxHp, damage = 0;
    std::string hpText = "";

    Equipment equipment[slots];

    void Equip(int _part, int _slot)
    {
        if (equipment[_slot].name == "tail" && id == -1)
            playerHadTail = 1;
        equipment[_slot] = defaultEquipment; //clean equipment
        equipment[_slot].Setup(*prefabPart[_part]);
        equipment[_slot].slot = _slot;
    }

    void Setup(Prefab& prefab, int _id = -1)
    {
        if (_id != -1)
            id = _id;

        //assign prefab values to enemy's values
        name = prefab.data["name"];
        //speed = ceil(Pixels(prefab.data["speed"]) / 2);
        speed = Pixels(prefab.data["speed"]);
        maxHp = prefab.data["maxHp"];
        hp = maxHp;

        //equip enemy with prefab equipment
        for (int i = 0; i < slots; i++)
            if (prefab.data["equipment"][i] != "none")
                for (int j = 0; j < parts; j++)
                    if (prefab.data["equipment"][i] == prefabPart[j]->name)
                        Equip(j, i);
    }
    
    void ApplyInvincibility()
    {
        if (invincibilityFrames > 0 && !pause)
        {
            damage = 0;
            invincibilityFrames--;
            if (invincibilityFrames == 0)
                currentSprite = sprite[0];
            else if (currentSprite != sprite.back())
                currentSprite = sprite.back();
        }
    }

    void SetKnockback(int _knockbackFrames, int _knockbackAngle)
    {
        knockbackFrames = _knockbackFrames;
        knockbackAngle = _knockbackAngle;
        speed = Pixels(3);
    }
    bool ApplyKnockback()
    {
        if (knockbackFrames > 0 && !pause)
        {
            pos = HypotenuseCoordinates(pos, speed, knockbackAngle);
            knockbackFrames--;
            if (knockbackFrames == 0)
                speed = Pixels(1);
            else if(ceil(Pixels(speed, 1)) != ceil(knockbackFrames / 6))
                speed -= floor(Pixels(.34));
            return 1;
        }
        return 0;
    }
    
    void ApplyDamage()
    {
        if (damage > 0 && !pause)
        {
            if (id == -1 && hunger)
            {
                damage *= 2;
                hunger = 0;
            }
                
            hp -= damage;
            invincibilityFrames = 19;
            damage = 0;
        }
    }

    void DrawHp(bool update = 1)
    {
        hpText = "";
        if (update)
        {
            hpText = std::to_string(hp) + "/" + std::to_string(maxHp) + " HP";
            if (hp > 150)
                tempColor = BLUE;
            else if (hp > 100)
                tempColor = GREEN;
            else if (hp > 50)
                tempColor = ORANGE;
            else if (hp > 0)
                tempColor = RED;
        }
        else
        {
            hpText = "0/" + std::to_string(maxHp) + " HP";
            tempColor = RED;
        }
        DrawText(hpText.c_str(), pos.x - Pixels(hpText.size()), pos.y - Pixels(20), Pixels(5), tempColor);
    }

    void MapRotation()
    {
        if (rotation < 0)
            rotation += 360;
        rotationIndex = floor(rotation / 45);
        if (rotationIndex < 0)
            rotationIndex += 8;
    }
};

struct CannonBall : GameObject
{
    bool parent = 0; //0 - player, 1 - enemy
    int speed = 0;
    int attack = 0;

    void MoveCannonBall()
    {
        if(!pause)
            pos = HypotenuseCoordinates(pos, speed, rotation);
    }
};

struct Food : GameObject
{
    bool eaten = 0;
    int type = 0; //attack, speed, evo points or recipe

    void TryToGetEaten()
    {
        if (pos.x > tempPos.x && pos.y > tempPos.y && pos.x < tempV2.x && pos.y < tempV2.y)
        {
            heal += 30;
            eaten = 1;
        }
    }
};

std::vector<CannonBall> cannonBalls;
std::vector<Food> food(foods);
std::vector<Food*> foodOnScreen;

Prefab prefabEnemy[prefabEnemies];
std::vector<Cell> enemy(enemies);
std::vector<Cell*> enemyOnScreen;

Cell player;