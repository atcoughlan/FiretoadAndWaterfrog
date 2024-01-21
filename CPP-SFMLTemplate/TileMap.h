// https://www.sfml-dev.org/tutorials/2.4/graphics-vertex-array.php
// ^^ SFML documentation

#pragma once
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "TempPlatform.h"

using namespace std;

/* 
modifications i made :
    - sfml documentation class is in one file, i separated the class into .h and .cpp
    - added getters
    - added m_tilesize
    - added TileType enum
    - added array of temp platforms
    - modified draw to draw temp platforms if active
*/

// Each tile type present within the level map
enum TileType
{
    lava,
    water,
    mud,
    platform,
    empty,
    button
};

// Creates tilemap within each level
class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    
    TileMap(const sf::Vector2f*, const sf::Vector2f*);

    bool load(const string&, sf::Vector2u, const int*, unsigned int, unsigned int);

    void onButton(float x, float y, TileType oppositeLiquid);
    void resetButtons(TileType oppositeLiquid);
    void tick(float deltaTime);

    int getTileNumber(int x, int y)
    {
        return m_level[x + y * m_width];
    }
    int getTileNumber(int index)
    {
        return m_level[index];
    }
    int getTileNumberPx(float x, float y)
    {
        int index = getIndex(x, y);
        int tileNumber = m_level[index];
        
        if (tileNumber == TileType::empty)
        {
            for (TempPlatform tempPlatform : m_tempPlatforms)
            {
                if (tempPlatform.getIsActive() && tempPlatform.getSprite().getGlobalBounds().contains(x, y))
                {
                    tileNumber = TileType::platform;
                    break;
                }
            }
        }

        return tileNumber;
    }

private:
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;

    const int* m_level;
    unsigned int m_width;
    unsigned int m_height;

    sf::Vector2u m_tileSize;

    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

    static const int m_nbTempPlatforms = 5;
    TempPlatform m_tempPlatforms[m_nbTempPlatforms];

    int getIndex(float x, float y)
    {
        return static_cast<int>(x / m_tileSize.x) + static_cast<int>(y / m_tileSize.y) * m_width;
    }
};