#pragma once

#include "Entity.h"
#include <SFML/Audio.hpp>
#include <list>
#include <vector>

class GameManager
{
private:
	static std::vector<Entity*> entities;
	static std::list<Entity*> entitiesToAdd;
	static std::list<Entity*> entitiesToRemove;

	static size_t score;
	static size_t highestScore;

	static sf::Texture backgroundTexture;
	static sf::Sprite backgroundSprite;

	static sf::Font font;
	static sf::Font tableFont;
	static sf::Text scoreText;
	static sf::Text gameOverText;
	static sf::Text continueText;
	static sf::Text highestScoreText;
	static sf::Text titleText;
	static sf::Text menuText;
	static sf::Text playText;

	static sf::Text namePromptText;
	static sf::Text nameText;
	static sf::Text rankingPromptText;

	static sf::Text rankingsText;
	static sf::Text funFactText;
	static sf::Text funFactPromptText;
	static sf::Text funFactContinueText;
	static sf::Text funFactMenuText;
	static std::string funFact;
	static std::string funFactImageUrl;
	static sf::RenderWindow funFactWindow;
	static std::string username;
	static std::string rankings;

	static sf::Texture funFactTexture;
	static sf::Sprite funFactSprite;

	static sf::SoundBuffer shootSoundBuffer;
	static sf::Sound shootSound;

	static float asteroidSpawnTimer;

	enum class GameState
	{
		Menu,
		Playing,
		GameOver
	};
	static GameState gameState;

public:
	static void initialize();
	static void saveUsername();
	static void startGame();
	static void update(sf::RenderWindow& window, float deltaTime);
	static void updateUsername(sf::Event event);
	static void gameOver();
	static void uploadScore();
	static void getRankings();
	static void getFunFact();

	static void addEntity(Entity* entity);
	static void removeEntity(Entity* entity);

	static const std::vector<Entity*>& getEntities();

	static void increaseScore(size_t points);
	static size_t getScore();

	static void playSound(sf::Sound& sound);
	static void playShootSound();
};