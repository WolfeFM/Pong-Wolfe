#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

const float pi = 3.14159f;

class paddle
{
	public: 
		sf::RectangleShape rectangle;
		float speed = 0.05f;

		paddle(float x, float y) {
			rectangle.setPosition(x, y);
			rectangle.setSize(sf::Vector2f(10, 50));
		}

		void move(bool up) {
			if (up)
			{
				//do moving up
				if (rectangle.getPosition().y - speed < 0) {
					rectangle.setPosition(sf::Vector2f(rectangle.getPosition().x, 0));
				}
				else {
					rectangle.move(sf::Vector2f(0, -speed));
				}
			}
			else{
				//do moving down
				if (rectangle.getPosition().y + speed > 560) {
					rectangle.setPosition(sf::Vector2f(rectangle.getPosition().x, 560));
				}
				else {
					rectangle.move(sf::Vector2f(0, speed));
				}
			}
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
		sf::Vector2f velocity;

		ball(sf::Vector2f startPos) {
			do
			{
				angle = (std::rand() % 360) * 2 * pi / 360;
			} while (std::abs(std::cos(angle)) < 0.7f);
			
			speed = 0.05f;
			circle.setRadius(5.f);
			circle.setPosition(startPos);
			this->updateVel();
		}

		void updateVel() {
			velocity = sf::Vector2f(cosf(angle) * speed, sinf(angle) * speed);
		}

		void move() {
			circle.move(velocity);
		}

		void checkCollision(paddle leftP, paddle rightP) {
			if (circle.getPosition().y < 0) {
				//bounce off top
				angle = -angle;
				updateVel();
				circle.setPosition(sf::Vector2f(circle.getPosition().x - (velocity.x / 2), 0.1f));
			}
			else if (circle.getPosition().y > 600) {
				//bounce off bottom
				angle = -angle;
				updateVel();
				circle.setPosition(sf::Vector2f(circle.getPosition().x - (velocity.x / 2), 599.9f));
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
				updateVel();
				circle.setPosition(sf::Vector2f(leftP.x() + leftP.width() + 0.05f, circle.getPosition().y));
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
				updateVel();
				circle.setPosition(sf::Vector2f(rightP.x() - (circle.getRadius() * 2) - 0.05f, circle.getPosition().y));
			}
		}
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "Wolfe's Big Box o' Pong");

	paddle leftPaddle(10.f,295.f);
	leftPaddle.rectangle.setFillColor(sf::Color::Blue);

	paddle rightPaddle(580.f, 295.f);

	ball playBall(sf::Vector2f(295, 295));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			leftPaddle.move(true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			leftPaddle.move(false);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			rightPaddle.move(true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			rightPaddle.move(false);
		}

		playBall.move();
		playBall.checkCollision(leftPaddle, rightPaddle);

		window.clear();
		window.draw(leftPaddle.rectangle);
		window.draw(rightPaddle.rectangle);
		window.draw(playBall.circle);
		window.display();
	}

	return 0;
}