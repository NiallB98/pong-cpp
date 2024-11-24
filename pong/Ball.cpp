#include "Ball.h"

Ball::Ball()
{
	
}

Ball::~Ball()
{

}

void Ball::Init(int winW, int winH)
{
	sprite.setFillColor(sf::Color::White);
	sprite.setRadius(6.f);
	sprite.setOrigin(sf::Vector2f(sprite.getRadius(), sprite.getRadius()));
	sprite.setPosition(sf::Vector2f(static_cast<float>(winW)
		/ 2.f, static_cast<float>(winH) / 2.f));
}

void Ball::Reset(int winW, int winH)
{
	speed = speedOriginal;
	speedIncrement = 0;
	dir = (180 * (rand() % 2)) - (degreeMax / 2) + (rand() % degreeMax);				// In degrees
	speedX = speed * cos(dir * M_PI / 180);
	speedY = speed * -sin(dir * M_PI / 180);

	lastHit = 0;
}

void Ball::Update(int winW, int winH, bool notPaused, int scoreL, int scoreR, Game game)
{
	if (sprite.getPosition().y >= winH - sprite.getRadius()											// Hitting top or bottom
		|| sprite.getPosition().y <= sprite.getRadius())
	{
		speedY *= -1;
		dir *= -1;
		soundHit.play();
	}

	if (sprite.getPosition().x >= winW - sprite.getRadius()											// Hitting left or right
		|| sprite.getPosition().x <= sprite.getRadius())
	{
		if (sprite.getPosition().x <= sprite.getRadius())												// Score increases
		{
			scoreR += 1;
			soundPointUp.play();
		}
		else
		{
			scoreL += 1;
			soundPointUp.play();
		}
		resetAll();
	}

	if ((sprite.getGlobalBounds().intersects(bar2.getGlobalBounds()) && lastHit != 1)					// Hitting either bars
		|| (sprite.getGlobalBounds().intersects(bar1.getGlobalBounds()) && lastHit != -1))
	{
		speedX *= -1.f;
		float dif, maxDif, dirAdd, ratioSign;

		if (sprite.getGlobalBounds().intersects(bar1.getGlobalBounds()))								// Hitting left player
		{
			dirAdd = 0.f;
			ratioSign = 1.f;

			dif = bar1.getPosition().y + bar1.getSize().y / 2.f - sprite.getPosition().y;
			maxDif = bar1.getSize().y / 2.f;
			soundHit.play();

			lastHit = -1;
		}
		else if (sprite.getGlobalBounds().intersects(bar2.getGlobalBounds()))
		{																							// Hitting right player
			dirAdd = 180.f;
			ratioSign = -1.f;

			dif = bar2.getPosition().y + bar2.getSize().y / 2.f - sprite.getPosition().y;
			maxDif = bar2.getSize().y / 2.f;
			soundHit.play();

			lastHit = 1;
		}

		float ratio = dif / maxDif * 45.f;
		dir = dirAdd + ratioSign * ratio;
		speedIncrement++;
	}

	if (notPaused) {
		speed = (speedOriginal + speedIncrease * speedIncrement);

		speedX = speed * static_cast<float>(cos(dir * M_PI / 180));
		speedY = speed * -static_cast<float>(sin(dir * M_PI / 180));

		sprite.move(speedX, speedY);
	}
}