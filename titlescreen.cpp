#include "titlescreen.h"
titlescreen::titlescreen(sf::RenderWindow& win)
	:window(win),
	texture("forest_background.png"),
	img(texture)
{
}
void titlescreen::title() {
	img.setPosition({window.getPosition().x / 2.f, window.getPosition().y / 2.f});
	sf::Vector2u size = window.getSize();
	img.setScale({ size.x / (float)texture.getSize().x, size.y / (float)texture.getSize().y });

	sf::Font font("HackNerdFontMono-BoldItalic.ttf");
	sf::Text play(font);
	sf::Vector2u play_size = sf::Vector2u( (float)size.x / 10.f, (float)size.y / 15.f );
	play.setString("PLAY");
	sf::FloatRect bounds = play.getLocalBounds();
	play.setScale(sf::Vector2f(play_size.x / bounds.size.x, play_size.y / bounds.size.y));
	play.setPosition({ size.x / 2.f - play_size.x / 2.f, size.y / 3.f - play_size.y / 2.f });
	bounds = play.getLocalBounds();
	
	sf::Vector2i mouse = sf::Mouse::getPosition();
	bool touch = play.getGlobalBounds().contains(sf::Vector2f(mouse));
	if (touch) {
		play.setFillColor(sf::Color::Black);
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && touch) {
		title_ON = false;
	}
	window.clear();
	window.draw(img);
	window.draw(play);
	window.display();
}
