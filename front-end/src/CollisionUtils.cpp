#include "CollisionUtils.h"

namespace CollisionUtils
{
bool intersects(const sf::Vector2f& point, const sf::FloatRect& rect)
{
	return rect.contains(point);
}

bool intersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2)
{
	return rect1.intersects(rect2);
}
}