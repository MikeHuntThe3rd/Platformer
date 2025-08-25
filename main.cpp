#include "SFML/Graphics.hpp"
#include "game.h"
#include "titlescreen.h"

int main() {
	#pragma region classes
	sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Platformer", sf::Style::Default, sf::State::Fullscreen);
	titlescreen ti(window);
	game gm(window);
	#pragma endregion
	#pragma region timer
	std::chrono::steady_clock::time_point curr = std::chrono::steady_clock::now();
	std::chrono::milliseconds interval = std::chrono::milliseconds(17);
	#pragma endregion

	
	while (window.isOpen()) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
			//ti.title_ON = true;
			window.close();
		}
		if (std::chrono::steady_clock::now() >= curr + interval) {
			curr = std::chrono::steady_clock::now();
			if (ti.title_ON) {
				ti.title();
			}
			else {
				gm.FuncDistrib();
			}
		}
	}
	return 0;
}