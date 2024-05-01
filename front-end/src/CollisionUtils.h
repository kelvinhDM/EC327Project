#pragma once

#include <SFML/Graphics.hpp>

namespace CollisionUtils
{
bool intersects(const sf::Vector2f& point, const sf::FloatRect& rect);
bool intersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
}