#pragma once
#include "SFML/Graphics.hpp"
#include "json.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <chrono>

class game
{
public:
	//sfml
	sf::RectangleShape player;
	sf::RectangleShape temp;
	sf::Vector2f speed = { 0.f, 0.f };
	sf::RenderWindow& window;
	game(sf::RenderWindow& win);
	//structs
	struct Bot
	{
		sf::RectangleShape object;
		sf::Vector2f speed = { 0.f, 0.f };
		int direction = 0; // positive right, negative left
		bool falling = false;
	};
	struct Col_Data
	{
		bool collided;
		sf::FloatRect with;
		sf::Vector2f side;
	};
	struct CoolDown {
		std::string id;
		int interval;
		std::chrono::steady_clock::time_point LastTrigger;
	};
	// Bots
	std::vector<Bot> Bots;
	//variables
	std::vector<sf::RectangleShape> Game_ObjS;
	std::vector<sf::RectangleShape> Checked_ObjS;
	std::vector<CoolDown> CoolDowns = { { "dash", 3000, std::chrono::steady_clock::now() } };
	std::chrono::steady_clock::time_point Timer;
	float acceleration = 3.f;
	float deceleration = 3.f;
	bool falling = false;
	//functions
	int RandRange(int start, int end);
	bool IntervalPassed(std::string id);
	void LoadMap(std::string level);
	void MoveBot(Bot &currBot);
	void FuncDistrib();
	void Physics(sf::RectangleShape &object, sf::Vector2f &speed, bool &falling);
	void movement();
	void WallJump(int dir);
	void Dash();
	void DrawAll();
	void GetRelevantTiles(sf::RectangleShape &object, sf::Vector2f speed);
	Col_Data collision(sf::RectangleShape &object, float XShift = 0.f, float YShift = 0.f);

};