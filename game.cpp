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
		myData["lvl1"]["waves"] = { {{"Location", {500.0, 500.0}}}
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
int game::RandRange(int start, int end) {
	static std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<> distrib(start, end);
	return distrib(gen);
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
	for (json currWave : objs[level]["waves"]) {
		for (json currBot : currWave) {
			sf::RectangleShape new_bot;
			new_bot.setPosition({ currBot["Location"][0], currBot["Location"][1] });
			new_bot.setSize({ 100.f, 200.f});
			new_bot.setFillColor(sf::Color::Green);
			Bots.push_back({ new_bot});
		}
	}
}
void game::MoveBot(Bot &currBot) {
	if (currBot.direction == 0) currBot.direction = RandRange(0, 1) == 0 ? -1 : 1;
	bool botCollision = isNextBotMoveValid(currBot);
 	if (botCollision) currBot.direction *= -1;
	currBot.speed.x = 10.f * currBot.direction;
	Physics(currBot.object, currBot.speed, currBot.falling);
}
bool game::isNextBotMoveValid(game::Bot& currBot) {
	sf::FloatRect nextBotLocationRect = currBot.object.getGlobalBounds(); // get the bots current position and size
	sf::RectangleShape botFloorObject;

	nextBotLocationRect.position.x += currBot.speed.x;
	nextBotLocationRect.position.y += currBot.speed.y;

	if (currBot.speed.x == 0) {
		return true;
	}

	for (sf::RectangleShape object : Checked_ObjS) {
		if (object.getPosition().y == currBot.object.getGlobalBounds().position.y + currBot.object.getSize().y) {
			botFloorObject = object;
		}
	}

	for (sf::RectangleShape currObject : Checked_ObjS) {
		// stairs
		if (abs(currObject.getPosition().y - (nextBotLocationRect.position.y + nextBotLocationRect.size.y)) <= nextBotLocationRect.size.y / 8.f) {
			if ((currBot.direction == 1 && currObject.getPosition().x + currObject.getSize().x > botFloorObject.getPosition().x + botFloorObject.getSize().x) || (currBot.direction == -1 && currObject.getPosition().x < botFloorObject.getPosition().x)) {
				if (currObject.getPosition().x <= nextBotLocationRect.position.x + nextBotLocationRect.size.x && currObject.getPosition().x + currObject.getSize().x >= nextBotLocationRect.position.x + nextBotLocationRect.size.x) {
					return false;
				}
				else if (currObject.getPosition().x <= nextBotLocationRect.position.x && currObject.getPosition().x + currObject.getSize().x >= nextBotLocationRect.position.x) {
					return false;
				}
			}
		}
	}
	// if there are no stairs, but the bot can move to the current direction
	if ((currBot.direction == 1 && botFloorObject.getPosition().x + botFloorObject.getSize().x > nextBotLocationRect.position.x + nextBotLocationRect.size.x) || (currBot.direction == -1 && botFloorObject.getPosition().x < nextBotLocationRect.position.x)) {
		return false;
	}
	return true;
}
void game::FuncDistrib() {
	// bots
	for (Bot &currBot : Bots) {
		GetRelevantTiles(currBot.object, currBot.speed); // get all of the objects near the current bots
		if (!currBot.falling) {
			MoveBot(currBot);
		}
		else
		{
			Physics(currBot.object, currBot.speed, currBot.falling);
		}
	}

	// player
	GetRelevantTiles(player, speed);
	Physics(player, speed, falling);
	movement();
	DrawAll();
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
			player.setPosition({player.getPosition().x, player.getPosition().y + speed.y});
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
    bool wall_col = (collision(player, speed.x).collided);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && wall_col && collision(player, speed.x, -(float)player.getSize().y / 8.f).collided) {
		speed = { 50.f * dir, -20.f };
	}
}
void game::Physics(sf::RectangleShape &object, sf::Vector2f &speed, bool &falling) {
	//Yaxis
	game::Col_Data Y_Col = game::Col_Data();
	if (falling) Y_Col = collision(object, 0.f, speed.y);
	else Y_Col = collision(object, 0.f, 1.f);
	sf::Vector2f pos = object.getPosition();

	if (Y_Col.collided && Y_Col.side.y == 1.f) {
		object.setPosition({ (float)object.getPosition().x, Y_Col.with.position.y - (float)object.getSize().y });
		float AbsDecel = (std::abs(speed.x) > deceleration) ? deceleration * (std::abs(speed.x) / speed.x) : speed.x;
		speed = { speed.x - AbsDecel, 0.f };
		falling = false;
	}
	else if (Y_Col.collided && Y_Col.side.y == -1.f) {
		object.setPosition({ object.getPosition().x, Y_Col.with.position.y + Y_Col.with.size.y });
		speed.y = 0.f;
		falling = true;
	}
	else {
		speed.y += acceleration;
		falling = true;
	}
	//Xaxis
	game::Col_Data X_col = collision(object, speed.x);
	if (X_col.collided) {
		if (X_col.with.position.y >= object.getPosition().y + (object.getSize().y / 8.f * 7)) {
			speed.y = 0.f;
			object.setPosition({ object.getPosition().x, object.getPosition().y - (float)object.getSize().y / 8.f });
		}
		else
		{
			if (X_col.side.x == -1.f) {
				object.setPosition({ X_col.with.position.x + X_col.with.size.x, object.getPosition().y });
			}
			else {
				object.setPosition({ X_col.with.position.x - object.getSize().x, object.getPosition().y});
			}
			speed = { 0.f, 0.f };
		}
	}
	sf::Vector2f PPos = object.getPosition();
	object.setPosition({PPos.x + speed.x, PPos.y + speed.y});
}
game::Col_Data game::collision(sf::RectangleShape &object, float XShift, float YShift) {
	for (sf::RectangleShape Curr_Obj: Checked_ObjS) {
		sf::FloatRect bounds = Curr_Obj.getGlobalBounds();
		sf::FloatRect object_Hitbox = object.getGlobalBounds();
		object_Hitbox.position.x += XShift;
		object_Hitbox.position.y += YShift;
		std::optional<sf::FloatRect> intersection = object_Hitbox.findIntersection(bounds);
		if (intersection.has_value()) {
			temp.setPosition(intersection.value().position);
			temp.setSize(intersection.value().size);
			temp.setFillColor(sf::Color::Green);
			sf::Vector2f dir({0.f, 0.f});
			if (intersection.value().position.y > object.getPosition().y + object.getSize().y - intersection.value().size.y) dir.y = 1.f;
			if (intersection.value().position.y < object.getPosition().y + intersection.value().size.y) dir.y = -1.f ;
			if (intersection.value().position.x > object.getPosition().x + object.getSize().x - intersection.value().size.x) dir.y = 1.f;;
			if (intersection.value().position.x < object.getPosition().x + intersection.value().size.x) dir.x = -1.f;;
			return { true, bounds, dir};
		}
	}
	return { false, sf::FloatRect(), {0.f, 0.f} };
}
void game::GetRelevantTiles(sf::RectangleShape &object, sf::Vector2f speed) {
	if (Checked_ObjS.size() != 0) {
		Checked_ObjS.clear();
	}
	sf::FloatRect bounds = object.getGlobalBounds();
	float x = -bounds.size.x + std::abs(speed.x) * -1;
	float y = -bounds.size.y + std::abs(speed.y) * -1;
	float width = bounds.size.x * 3.f + std::abs(speed.x);
	float height = bounds.size.y * 3.f + std::abs(speed.y);
	bounds.position = {bounds.position.x + x, bounds.position.y + y};
	bounds.size = {width, height};
	for (sf::RectangleShape curr : Game_ObjS) {
		if (bounds.findIntersection(curr.getGlobalBounds())) Checked_ObjS.push_back(curr);
	}
}
void game::DrawAll() {
	window.clear();
	for (sf::RectangleShape curr: Game_ObjS) {
		window.draw(curr);
	}
	for (Bot currBot : Bots) {
		window.draw(currBot.object);
	}
	window.draw(player);
	window.draw(temp);
	window.display();
}
