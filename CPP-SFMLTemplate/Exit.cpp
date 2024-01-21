#include "Exit.h"
#include <Windows.h>
#include <string>

using namespace std;

Exit::Exit(Element element) 
{
    // Select which exit texture to use depending whether it's for Firetoad or Waterfrog
    string fileName;
    switch (element)
    {
    case Element::fire: 
        fileName = "fireExit";
        break;
    case Element::water:
        fileName = "waterExit";
        break;
    }
    
    // Sprite initialisation
    if (!m_texture.loadFromFile("Assets/" + fileName + ".png"))
    {
        system("pause");
    }
    m_sprite = sf::Sprite(m_texture);

    m_width = m_sprite.getGlobalBounds().width;
    m_height = m_sprite.getGlobalBounds().height;

    m_position = sf::Vector2f(0.f, 0.f);
}

void Exit::tick(float deltaTime)
{
    m_sprite.setPosition(m_position);
}