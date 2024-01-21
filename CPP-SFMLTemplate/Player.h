#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "TileMap.h"
#include "Constants.h"

using namespace std;

// Direction the player will take after a certain key is pressed
enum Direction
{
    left,
    right,
    up
};

// Parent class to Fireboy and Watergirl, in-game character
class Player
{
public:
    Player();

    // Updates player position every frame
    void tick(float deltaTime);

    // Modifies the player's velocity and whether it is in the air based on a direction given as a paramater
    void keyPressed(Direction);

    // Getters
    sf::Sprite& getSprite()
    {
        return m_sprite;
    }
    bool getIsInAir()
    {
        return m_isInAir;
    }
    sf::Vector2f getPosition()
    {
        return m_position;
    }
    float getWidth()
    {
        return m_width;
    }
    bool getIsMoving()
    {
        return m_isMoving;
    }
    bool getIsDead()
    {
        return m_isDead;
    }

    // Setters
    void setIsInAir(bool isInAir)
    {
        m_isInAir = isInAir;
    }
    void setUpKeyHit(bool upKeyHit)
    {
        m_upKeyHit = upKeyHit;
    }
    void setVelocity(sf::Vector2f velocity)
    {
        m_velocity = velocity;
    }
    void setVelocityX(float velocityX)
    {
        m_velocity.x = velocityX;
    }
    void setVelocityY(float velocityY)
    {
        m_velocity.y = velocityY;
    }
    void setTileMap(TileMap* tileMap)
    {
        m_tileMap = tileMap;
    }
    void setPosition(sf::Vector2f position)
    {
        m_position = position;
    }
    void setIsDead(bool isDead)
    {
        m_isDead = isDead;
    }

protected:
    // Initializes sprite's texture, width and height based on the filename given as parameter
    void initSprite(string);

    TileType m_oppositeLiquid;

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    sf::Vector2i m_screenSize = sf::Vector2i(1248, 928);

    float m_width, m_height;

    sf::Vector2f m_velocity, m_position, m_acceleration;

    bool m_isInAir;

    TileMap* m_tileMap;

    bool m_upKeyHit;

    void collisionDetection();

    bool m_isMoving;

    bool m_isDead;
};
