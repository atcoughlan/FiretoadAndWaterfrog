#include "TileMap.h"
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

TileMap::TileMap(const sf::Vector2f* tempPlatformPositions, const sf::Vector2f* buttonPositions)
{
    for (int i = 0; i < m_nbTempPlatforms; i++)
    {
        m_tempPlatforms[i].setPosition(tempPlatformPositions[i]);
        m_tempPlatforms[i].setButtonPosition(buttonPositions[i]);
    }
}

bool TileMap::load(const string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
{
    // load the tileset texture
    if (!m_tileset.loadFromFile(tileset))
        return false;

    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(width * height * 4);

    // populate the vertex array, with one quad per tile
    for (unsigned int i = 0; i < width; ++i)
    {
        for (unsigned int j = 0; j < height; ++j)
        {
            // get the current tile number
            int tileNumber = tiles[i + j * width];

            // find its position in the tileset texture
            int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
            int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

            // get a pointer to the current tile's quad
            sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

            // define its 4 corners
            quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
            quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
            quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

            // define its 4 texture coordinates
            quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
            quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
            quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
            quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
        }
    }

    // reference to m_level from game
    m_level = tiles;
    m_width = width;
    m_height = height;

    m_tileSize = tileSize;

    return true;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);

    // draw the temporary platforms
    for (TempPlatform tempPlatform : m_tempPlatforms)
    {
        if (tempPlatform.getIsActive())
        {
            target.draw(tempPlatform.getSprite());
        }
    }
}

void TileMap::onButton(float x, float y, TileType oppositeLiquid)
{
    int index = getIndex(x, y);

    for (int i = 0; i < m_nbTempPlatforms; i++)
    {
        int tempPlatformIndex = getIndex(m_tempPlatforms[i].getButtonPosition().x, m_tempPlatforms[i].getButtonPosition().y);

        if (index == tempPlatformIndex)
        {
            m_tempPlatforms[i].setIsActive(true);
            
            if (oppositeLiquid == TileType::lava)
            {
                m_tempPlatforms[i].setIsWaterfrogOn(true);
            }
            else
            {
                m_tempPlatforms[i].setIsFiretoadOn(true);
            }
        }
    }
}

void TileMap::resetButtons(TileType oppositeLiquid)
{
    for (int i = 0; i < m_nbTempPlatforms; i++)
    {
        if (oppositeLiquid == TileType::lava)
        {
            m_tempPlatforms[i].setIsWaterfrogOn(false);
        }
        else
        {
            m_tempPlatforms[i].setIsFiretoadOn(false);
        }
    }
}

void TileMap::tick(float deltaTime)
{
    
}