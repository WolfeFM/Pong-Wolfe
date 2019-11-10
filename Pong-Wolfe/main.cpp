#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>
#include <string>

const float pi = 3.14159f;
sf::SoundBuffer boingBuffer;
sf::Sound boingSound;
sf::SoundBuffer cheersBuffer;
sf::Sound cheersSound;
sf::SoundBuffer ahhhBuffer;
sf::Sound ahhhSound;
sf::SoundBuffer whoaBuffer;
sf::Sound whoaSound;
sf::Font font;
sf::Texture paddleTexture;

class paddle
{
	public: 
		sf::RectangleShape rectangle;
		float speed = 250.f;

		paddle(float x, float y) {
			rectangle.setPosition(x, y);
			rectangle.setSize(sf::Vector2f(10, 50));
		}

		void move(bool up, sf::Time dt) {
			if (up)
			{
				//do moving up
				if (rectangle.getPosition().y - (speed * dt.asSeconds()) < 0) {
					rectangle.setPosition(sf::Vector2f(rectangle.getPosition().x, 0));
				}
				else {
					rectangle.move(sf::Vector2f(0, -(speed * dt.asSeconds())));
				}
			}
			else{
				//do moving down
				if (rectangle.getPosition().y + (speed * dt.asSeconds()) > 600 - rectangle.getSize().y - 1) {
					rectangle.setPosition(sf::Vector2f(rectangle.getPosition().x, 600 - rectangle.getSize().y - 1));
				}
				else {
					rectangle.move(sf::Vector2f(0, (speed * dt.asSeconds())));
				}
			}
		}

		void setPosition(sf::Vector2f position) {
			rectangle.setPosition(position);
		}

		float x() {
			return rectangle.getPosition().x;
		}

		float y() {
			return rectangle.getPosition().y;
		}

		float width() {
			return rectangle.getSize().x;
		}

		float height() {
			return rectangle.getSize().y;
		}
};

class ball
{
	public:
		sf::CircleShape circle;
		float angle;
		float speed;

		ball(sf::Vector2f startPos) {
			do
			{
				angle = (std::rand() % 360) * 2 * pi / 360;
			} while (std::abs(std::cos(angle)) < 0.7f);
			
			speed = 200.f;
			circle.setRadius(5.f);
			circle.setPosition(startPos);
		}

		void move(sf::Time dt) {
			circle.move(sf::Vector2f(cosf(angle) * speed * dt.asSeconds(), sinf(angle) * speed * dt.asSeconds()));
		}

		void returnBall() {
			do
			{
				angle = (std::rand() % 360) * 2 * pi / 360;
			} while (std::abs(std::cos(angle)) < 0.7f);

			speed = 200.f;
			circle.setRadius(5.f);
			circle.setPosition(sf::Vector2f(295.f, std::rand()%590));
		}

		void checkCollision(paddle leftP, paddle rightP, sf::Time dt) {
			if (circle.getPosition().y < 0) {
				//bounce off top
				angle = -angle;
				circle.setPosition(sf::Vector2f(circle.getPosition().x - (cosf(angle) * speed * dt.asSeconds() / 2), 0.1f));
				boingSound.play();
			}
			else if (circle.getPosition().y > 600 - circle.getRadius() * 2) {
				//bounce off bottom
				angle = -angle;
				circle.setPosition(sf::Vector2f(circle.getPosition().x - (cosf(angle) * speed * dt.asSeconds() / 2), 599.9f - circle.getRadius() * 2));
				boingSound.play();
			}

			float centerx = circle.getPosition().x + circle.getRadius();
			float centery = circle.getPosition().y + circle.getRadius();
			float testx = centerx;
			float testy = centery;
			float distx2 = 0;
			float disty2 = 0;

			if (centerx < leftP.x()) {
				testx = leftP.x();
			}
			else if (centerx > leftP.x() + leftP.width()) {
				testx = leftP.x() + leftP.width();
			}
			if (centery < leftP.y()) {
				testy = leftP.y();
			}
			else if (centery > leftP.y() + leftP.height()) {
				testy = leftP.y() + leftP.height();
			}
			distx2 = (centerx - testx) * (centerx - testx);
			disty2 = (centery - testy) * (centery - testy);
			if ((distx2 + disty2) <= (circle.getRadius() * circle.getRadius())) {
				//bounce off left paddle here
				angle = pi - angle;
				circle.setPosition(sf::Vector2f(leftP.x() + leftP.width() + 0.05f, circle.getPosition().y));
				speed += 50;
				whoaSound.play();
			}

			testx = centerx;
			testy = centery;
			distx2 = 0;
			disty2 = 0;

			if (centerx < rightP.x()) {
				testx = rightP.x();
			}
			else if (centerx > rightP.x() + rightP.width()) {
				testx = rightP.x() + rightP.width();
			}
			if (centery < rightP.y()) {
				testy = rightP.y();
			}
			else if (centery > rightP.y() + rightP.height()) {
				testy = rightP.y() + rightP.height();
			}
			distx2 = (centerx - testx) * (centerx - testx);
			disty2 = (centery - testy) * (centery - testy);
			if ((distx2 + disty2) <= (circle.getRadius() * circle.getRadius())) {
				//bounce off left paddle here
				angle = pi - angle + (2 * pi);
				circle.setPosition(sf::Vector2f(rightP.x() - (circle.getRadius() * 2) - 0.05f, circle.getPosition().y));
				speed += 50;
				whoaSound.play();
			}
		}

		float x() {
			return circle.getPosition().x + circle.getRadius();
		}

		float y() {
			return circle.getPosition().y + circle.getRadius();
		}
};

int main()
{
	int leftScore = 0;
	int rightScore = 0;
	sf::RenderWindow window(sf::VideoMode(600, 600), "Wolfe's Big Box o' Pong");
	
	sf::Text leftScoreText;
	leftScoreText.setFont(font);
	leftScoreText.setFillColor(sf::Color::Blue);
	leftScoreText.setCharacterSize(48);
	leftScoreText.setPosition(sf::Vector2f(40,40));
	sf::Text rightScoreText;
	rightScoreText.setFont(font);
	rightScoreText.setFillColor(sf::Color::White);
	rightScoreText.setCharacterSize(48);
	rightScoreText.setPosition(sf::Vector2f(522, 40));

	sf::Text playAgain;
	playAgain.setFont(font);
	playAgain.setFillColor(sf::Color::Green);
	playAgain.setCharacterSize(36);
	playAgain.setPosition(sf::Vector2f(60, 300));
	playAgain.setString("Press the spacebar to play again or esc to quit.");
	playAgain.setRotation(-30.f);

	if (!boingBuffer.loadFromFile("boingPong.wav")) {
		return -1;
	}
	boingSound.setBuffer(boingBuffer);
	
	if (!cheersBuffer.loadFromFile("cheers.wav")) {
		return -1;
	}
	cheersSound.setBuffer(cheersBuffer);

	if (!ahhhBuffer.loadFromFile("ahhh.wav")) {
		return -1;
	}
	ahhhSound.setBuffer(ahhhBuffer);

	if (!whoaBuffer.loadFromFile("whoaPong.wav")) {
		return -1;
	}
	whoaSound.setBuffer(whoaBuffer);

	if (!font.loadFromFile("steelfish.ttf"))
	{
		return -1;
	}
	if (!paddleTexture.loadFromFile("paddleTexture.jpg")){
		std::cout << "oops";
		return -1;
	}

	paddle leftPaddle(10.f, 295.f);
	leftPaddle.rectangle.setFillColor(sf::Color::Blue);

	paddle rightPaddle(580.f, 295.f);
	rightPaddle.rectangle.setTexture(&paddleTexture, true);

	ball playBall(sf::Vector2f(295, 295));

	bool addedScore = false;
	

	sf::Time dt;
	sf::Clock clock;
	while (window.isOpen())
	{
		dt = clock.getElapsedTime();
		clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			leftPaddle.move(true, dt);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			leftPaddle.move(false, dt);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			rightPaddle.move(true, dt);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			rightPaddle.move(false, dt);
		}

		playBall.move(dt);
		playBall.checkCollision(leftPaddle, rightPaddle, dt);

		if (playBall.x() < -5.f || playBall.x() > 605.f) {
			if (playBall.x() < -5.f && !addedScore) {
				//add score to right
				++rightScore;
				cheersSound.play();
				addedScore = true;
			}
			else if (playBall.x() > 605.f && !addedScore) {
				//add score to left
				++leftScore;
				ahhhSound.play();
				addedScore = true;
			}
			if (leftScore >= 5 || rightScore >= 5) {
				window.draw(playAgain);
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					//reset whole game
					leftScore = 0;
					rightScore = 0;
					playBall.returnBall();
					leftPaddle.setPosition(sf::Vector2f(leftPaddle.x(),(300 - leftPaddle.height()/2)));
					rightPaddle.setPosition(sf::Vector2f(rightPaddle.x(), (300 - rightPaddle.height() / 2)));
					addedScore = false;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					window.close();
				}
			}
			else {
				playBall.returnBall();
				addedScore = false;
			}
		}

		leftScoreText.setString(std::to_string(leftScore));
		rightScoreText.setString(std::to_string(rightScore));

		window.clear();
		window.draw(leftPaddle.rectangle);
		window.draw(rightPaddle.rectangle);
		window.draw(leftScoreText);
		window.draw(rightScoreText);
		if (leftScore >= 5 || rightScore >= 5) {
			window.draw(playAgain);
		}
		window.draw(playBall.circle);
		window.display();

		
	}

	return 0;
}