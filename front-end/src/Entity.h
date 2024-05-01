#pragma once

#include <SFML/Graphics.hpp>

class Entity
{
protected:
	sf::Vector2f position;
	float rotation;

public:
	Entity(sf::Vector2f position, float rotation) :
		position(position),
		rotation(rotation)
	{}

	virtual void update(float deltaTime) = 0;
	virtual void render(sf::RenderWindow& window) = 0;

	sf::Vector2f getPosition() const
	{
		return position;
	}
	float getRotation() const
	{
		return rotation;
	}

	virtual ~Entity() = default;
};