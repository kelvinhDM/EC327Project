#pragma once

#include "Entity.h"

constexpr float BULLET_SPEED = 400.0f;
constexpr float BULLET_LIFETIME = 3.0f;

class Bullet : public Entity
{
private:
	sf::CircleShape shape;
	sf::Vector2f direction;
	float lifetime;

public:
	Bullet(sf::Vector2f position, sf::Vector2f direction);

	void update(float deltaTime) override;
	void render(sf::RenderWindow& window) override;
};