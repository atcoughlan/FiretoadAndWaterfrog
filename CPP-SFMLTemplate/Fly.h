#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

using namespace std;

// Class used to create collectible flies for both Firetoad and Waterfrog
class Fly
{
public:
	
	Fly(Element);

	void tick(float);
	
	// Getters
	sf::Sprite& getSprite()
	{
		return m_sprite;
	}
	bool getIsCollected()
	{
		return m_isCollected;
	}

	// Setters
	void setPosition(sf::Vector2f position)
	{
		m_position = position;
	}
	void setIsCollected(bool isCollected)
	{
		m_isCollected = isCollected;
	}

private:
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	float m_width, m_height;

	sf::Vector2f m_position;

	bool m_isCollected;
};

