#pragma once
#include "SFML/Graphics.hpp"
#include "json.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
class game
{
public:
	//sfml
	sf::RectangleShape player;
	sf::RectangleShape bot1;
	sf::RectangleShape temp;
	sf::Vector2f speed = { 0.f, 0.f };
	sf::RenderWindow& window;
	game(sf::RenderWindow& win);
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
	void Physics();
	void movement();
	void WallJump(int dir);
	void Dash();
	void DrawAll();
	void GetRelevantTiles();
	Col_Data collision(float XShift = 0.f, float YShift = 0.f);
	
};