#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

// Class used to place exit doors for Firetoad and Waterfrog
class Exit
{
public:
	Exit(Element);

	void tick(float);

	// Getters
	sf::Sprite& getSprite()
	{
		return m_sprite;
	}

	// Setters
	void setPosition(sf::Vector2f position)
	{
		m_position = position;
	}

private:
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	float m_width, m_height;

	sf::Vector2f m_position;
};

