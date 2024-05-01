#include "GameManager.h"
#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>

int main()
{
	util::Platform platform;

#if defined(_DEBUG)
	std::cout << "DEBUG MODE!" << std::endl;
#endif

	sf::RenderWindow window(sf::VideoMode(1200, 900), "Asteroids", sf::Style::Close | sf::Style::Titlebar);
	sf::Clock clock;

	GameManager::initialize();
	window.setKeyRepeatEnabled(true);

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::TextEntered)
			{
				GameManager::updateUsername(event);
			}
		}

		window.clear();
		GameManager::update(window, deltaTime);
		window.display();
	}

	return 0;
}