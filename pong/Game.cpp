#include "Game.h"

Game::Game()
{
	width = 1024;
	height = 640;
	fps = 144;
	title = "C++ Pong Game";

	notPaused = false;
	pausePressed = false;

	pressedUp = 0.f;
	pressedDown = 0.f;

	scoreL = 0;
	scoreR = 0;

	moveSpeed = 0.4f;

	ballSpeedOriginal = 0.4f;
	ballSpeed = ballSpeedOriginal;
	speedIncrease = 0.04f;
	speedIncrement = 0;
	ballDegreeMax = 40;
	ballDir = (180 * (rand() % 2)) - (ballDegreeMax / 2) + (rand() % ballDegreeMax);					// In degrees
	ballSpeedX = ballSpeed * cos(ballDir * M_PI / 180);
	ballSpeedY = ballSpeed * -sin(ballDir * M_PI / 180);

	lastHit = 0;

	initWindow();
	initBars();
	initBall();
	initText();
	initAudio();
}

Game::~Game()
{
	delete window;
}

void Game::initBars()
{
	// Left bar
	bar1.setFillColor(sf::Color::White);
	bar1.setSize(sf::Vector2f(20.f, 100.f));
	bar1.setPosition(sf::Vector2f(70.f, (static_cast<float>(height) - bar1.getSize().y) / 2.f));

	// Right bar
	bar2.setFillColor(sf::Color::White);
	bar2.setSize(sf::Vector2f(20.f, 100.f));
	bar2.setPosition(sf::Vector2f(static_cast<float>(width)
		- bar2.getSize().x - bar1.getPosition().x, bar1.getPosition().y));

	aiOffset = 0.8 * ((rand() % (static_cast<int>(bar2.getSize().y))) - bar2.getSize().y / 2.f);
}

void Game::initBall()
{
	ball.setFillColor(sf::Color::White);
	ball.setRadius(6.f);
	ball.setOrigin(sf::Vector2f(ball.getRadius(), ball.getRadius()));
	ball.setPosition(sf::Vector2f(static_cast<float>(width)
		/ 2.f, static_cast<float>(height) / 2.f));
}

void Game::initText()
{
	if (!font.loadFromFile("fonts/bitFont.ttf"))
	{
		std::cout << "Error: Could not load font" << std::endl;
		notPaused = true;
	}

	txtPause.setFont(font);																			// Select pause font
	txtPause.setString("||");																		// String to display
	txtPause.setCharacterSize(72);																	// Char size in px
	sf::FloatRect txtRect = txtPause.getLocalBounds();
	txtPause.setOrigin(txtRect.left + txtRect.width / 2.0f, txtRect.top + txtRect.height / 2.0f);
	txtPause.setPosition(sf::Vector2f(width / 2.f, 2.f * height / 3.f));
	txtPause.setFillColor(sf::Color::White);														// Set font color

	txtScore.setFont(font);																			// Select score font
	std::string text = std::to_string(scoreL) + " | " + std::to_string(scoreR);						// String to display
	txtScore.setString(text);
	txtScore.setCharacterSize(100);																	// Char size in px
	txtRect = txtScore.getLocalBounds();
	txtScore.setOrigin(txtRect.left + txtRect.width / 2.0f, txtRect.top + txtRect.height / 2.0f);
	txtScore.setPosition(sf::Vector2f(width / 2.f, height / 6.f));
	txtScore.setFillColor(sf::Color::White);														// Set font color
}

void Game::initAudio()
{
	if (!bufferBallHit.loadFromFile("audio/ballHit/ballHit.wav"))
		std::cout << "Error: Could not load audio ballHit.wav" << std::endl;
	if (!bufferPointUp.loadFromFile("audio/pointUp/pointUp.wav"))
		std::cout << "Error: Could not load audio pointUp.wav" << std::endl;

	soundBallHit.setBuffer(bufferBallHit);
	soundPointUp.setBuffer(bufferPointUp);

	volumeFX = 50;

	soundBallHit.setVolume(volumeFX);
	soundPointUp.setVolume(volumeFX);
}

void Game::resetAll()
{
	srand(time(NULL));
	initBars();
	initBall();

	float dt = static_cast<float>(clock.getElapsedTime().asMilliseconds());

	ballSpeed = ballSpeedOriginal * dt;
	speedIncrement = 0;
	ballDir = (180 * (rand() % 2)) - (ballDegreeMax / 2) + (rand() % ballDegreeMax);				// In degrees
	ballSpeedX = ballSpeed * cos(ballDir * M_PI / 180);
	ballSpeedY = ballSpeed * -sin(ballDir * M_PI / 180);

	notPaused = false;
	lastHit = 0;

	std::string text = std::to_string(scoreL) + " | " + std::to_string(scoreR);						// String to display
	txtScore.setString(text);
}

void Game::updateBars()
{
	float dt = static_cast<float>(clock.getElapsedTime().asMilliseconds());
	
	float bar1Dir = pressedDown - pressedUp;														// Checking left bar can move
	float bar1Speed = dt * moveSpeed * bar1Dir;

	if (bar1.getPosition().y + bar1Speed >= 0
		&& bar1.getPosition().y + bar1Speed <= height - bar1.getSize().y)
	{
		bar1.move(0.f, bar1Speed);
	}
	else if (bar1Dir != 0)
	{
		bar1.move(0.f, bar1Dir * std::min(bar1.getPosition().y,
			height - bar1.getPosition().y - bar1.getSize().y));
	}


	float ballRelativeY = ball.getPosition().y - (bar2.getPosition().y								// Right bar
		+ bar2.getSize().y / 2 + aiOffset);
	float bar2Speed = std::min(std::abs(ballRelativeY), moveSpeed * dt)
		* std::copysignf(1.f, ballRelativeY);

	bar2.move(0.f, bar2Speed);


	if (bar2.getPosition().y + bar2.getSize().y > height)											// If right bar is heading towards out of bounds
		bar2.setPosition(sf::Vector2f(bar2.getPosition().x, height - bar2.getSize().y));
	else if (bar2.getPosition().y < 0)
		bar2.setPosition(sf::Vector2f(bar2.getPosition().x, 0));
}

void Game::updateBall()
{
	float dt = static_cast<float>(clock.getElapsedTime().asMilliseconds());

	if (ball.getPosition().y >= height - ball.getRadius()											// Hitting top or bottom
		|| ball.getPosition().y <= ball.getRadius())
	{
		ballSpeedY *= -1;
		ballDir *= -1;
		soundBallHit.play();
	}

	if (ball.getPosition().x >= width - ball.getRadius()											// Hitting left or right
		|| ball.getPosition().x <= ball.getRadius())
	{
		if (ball.getPosition().x <= ball.getRadius())												// Score increases
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

	if ((ball.getGlobalBounds().intersects(bar2.getGlobalBounds()) && lastHit != 1)					// Hitting either bars
		|| (ball.getGlobalBounds().intersects(bar1.getGlobalBounds()) && lastHit != -1))
	{
		ballSpeedX *= -1.f;
		float dif, maxDif, dirAdd, ratioSign;

		if (ball.getGlobalBounds().intersects(bar1.getGlobalBounds()))								// Hitting left player
		{
			dirAdd = 0.f;
			ratioSign = 1.f;

			dif = bar1.getPosition().y + bar1.getSize().y / 2.f - ball.getPosition().y;
			maxDif = bar1.getSize().y / 2.f;
			soundBallHit.play();

			lastHit = -1;
		}
		else if (ball.getGlobalBounds().intersects(bar2.getGlobalBounds()))
		{																							// Hitting right player
			dirAdd = 180.f;
			ratioSign = -1.f;

			dif = bar2.getPosition().y + bar2.getSize().y / 2.f - ball.getPosition().y;
			maxDif = bar2.getSize().y / 2.f;
			soundBallHit.play();

			lastHit = 1;
		}

		float ratio = dif / maxDif * 45.f;
		ballDir = dirAdd + ratioSign * ratio;
		speedIncrement++;
	}

	if (notPaused) {
		ballSpeed = (ballSpeedOriginal + speedIncrease * speedIncrement) * dt;

		ballSpeedX = ballSpeed * static_cast<float>(cos(ballDir * M_PI / 180));
		ballSpeedY = ballSpeed * -static_cast<float>(sin(ballDir * M_PI / 180));

		ball.move(ballSpeedX, ballSpeedY);
	}

}

void Game::pollEvents()
{
	while (window->pollEvent(ev))
	{
		if (ev.type == sf::Event::Closed)
		{
			window->close();
			running = false;
			break;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))										// Esc key
	{
		window->close();
		running = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))											// R Key
	{
		resetAll();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !pausePressed)						// Space Key
	{
		notPaused = !notPaused;
		pausePressed = true;
	}
	else if (pausePressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		pausePressed = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))											// Up key
		pressedUp = 1.f;
	else
		pressedUp = 0.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))											// Down key
		pressedDown = 1.f;
	else
		pressedDown = 0.f;
}

void Game::update()
{
	pollEvents();

	if (notPaused)
	{
		updateBars();
		updateBall();
	}
}

void Game::render()
{
	window->clear();

	window->draw(bar1);
	window->draw(bar2);
	window->draw(ball);

	if (!notPaused) window->draw(txtPause);															// Drawing text
	window->draw(txtScore);

	Game::clock.restart();																			// Resetting clock to zero
	window->display();
}

void Game::initWindow()
{
	window = new sf::RenderWindow(sf::VideoMode(width, height), title);
	window->setFramerateLimit(fps);
	sf::Image icon;
	if (!icon.loadFromFile("icon/icon.png"))
		exit(EXIT_FAILURE);
	window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	running = true;
}
