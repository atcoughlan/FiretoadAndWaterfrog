#include "Player.h"
#include <SFML/Graphics.hpp>

Player::Player()
{
    m_velocity = sf::Vector2f(0.f, 0.f);
    m_acceleration = sf::Vector2f(0.f, 1000.f); // y = gravity
    
    m_isInAir = false;
    m_upKeyHit = false;
    m_isMoving = false;
    m_isDead = false;

    m_width = 0.f;
    m_height = 0.f;
    m_position = sf::Vector2f(0.f, 0.f);
}

void Player::initSprite(string fileName)
{
    if (!m_texture.loadFromFile("Assets/" + fileName + ".png"))
    {
        system("pause");
    }
    m_sprite = sf::Sprite(m_texture);

    m_width = m_sprite.getGlobalBounds().width;
    m_height = m_sprite.getGlobalBounds().height;
}

void Player::keyPressed(Direction direction)
{
    switch (direction)
    {
    case Direction::left:
        m_velocity.x = -250.f;
        break;

    case Direction::right:
        m_velocity.x = 250.f;
        break;

    case Direction::up:
        if (!m_isInAir)
        {
            m_velocity.y = -500.f;
            m_isInAir = true;
            m_upKeyHit = true;
        }
        break;
    }
}

void Player::tick(float deltaTime)
{
    // Initialization
    m_isMoving = false;

    // Player gets subjected to gravity
    if (m_isInAir)
    {
        m_velocity += m_acceleration * deltaTime;
    }

    // Updates player's position
    m_position += m_velocity * deltaTime;
    m_velocity.x = 0.f;

    collisionDetection();

    m_sprite.setPosition(m_position);
}

void Player::collisionDetection()
{
    // Resets buttons to inactive every frame in case the player moves off of it
    m_tileMap->resetButtons(m_oppositeLiquid);
    
    // Start of collision detection, calculates the coordinates of every corner of the character in its next position
    sf::Vector2f upperLeftCorner = m_position;
    sf::Vector2f corners[4] =
    {
        upperLeftCorner,                                                                 // 0 - upper left
        sf::Vector2f(upperLeftCorner.x + m_width - 1, upperLeftCorner.y),                // 1 - upper right
        sf::Vector2f(upperLeftCorner.x + m_width - 1, upperLeftCorner.y + m_height - 1), // 2 - lower right 
        sf::Vector2f(upperLeftCorner.x, upperLeftCorner.y + m_height - 1)                // 3 - lower left
    };

    // Direction player is going based on previous position and new position
    bool right = false, left = false, up = false, down = false;
    sf::Vector2f prevPos(m_sprite.getPosition());

    if (prevPos.y < m_position.y)
    {
        down = true;
        m_isMoving = true;
    }
    else if (prevPos.y > m_position.y)
    {
        up = true;
        m_isMoving = true;
    }
    if (prevPos.x < m_position.x)
    {
        right = true;
        m_isMoving = true;
    }
    else if (prevPos.x > m_position.x)
    {
        left = true;
        m_isMoving = true;
    }

    // Player is moving to the right, checks corners 1,2 and midpoint between them
    if (right)
    {
        sf::Vector2f middleRight(upperLeftCorner.x + m_width - 1, upperLeftCorner.y + m_height / 2 - 1);
        
        // Get the tile numbers at each three points
        int upperRightTileNumber = m_tileMap->getTileNumberPx(corners[1].x, corners[1].y);
        int middleRightTileNumber = m_tileMap->getTileNumberPx(middleRight.x, middleRight.y);
        int lowerRightTileNumber = m_tileMap->getTileNumberPx(corners[2].x, corners[2].y);

        int tileNumbers[3] = { upperRightTileNumber, middleRightTileNumber, lowerRightTileNumber };

        sf::Vector2f coords[3] = { corners[1], middleRight, corners[2] };

        // Loop through tile numbers of the three points and check if any are not empty
        for (int i = 0; i < 3; i++)
        {
            if (tileNumbers[i] != TileType::empty)
            {
                // Point is colliding with a tile, so snap character to the edge of the tile
                int x = static_cast<int>(coords[i].x / 32) * 32;
                
                // max ensures that the character isn't snapped back behind its previous position
                m_position.x = max(x - m_width, prevPos.x);
                
                break;
            }
        }
    }

    // Player is moving to the left, checks corners 0,3 and midpoint between them
    if (left)
    {
        sf::Vector2f middleLeft(upperLeftCorner.x, upperLeftCorner.y + m_height / 2 - 1);

        // Get the tile numbers at each three points
        int upperLeftTileNumber = m_tileMap->getTileNumberPx(corners[0].x, corners[0].y);
        int middleLeftTileNumber = m_tileMap->getTileNumberPx(middleLeft.x, middleLeft.y);
        int lowerLeftTileNumber = m_tileMap->getTileNumberPx(corners[3].x, corners[3].y);

        int tileNumbers[3] = { upperLeftTileNumber, middleLeftTileNumber, lowerLeftTileNumber };

        sf::Vector2f coords[3] = { corners[0], middleLeft, corners[3] };
        
        // Loop through tile numbers of the three points and check if any are not empty
        for (int i = 0; i < 3; i++)
        {
            if (tileNumbers[i] != TileType::empty)
            {
                // Point is colliding with a tile, so snap character to the edge of the tile
                int x = static_cast<int>(coords[i].x / 32 + 1) * 32;
                
                // min ensures that the character isn't snapped back behind its previous position
                m_position.x = min(static_cast<float>(x), prevPos.x);

                break;
            }
        }
    }

    // Recalculate the corner coordinates after the character has been snapped back in the x-axis
    upperLeftCorner = m_position;
    corners[0] = upperLeftCorner;                                                                 // 0 - upper left
    corners[1] = sf::Vector2f(upperLeftCorner.x + m_width - 1, upperLeftCorner.y);                // 1 - upper right
    corners[2] = sf::Vector2f(upperLeftCorner.x + m_width - 1, upperLeftCorner.y + m_height - 1); // 2 - lower right 
    corners[3] = sf::Vector2f(upperLeftCorner.x, upperLeftCorner.y + m_height - 1);               // 3 - lower left

    // Player is moving up, checks corners 0,1
    if (up)
    {
        // Get the tile numbers at each point
        int upperLeftTileNumber = m_tileMap->getTileNumberPx(corners[0].x, corners[0].y);
        int upperRightTileNumber = m_tileMap->getTileNumberPx(corners[1].x, corners[1].y);

        int tileNumbers[2] = { upperLeftTileNumber, upperRightTileNumber };

        sf::Vector2f coords[2] = { corners[0], corners[1] };
        
        // Loop through tile numbers of the two points and check if any are not empty
        for (int i = 0; i < 2; i++)
        {
            if (tileNumbers[i] != TileType::empty)
            {                
                // Point is colliding with a tile, so snap character to the edge of the tile
                int y = static_cast<int>(coords[i].y / 32 + 1) * 32;
                
                // Stop the character's upward movement
                m_position.y = y + 0.f;
                m_velocity.y = 0.f;
                
                break;
            }
        }
    }

    // Only check the pixels under the player if they are on the ground or the up key has not just been pressed
    // Checks corners 2,3
    if (!m_upKeyHit || !m_isInAir)
    {

        float rightCheckY = corners[2].y + 1;
        float leftCheckY = corners[3].y + 1;

        float checks[2] = { rightCheckY, leftCheckY };

        bool isInEmpty = true;

        for (int i = 0; i < 2; i++)
        {
            // Get the tile number at the point
            int tileUnderNumber = m_tileMap->getTileNumberPx(corners[i + 2].x, checks[i]);

            // Check to see if either of the two corner points is above a dangerous substance or a button
            if (tileUnderNumber == TileType::mud || tileUnderNumber == m_oppositeLiquid)
            {
                m_isDead = true;
            }
            else if (tileUnderNumber == TileType::button)
            {
                // Tell the TileMap to activate all temp platforms connected to this button
                m_tileMap->onButton(corners[i + 2].x, checks[i], m_oppositeLiquid);
            }
            
            if (tileUnderNumber != TileType::empty)
            {
                // Point is colliding with a tile, so snap character to the edge of the tile
                m_position.y = static_cast<int>(checks[i] / 32) * 32 - m_height;
                m_isInAir = false;
                m_velocity.y = 0.f;
                isInEmpty = false;
            }
        }
        if (isInEmpty)
        {
            m_isInAir = true;
        }
    }
    else
    {
        m_upKeyHit = false;
    }
}