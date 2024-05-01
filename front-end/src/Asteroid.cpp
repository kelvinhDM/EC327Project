#include "Asteroid.h"
#include "Constants.h"
#include <random>

Asteroid::Asteroid(sf::Vector2f direction, sf::Vector2f position) :
	Entity(position, 0),
	direction(direction),
	lifetime()
{
	if (!texture.loadFromFile("content/asteroid.png"))
	{
		throw std::runtime_error("Failed to load asteroid.png");
	}
	sprite.setTexture(texture);
	sprite.setOrigin(ASTEROID_WIDTH / 2.0f, ASTEROID_HEIGHT / 2.0f);
}

void Asteroid::update(float deltaTime)
{
	lifetime += deltaTime;

	position += ASTEROID_MOVEMENT_SPEED * direction * deltaTime;
	rotation += ASTEROID_ROTATION_SPEED * deltaTime;

	if (position.x < ASTEROID_WIDTH / 2.0f)
	{
		direction.x = std::abs(direction.x);
	}
	else if (position.x > SCREEN_WIDTH - ASTEROID_WIDTH / 2.0f)
	{
		direction.x = -std::abs(direction.x);
	}

	if (position.y < ASTEROID_HEIGHT / 2.0f)
	{
		direction.y = std::abs(direction.y);
	}
	else if (position.y > SCREEN_HEIGHT - ASTEROID_HEIGHT / 2.0f)
	{
		direction.y = -std::abs(direction.y);
	}
}

void Asteroid::render(sf::RenderWindow& window)
{
	sprite.setPosition(position);
	sprite.setRotation(rotation);
	window.draw(sprite);
}

sf::Vector2f Asteroid::getRandomDirection()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dist(0.0f, 2.0f * PI);

	float angle = dist(gen);
	return sf::Vector2f(std::cos(angle), std::sin(angle));
}

sf::Vector2f Asteroid::getRandomPosition()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> xDist(ASTEROID_WIDTH / 2.0f,
		SCREEN_WIDTH - ASTEROID_WIDTH / 2.0f);
	static std::uniform_real_distribution<float> yDist(ASTEROID_HEIGHT / 2.0f,
		SCREEN_HEIGHT - ASTEROID_HEIGHT / 2.0f);

	return sf::Vector2f(xDist(gen), yDist(gen));
}