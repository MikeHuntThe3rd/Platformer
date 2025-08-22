#pragma once
#include "SFML/Graphics.hpp"
#include "json.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <random>
class game
{
public:
	//sfml
	sf::RectangleShape player;
	sf::RectangleShape temp;
	sf::Vector2f speed = { 0.f, 0.f };
	sf::RenderWindow& window;
	game(sf::RenderWindow& win);

	// Bots
	struct Bot
	{
		sf::RectangleShape object;
		sf::Vector2f speed = {0.f, 0.f};
		int direction = 0; // positive right, negative left
		bool falling = false;
	};
	std::vector<Bot> Bots;

	//variables
	struct Col_Data
	{
		bool collided;
		sf::FloatRect with;
		sf::Vector2f side;
	};
	std::vector<sf::RectangleShape> Game_ObjS;
	std::vector<sf::RectangleShape> Checked_ObjS;
	float acceleration = 3.f;
	float deceleration = 3.f;
	bool falling = false;
	//functions
	void LoadMap(std::string level);
	void FuncDistrib();
	void Physics(sf::RectangleShape& object, sf::Vector2f& speed, bool& falling);
	void movement();
	void WallJump(int dir);
	void Dash();
	void DrawAll();
	void GetRelevantTiles(sf::RectangleShape& object, sf::Vector2f speed);
	int RandRange(int start, int end);
	Col_Data collision(sf::RectangleShape& object, float XShift = 0.f, float YShift = 0.f);
	
};