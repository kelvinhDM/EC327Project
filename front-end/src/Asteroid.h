#pragma once

#include "Entity.h"

constexpr float ASTEROID_WIDTH = 100.0f;
constexpr float ASTEROID_HEIGHT = 100.0f;
constexpr float ASTEROID_ROTATION_SPEED = 25.0f;
constexpr float ASTEROID_MOVEMENT_SPEED = 28.0f;
constexpr float ASTEROID_HIT_DURATION = 2.0f;
constexpr float ASTEROID_SPAWN_INTERVAL = 3.0f;

class Asteroid : public Entity
{
private:
	sf::Sprite sprite;
	sf::Texture texture;
	sf::Vector2f direction;
	float lifetime;

public:
	Asteroid(sf::Vector2f direction = getRandomDirection(),
		sf::Vector2f position = getRandomPosition());

	void update(float deltaTime) override;
	void render(sf::RenderWindow& window) override;
	const sf::VertexArray& getShape() const;

	static sf::Vector2f getRandomDirection();
	static sf::Vector2f getRandomPosition();

	float getLifetime() const
	{
		return lifetime;
	}

	const sf::Sprite& getSprite() const
	{
		return sprite;
	}
};