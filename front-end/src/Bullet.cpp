#include "Bullet.h"
#include "Asteroid.h"
#include "CollisionUtils.h"
#include "GameManager.h"

Bullet::Bullet(sf::Vector2f position, sf::Vector2f direction) :
	Entity(position, 0.0f),
	shape(1.0f),
	direction(direction),
	lifetime(BULLET_LIFETIME)
{
}

void Bullet::update(float deltaTime)
{
	lifetime -= deltaTime;
	position += direction * BULLET_SPEED * deltaTime;

	if (lifetime <= 0.0f)
	{
		GameManager::removeEntity(this);
		return;
	}

	sf::FloatRect bulletBounds(position.x, position.y, 1.0f, 1.0f);

	for (auto entity : GameManager::getEntities())
	{
		if (auto asteroid = dynamic_cast<Asteroid*>(entity))
		{
			sf::FloatRect asteroidBounds = asteroid->getSprite().getGlobalBounds();

			if (bulletBounds.intersects(asteroidBounds))
			{
				GameManager::removeEntity(asteroid);
				GameManager::removeEntity(this);
				GameManager::increaseScore(10);
				return;
			}
		}
	}
}

void Bullet::render(sf::RenderWindow& window)
{
	window.draw(shape, sf::Transform().translate(position));
}