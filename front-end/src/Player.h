#pragma once

#include "Entity.h"

constexpr float PLAYER_WIDTH = 100.0f;
constexpr float PLAYER_HEIGHT = 70.0f;
constexpr float PLAYER_TURN_SPEED = 200.0f;
constexpr float PLAYER_MOVE_SPEED = 200.0f;
constexpr float PLAYER_SHOOT_DELAY = 0.5f;

class Player : public Entity
{
private:
	sf::Sprite sprite;
	sf::Texture texture;
	float shootTimer;

public:
	Player();

	void update(float deltaTime) override;
	void render(sf::RenderWindow& window) override;
};