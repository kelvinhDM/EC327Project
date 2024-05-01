#include "GameManager.h"
#include "Asteroid.h"
#include "Constants.h"
#include "Player.h"
#include <fstream>

std::vector<Entity*> GameManager::entities;
std::list<Entity*> GameManager::entitiesToAdd;
std::list<Entity*> GameManager::entitiesToRemove;

size_t GameManager::score;
size_t GameManager::highestScore;

sf::Texture GameManager::backgroundTexture;
sf::Sprite GameManager::backgroundSprite;

sf::Font GameManager::font;
sf::Font GameManager::tableFont;
sf::Text GameManager::scoreText;
sf::Text GameManager::gameOverText;
sf::Text GameManager::continueText;
sf::Text GameManager::highestScoreText;
sf::Text GameManager::titleText;
sf::Text GameManager::menuText;
sf::Text GameManager::playText;
sf::Text GameManager::namePromptText;
sf::Text GameManager::nameText;
sf::Text GameManager::rankingsText;
sf::Text GameManager::rankingPromptText;
sf::Text GameManager::funFactText;
sf::Text GameManager::funFactPromptText;
sf::Text GameManager::funFactContinueText;
sf::Text GameManager::funFactMenuText;
std::string GameManager::funFact;
std::string GameManager::username;
std::string GameManager::rankings;
std::string GameManager::funFactImageUrl;
sf::RenderWindow GameManager::funFactWindow;

sf::Texture GameManager::funFactTexture;
sf::Sprite GameManager::funFactSprite;

sf::SoundBuffer GameManager::shootSoundBuffer;
sf::Sound GameManager::shootSound;

float GameManager::asteroidSpawnTimer;

GameManager::GameState GameManager::gameState;

void GameManager::initialize()
{
	std::ifstream file("score.dat", std::ios::binary | std::ios::in);
	if (file.is_open())
	{
		file.read(reinterpret_cast<char*>(&highestScore), sizeof(size_t));
		file.close();
	}

	username = "Anonymous";
	std::ifstream usernameFile("username.dat");
	if (usernameFile.is_open())
	{
		std::getline(usernameFile, username);
		usernameFile.close();
	}

	if (!backgroundTexture.loadFromFile("content/universe.jpg"))
	{
		throw std::runtime_error("Failed to load universe.jpg");
	}
	float scaleX = static_cast<float>(SCREEN_WIDTH) / backgroundTexture.getSize().x;
	float scaleY = static_cast<float>(SCREEN_HEIGHT) / backgroundTexture.getSize().y;
	backgroundSprite.setScale(scaleX, scaleY);
	backgroundSprite.setTexture(backgroundTexture);

	font.loadFromFile("content/text-font.ttf");
	tableFont.loadFromFile("content/table-font.ttf");
	scoreText.setFont(font);
	scoreText.setPosition(sf::Vector2f(40, 20));
	scoreText.setCharacterSize(48);

	namePromptText.setFont(font);
	namePromptText.setPosition(sf::Vector2f(200, 220));
	namePromptText.setCharacterSize(24);
	namePromptText.setString("Enter your name:");

	nameText.setFont(font);
	nameText.setPosition(sf::Vector2f(200, 250));
	nameText.setCharacterSize(24);
	nameText.setString(username);

	rankingPromptText.setFont(font);
	rankingPromptText.setPosition(sf::Vector2f(260, 280));
	rankingPromptText.setCharacterSize(48);
	rankingPromptText.setString("---> Global Rankings <---");

	rankingsText.setFont(tableFont);
	rankingsText.setPosition(sf::Vector2f(100, 330));
	rankingsText.setCharacterSize(32);

	gameOverText.setFont(font);
	gameOverText.setPosition(sf::Vector2f(250, 10));
	gameOverText.setCharacterSize(128);
	gameOverText.setString("Game Over!");

	funFactPromptText.setFont(font);
	funFactPromptText.setPosition(sf::Vector2f(420, 150));
	funFactPromptText.setCharacterSize(36);
	funFactPromptText.setString("A Fun Fact!");

	funFactText.setFont(tableFont);
	funFactText.setPosition(sf::Vector2f(90, 180));
	funFactText.setCharacterSize(24);

	funFactContinueText.setFont(font);
	funFactContinueText.setCharacterSize(24);
	funFactContinueText.setString("Press ENTER to continue");

	funFactMenuText.setFont(font);
	funFactMenuText.setCharacterSize(24);
	funFactMenuText.setString("Press ESC to exit to menu");

	funFact = "";

	continueText.setFont(font);
	continueText.setPosition(sf::Vector2f(420, 810));
	continueText.setCharacterSize(24);
	continueText.setString("Press ENTER to continue");

	highestScoreText.setFont(font);
	highestScoreText.setPosition(sf::Vector2f(40, 20));
	highestScoreText.setCharacterSize(48);
	highestScoreText.setString("Your Highest Score: " + std::to_string(highestScore));

	menuText.setFont(font);
	menuText.setPosition(sf::Vector2f(420, 850));
	menuText.setCharacterSize(24);
	menuText.setString("Press ESC to exit to menu");

	titleText.setFont(font);
	titleText.setPosition(sf::Vector2f(270, 100));
	titleText.setCharacterSize(100);
	titleText.setString("ASTEROIDS");

	playText.setFont(font);
	playText.setPosition(sf::Vector2f(450, 780));
	playText.setCharacterSize(24);
	playText.setString("Press ENTER to play!");

	shootSoundBuffer.loadFromFile("content/shoot.wav");
	shootSound.setBuffer(shootSoundBuffer);

	getRankings();
	gameState = GameState::Menu;
}

void GameManager::saveUsername()
{
	std::ofstream usernameFile("username.dat");
	if (usernameFile.is_open())
	{
		usernameFile << username;
		usernameFile.close();
	}
}

void GameManager::startGame()
{
	gameState = GameState::Playing;
	entities.clear();
	entities.push_back(new Player());
	asteroidSpawnTimer = ASTEROID_SPAWN_INTERVAL;
	score = 0;
}

void GameManager::updateUsername(sf::Event event)
{
	if (event.type == sf::Event::TextEntered && gameState == GameState::Menu)
	{
		if (event.text.unicode == '\b' && !username.empty())
		{
			username.pop_back();
		}
		else if (event.text.unicode < 128 && username.size() < 20)
		{
			// Check if the character is an English alphabet (either uppercase or lowercase)
			if ((event.text.unicode >= 'a' && event.text.unicode <= 'z') || (event.text.unicode >= 'A' && event.text.unicode <= 'Z'))
			{
				username += static_cast<char>(event.text.unicode);
			}
		}
		nameText.setString(username);
	}
}

void GameManager::update(sf::RenderWindow& window, float deltaTime)
{
	window.draw(backgroundSprite);
	if (gameState == GameState::Menu)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			saveUsername();
			startGame();
		}

		window.draw(highestScoreText);
		window.draw(titleText);
		window.draw(playText);
		window.draw(namePromptText);
		window.draw(nameText);
		window.draw(rankingPromptText);
		window.draw(rankingsText);
	}
	else if (gameState == GameState::Playing)
	{
		entitiesToAdd.clear();
		entitiesToRemove.clear();
		asteroidSpawnTimer -= deltaTime;

		for (auto entity : entities)
		{
			entity->update(deltaTime);
			entity->render(window);
		}

		for (auto it = entities.begin(); it != entities.end();)
		{
			Entity* entity = *it;
			if (std::find(entitiesToRemove.begin(), entitiesToRemove.end(), entity) != entitiesToRemove.end())
			{
				it = entities.erase(it);
				delete entity;
			}
			else
			{
				entity->update(deltaTime);
				entity->render(window);
				it++;
			}
		}

		entities.insert(entities.end(), entitiesToAdd.begin(), entitiesToAdd.end());
		entitiesToAdd.clear();
		entitiesToRemove.clear();

		entities.insert(entities.end(), entitiesToAdd.begin(), entitiesToAdd.end());

		if (asteroidSpawnTimer <= 0.0f)
		{
			entities.push_back(new Asteroid());
			asteroidSpawnTimer = ASTEROID_SPAWN_INTERVAL;
		}

		scoreText.setString(std::to_string(score));
		window.draw(scoreText);
	}
	else if (gameState == GameState::GameOver)
	{
		entities.clear();
		window.draw(scoreText);
		window.draw(funFactPromptText);
		window.draw(funFactText);
		window.draw(gameOverText);
		window.draw(continueText);
		window.draw(menuText);
		// window.draw(funFactSprite);

		if (!funFactImageUrl.empty())
		{
			funFactWindow.clear();
			funFactContinueText.setPosition(sf::Vector2f(funFactWindow.getSize().x / 2.0f - funFactContinueText.getGlobalBounds().width / 2.0f,
				funFactWindow.getSize().y - funFactContinueText.getGlobalBounds().height - 20));
			funFactMenuText.setPosition(sf::Vector2f(funFactWindow.getSize().x / 2.0f - funFactMenuText.getGlobalBounds().width / 2.0f,
				funFactWindow.getSize().y - funFactMenuText.getGlobalBounds().height - 50));
			funFactWindow.draw(funFactSprite);
			funFactWindow.draw(funFactContinueText);
			funFactWindow.draw(funFactMenuText);
			funFactWindow.display();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			if (funFactWindow.isOpen())
				funFactWindow.close();
			startGame();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			if (funFactWindow.isOpen())
				funFactWindow.close();
			getRankings();
			gameState = GameState::Menu;
		}
	}
}

void GameManager::gameOver()
{
	if (score > highestScore)
	{
		highestScore = score;
		std::ofstream file("score.dat", std::ios::binary | std::ios::out);
		if (file.is_open())
		{
			file.write(reinterpret_cast<const char*>(&highestScore), sizeof(size_t));
			file.close();
		}
		else
		{
			printf("Failed to write high score to file!\n");
		}

		highestScoreText.setString("High Score: " + std::to_string(highestScore));
	}
	uploadScore();
	getFunFact();
	gameState = GameState::GameOver;
}

void GameManager::uploadScore()
{
	sf::Http http("http://127.0.0.1", 8000);
	std::string uri = "/rankings/?score=" + std::to_string(score) + "&username=" + username;
	sf::Http::Request request(uri, sf::Http::Request::Post);

	sf::Http::Response response = http.sendRequest(request);

	if (response.getStatus() == sf::Http::Response::Ok)
	{
		std::cout << "Score uploaded successfully!" << std::endl;
	}
	else
	{
		std::cout << "Failed to upload score. Status code: " << response.getStatus() << std::endl;
	}
}

void GameManager::getRankings()
{
	sf::Http http("http://127.0.0.1", 8000);
	sf::Http::Request request("/rankings/", sf::Http::Request::Get);
	sf::Http::Response response = http.sendRequest(request);

	if (response.getStatus() == sf::Http::Response::Ok)
	{
		rankings = response.getBody();
		std::cout << "Rankings:" << std::endl;
		std::cout << rankings << std::endl;
		rankingsText.setString(rankings);
	}
	else
	{
		std::cout << "Failed to get rankings. Status code: " << response.getStatus() << std::endl;
	}
}

void GameManager::getFunFact()
{
	sf::Http http("http://127.0.0.1", 8000);
	std::string uri = "/facts/get-one-by-username/?username=" + username;
	sf::Http::Request request(uri, sf::Http::Request::Get);
	sf::Http::Response response = http.sendRequest(request);

	if (response.getStatus() == sf::Http::Response::Ok)
	{
		std::string body = response.getBody();
		funFact = response.getBody();
		std::cout << "Fun fact:" << std::endl;
		std::cout << funFact << std::endl;

		std::string delimiter = "IMAGE_URL";
		size_t pos = body.find(delimiter);
		if (pos != std::string::npos)
		{
			funFact = body.substr(0, pos);
			funFactImageUrl = body.substr(pos + delimiter.length());
		}
		else
		{
			funFact = body;
			funFactImageUrl = "";
		}
		funFactText.setString(funFact);
		if (!funFactImageUrl.empty())
		{
			std::string uri = "/images/" + funFactImageUrl;
			sf::Http::Request imageRequest(uri, sf::Http::Request::Get);
			sf::Http::Response imageResponse = http.sendRequest(imageRequest);

			if (imageResponse.getStatus() == sf::Http::Response::Ok)
			{
				const auto& imageData = imageResponse.getBody();
				if (funFactTexture.loadFromMemory(imageData.data(), imageData.size()))
				{
					float maxDimension = 1000.0f;
					sf::Vector2u textureSize = funFactTexture.getSize();
					float scaleX = maxDimension / textureSize.x;
					float scaleY = maxDimension / textureSize.y;
					float scale = std::min(scaleX, scaleY);

					if (scale < 1.0f)
					{
						funFactSprite.setScale(scale, scale);
						textureSize.x *= scale;
						textureSize.y *= scale;
					}

					funFactSprite.setTexture(funFactTexture);
					funFactSprite.setPosition(sf::Vector2f(0, 0));
					funFactWindow.create(sf::VideoMode(textureSize.x, textureSize.y), "Fun Fact Image");
				}
				else
				{
					std::cout << "Failed to load fun fact image from memory." << std::endl;
				}
			}
		}
	}
	else
	{
		std::cout << "Failed to get a fun fact. Status code: " << response.getStatus() << std::endl;
	}
}

void GameManager::addEntity(Entity* entity)
{
	entitiesToAdd.push_back(entity);
}

void GameManager::removeEntity(Entity* entity)
{
	entitiesToRemove.push_back(entity);
}

const std::vector<Entity*>& GameManager::getEntities()
{
	return entities;
}

void GameManager::increaseScore(size_t points)
{
	score += points;
}

size_t GameManager::getScore()
{
	return score;
}

void GameManager::playSound(sf::Sound& sound)
{
	sound.play();
}

void GameManager::playShootSound()
{
	shootSound.play();
}