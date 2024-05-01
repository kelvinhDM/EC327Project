#include "Player.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "CollisionUtils.h"
#include "Constants.h"
#include "GameManager.h"

Player::Player() :
	Entity(sf::Vector2f(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f), 0),
	shootTimer()
{
	if (!texture.loadFromFile("content/player.png"))
	{
		throw std::runtime_error("Failed to load player.png");
	}
	sprite.setTexture(texture);
	sprite.setOrigin(PLAYER_WIDTH / 2.0f, PLAYER_HEIGHT / 2.0f);
}

void Player::update(float deltaTime)
{
	shootTimer -= deltaTime;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		rotation -= PLAYER_TURN_SPEED * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		rotation += PLAYER_TURN_SPEED * deltaTime;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		float radians = rotation * (PI / 180.0f);

		position.x += std::cos(radians) * PLAYER_MOVE_SPEED * deltaTime;
		position.y += std::sin(radians) * PLAYER_MOVE_SPEED * deltaTime;

		position.x = std::min(std::max(position.x, PLAYER_WIDTH / 2.0f),
			SCREEN_WIDTH - PLAYER_WIDTH / 2.0f);
		position.y = std::min(std::max(position.y, PLAYER_HEIGHT / 2.0f),
			SCREEN_HEIGHT - PLAYER_HEIGHT / 2.0f);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && shootTimer <= 0.0f)
	{
		GameManager::playShootSound();
		shootTimer = PLAYER_SHOOT_DELAY;

		float radians = rotation * (PI / 180.0f);

		GameManager::addEntity(new Bullet(position, sf::Vector2f(std::cos(radians), std::sin(radians))));
	}

	sf::Transform playerTransform;
	playerTransform.translate(position);
	playerTransform.rotate(rotation);

	for (auto entity : GameManager::getEntities())
	{
		if (auto asteroid = dynamic_cast<Asteroid*>(entity))
		{
			if (asteroid->getLifetime() < ASTEROID_HIT_DURATION)
			{
				continue;
			}

			sf::FloatRect playerBounds = sprite.getGlobalBounds();
			sf::FloatRect asteroidBounds = asteroid->getSprite().getGlobalBounds();

			if (CollisionUtils::intersects(playerBounds, asteroidBounds))
			{
				GameManager::gameOver();
			}
		}
	}
}

void Player::render(sf::RenderWindow& window)
{
	sprite.setPosition(position);
	sprite.setRotation(rotation);
	window.draw(sprite);
}
