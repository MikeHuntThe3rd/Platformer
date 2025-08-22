#include "game.h"
using json = nlohmann::ordered_json;
game::game(sf::RenderWindow& win)
	:window(win)
{
	if (!std::filesystem::exists("levels.json")) {
		json myData;
		myData["lvl1"]["objects"] = { { {"Size", {(float)window.getSize().x, 100.f}}, {"Location", {0.f, (float)window.getSize().y - 100.f}}},
									  { {"Size", {100.f, (float)window.getSize().y}}, { "Location", { 0.f, 0.f } } },
									  { {"Size", {100.0, 100.0}}, { "Location", { 960.0, 960.0 } } }
		};
		std::ofstream outFile("levels.json");
		outFile << myData.dump(4);
		outFile.close();
	}
	LoadMap("lvl1");
	player.setSize({100.f, 200.f});
	player.setPosition({500.f, 700.f});
	player.setFillColor(sf::Color::Blue);
}
void game::LoadMap(std::string level) {
	std::ifstream file("levels.json");
	json objs;
	file >> objs;
	file.close();
	for (json curr : objs[level]["objects"]) {
		sf::RectangleShape new_obj;
		new_obj.setPosition({ curr["Location"][0], curr["Location"][1] });
		new_obj.setSize({ curr["Size"][0], curr["Size"][1] });
		new_obj.setFillColor(sf::Color::Red);
		Game_ObjS.push_back(new_obj);
	}
}
void game::FuncDistrib() {
	GetRelevantTiles();
	Physics();
	movement();
	DrawAll();
	Checked_ObjS.clear();
}
void game::movement() {
	if (falling) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			speed.x += (speed.x > -40.f) ? -2.f : 0.f;
			WallJump(1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			speed.x += (speed.x < 40.f)? 2.f: 0.f;
			WallJump(-1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			speed.y += acceleration;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
			Dash();
		}
	}
	else {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !falling) {
			speed.y = -50.f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			speed.x += (speed.x > -20.f) ? -20.f : 0.f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			speed.x += (speed.x < 20.f) ? 20.f : 0.f;
		}
	}
	
}
void game::Dash() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		speed.y = -50.f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		speed.y = 50.f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		speed.x = -50.f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		speed.x = 50.f;
	}
}
void game::WallJump(int dir) {
    bool wall_col = (collision(speed.x).collided);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && wall_col && collision(speed.x, -(float)player.getSize().y / 8.f).collided) {
		speed = { 50.f * dir, -20.f };
	}
}
void game::Physics() {
	//Yaxis
	game::Col_Data Y_Col = game::Col_Data();
	if (falling) Y_Col = collision(0.f, speed.y);
	else Y_Col = collision(0.f, 1.f);
	sf::Vector2f pos = player.getPosition();

	if (Y_Col.collided && Y_Col.side.y == 1.f) {
		player.setPosition({ (float)player.getPosition().x, Y_Col.with.position.y - (float)player.getSize().y });
		float AbsDecel = (std::abs(speed.x) > deceleration) ? deceleration * (std::abs(speed.x) / speed.x) : speed.x;
		speed = { speed.x - AbsDecel, 0.f };
		falling = false;
	}
	else if (Y_Col.collided && Y_Col.side.y == -1.f) {
		player.setPosition({player.getPosition().x, Y_Col.with.position.y + Y_Col.with.size.y});
		speed.y = 0.f;
		falling = true;
	}
	else {
		speed.y += acceleration;
		falling = true;
	}
	//Xaxis
	game::Col_Data X_col = collision(speed.x);
	if (X_col.collided) {
		if (X_col.with.position.y >= player.getPosition().y + (player.getSize().y / 8.f * 7)) {
			speed.y = 0.f;
			player.setPosition({ player.getPosition().x, player.getPosition().y - (float)player.getSize().y / 8.f });
		}
		else
		{
			if (X_col.side.x == -1.f) {
				player.setPosition({ X_col.with.position.x + X_col.with.size.x, player.getPosition().y });
			}
			else {
				player.setPosition({ X_col.with.position.x - player.getSize().x, player.getPosition().y});
			}
			speed = { 0.f, 0.f };
		}
	}
	sf::Vector2f PPos = player.getPosition();
	player.setPosition({PPos.x + speed.x, PPos.y + speed.y});
}
game::Col_Data game::collision(float XShift, float YShift) {
	for (sf::RectangleShape Curr_Obj: Checked_ObjS) {
		sf::FloatRect bounds = Curr_Obj.getGlobalBounds();
		sf::FloatRect player_Hitbox = player.getGlobalBounds();
		player_Hitbox.position.x += XShift;
		player_Hitbox.position.y += YShift;
		std::optional<sf::FloatRect> intersection = player_Hitbox.findIntersection(bounds);
		if (intersection.has_value()) {
			temp.setPosition(intersection.value().position);
			temp.setSize(intersection.value().size);
			temp.setFillColor(sf::Color::Green);
			sf::Vector2f dir({0.f, 0.f});
			if (intersection.value().position.y > player.getPosition().y + player.getSize().y - intersection.value().size.y) dir.y = 1.f;
			if (intersection.value().position.y < player.getPosition().y + intersection.value().size.y) dir = { 0.f, -1.f };
			if (intersection.value().position.x > player.getPosition().x + player.getSize().x - intersection.value().size.x) dir.y = 1.f;;
			if (intersection.value().position.x < player.getPosition().x + intersection.value().size.x) dir.x = -1.f;;
			return { true, bounds, dir};
		}
	}
	return { false, sf::FloatRect(), {0.f, 0.f} };
}
void game::GetRelevantTiles() {
	sf::FloatRect player_bounds = player.getGlobalBounds();
	if (speed.x >= 0.f) {
		player_bounds.size.x += speed.x + 10.f;
	}
	else {
		player_bounds.position.x += speed.x - 10.f;
		player_bounds.size.x += std::abs(speed.x) + 10.f;
	}
	if (speed.y >= 0.f) {
		player_bounds.size.y += speed.y + 10.f;
	}
	else
	{
		player_bounds.position.y += speed.y - 10.f;
		player_bounds.size.y += std::abs(speed.y) + 10.f;
	}
	for (sf::RectangleShape curr : Game_ObjS) {
		if (player_bounds.findIntersection(curr.getGlobalBounds())) Checked_ObjS.push_back(curr);
	}
}
void game::DrawAll() {
	window.clear();
	for (sf::RectangleShape curr: Game_ObjS) {
		window.draw(curr);
	}
	window.draw(player);
	window.draw(temp);
	window.display();
}
