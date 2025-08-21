#pragma once
#include "SFML/Graphics.hpp"
class titlescreen
{
public:
	//initalization
	titlescreen(sf::RenderWindow& win);
	sf::RenderWindow& window;
	sf::Texture texture;
	sf::Sprite img;
	//variables
	bool title_ON = true;
	//functions
	void title();
};

