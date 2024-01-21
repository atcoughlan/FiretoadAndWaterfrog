#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

using namespace std;

// Class for temporary platforms that appear in the game when their associated button is activated
class TempPlatform
{
public:

	TempPlatform();

	// Getters
	sf::Sprite& getSprite()
	{
		return m_sprite;
	}
	bool getIsActive()
	{
		return m_isFiretoadOn || m_isWaterfrogOn;
	}
	sf::Vector2f getButtonPosition()
	{
		return m_buttonPosition;
	}
	bool getIsFiretoadOn()
	{
		return m_isFiretoadOn;
	}
	bool getIsWaterfrogOn()
	{
		return m_isWaterfrogOn;
	}

	// Setters
	void setPosition(sf::Vector2f position)
	{
		m_position = position;
		m_sprite.setPosition(position);
	}
	void setButtonPosition(sf::Vector2f buttonPosition)
	{
		m_buttonPosition = buttonPosition;
	}
	void setIsActive(bool isActive)
	{
		m_isActive = isActive;
	}
	void setIsFiretoadOn(bool isFiretoadOn)
	{
		m_isFiretoadOn = isFiretoadOn;
	}
	void setIsWaterfrogOn(bool isWaterfrogOn)
	{
		m_isWaterfrogOn = isWaterfrogOn;
	}

private:
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	float m_width, m_height;

	sf::Vector2f m_position;

	bool m_isActive;

	sf::Vector2f m_buttonPosition;

	bool m_isFiretoadOn, m_isWaterfrogOn;
};

