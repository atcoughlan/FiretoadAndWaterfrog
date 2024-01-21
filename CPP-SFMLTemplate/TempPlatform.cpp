#include "TempPlatform.h"

TempPlatform::TempPlatform()
{
    if (!m_texture.loadFromFile("Assets/tempPlatform.png"))
    {
        system("pause");
    }
    m_sprite = sf::Sprite(m_texture);

    m_width = m_sprite.getGlobalBounds().width;
    m_height = m_sprite.getGlobalBounds().height;

    m_position = sf::Vector2f(0.f, 0.f);

    m_isActive = m_isFiretoadOn = m_isWaterfrogOn = false;

    m_buttonPosition = sf::Vector2f(0.f, 0.f);
}