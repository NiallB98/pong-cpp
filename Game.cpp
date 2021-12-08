#include "Game.h"

Game::Game()
{
	width = 1024;
	height = 640;
	fps = 60;
	title = "C++ Pong Game";

	notPaused = false;
	pausePressed = false;

	pressedUp = 0.f;
	pressedDown = 0.f;

	scoreL = 0;
	scoreR = 0;

	moveSpeed = 6.f;

	ballSpeedOriginal = 5.f;
	ballSpeed = ballSpeedOriginal;
	speedIncrease = 0.5f;
	ballDegreeMax = 40;
	ballDir = (180 * (rand() % 2)) - (ballDegreeMax/2) + (rand() % ballDegreeMax); // In degrees
	ballSpeedX = ballSpeed * cos(ballDir * M_PI / 180);
	ballSpeedY = ballSpeed * -sin(ballDir * M_PI / 180);

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
	bar2.setPosition(sf::Vector2f(static_cast<float>(width) - bar2.getSize().x - bar1.getPosition().x, bar1.getPosition().y));

	aiOffset = 0.8*((rand() % (static_cast<int>(bar2.getSize().y))) - bar2.getSize().y / 2.f);
}

void Game::initBall()
{
	ball.setFillColor(sf::Color::White);
	ball.setRadius(6.f);
	ball.setOrigin(sf::Vector2f(ball.getRadius(), ball.getRadius()));
	ball.setPosition(sf::Vector2f(static_cast<float>(width) / 2.f, static_cast<float>(height) / 2.f));
}

void Game::initText()
{
	if (!font.loadFromFile("fonts/bitFont.ttf"))
	{
		std::cout << "Error: Could not load font" << std::endl;
		notPaused = true;
	}

	///// Pause font /////
	// select the font
	txtPause.setFont(font);

	// String to display
	txtPause.setString("||");

	// Char size
	txtPause.setCharacterSize(72); // in px

	sf::FloatRect textRect = txtPause.getLocalBounds();
	txtPause.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	txtPause.setPosition(sf::Vector2f(width / 2.f, 2.f * height / 3.f));

	// set the color
	txtPause.setFillColor(sf::Color::White);

	///// Score font /////
	// select the font
	txtScore.setFont(font);

	// String to display
	std::string text = std::to_string(scoreL)+" | "+ std::to_string(scoreR);
	txtScore.setString(text);

	// Char size
	txtScore.setCharacterSize(100); // in px

	textRect = txtScore.getLocalBounds();
	txtScore.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	txtScore.setPosition(sf::Vector2f(width / 2.f, height / 6.f));

	// set the color
	txtScore.setFillColor(sf::Color::White);
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

	ballSpeed = ballSpeedOriginal;
	ballDir = (180 * (rand() % 2)) - (ballDegreeMax / 2) + (rand() % ballDegreeMax); // In degrees
	ballSpeedX = ballSpeed * cos(ballDir * M_PI / 180);
	ballSpeedY = ballSpeed * -sin(ballDir * M_PI / 180);

	notPaused = false;

	// String to display
	std::string text = std::to_string(scoreL) + " | " + std::to_string(scoreR);
	txtScore.setString(text);
}

void Game::updateBars()
{
	///// Left bar /////
	// Checking bar can move
	float bar1Dir = pressedDown - pressedUp;
	float bar1Speed = moveSpeed * bar1Dir;
	if (bar1.getPosition().y + bar1Speed >= 0 && bar1.getPosition().y + bar1Speed <= height - bar1.getSize().y)
		bar1.move(0.f, bar1Speed);
	else if (bar1Dir != 0)
	{
		bar1.move(0.f, bar1Dir * std::min(bar1.getPosition().y, height - bar1.getPosition().y - bar1.getSize().y));
	}

	///// Right bar /////
	float ballRelativeY = ball.getPosition().y - (bar2.getPosition().y + bar2.getSize().y / 2 + aiOffset);

	float bar2Speed = std::min(std::abs(ballRelativeY), moveSpeed) * std::copysignf(1.f, ballRelativeY);

	bar2.move(0.f, bar2Speed);

	// If right bar is heading towards out of bounds
	if (bar2.getPosition().y + bar2.getSize().y > height)
		bar2.setPosition(sf::Vector2f(bar2.getPosition().x, height - bar2.getSize().y));
	else if (bar2.getPosition().y < 0)
		bar2.setPosition(sf::Vector2f(bar2.getPosition().x, 0));
}

void Game::updateBall()
{
	// Hitting top or bottom
	if (ball.getPosition().y >= height - ball.getRadius() || ball.getPosition().y <= ball.getRadius())
	{
		ballSpeedY *= -1;
		ballDir *= -1;
		soundBallHit.play();
	}

	// Hitting left or right
	if (ball.getPosition().x >= width - ball.getRadius() || ball.getPosition().x <= ball.getRadius())
	{
		// Score increases
		if (ball.getPosition().x <= ball.getRadius())
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

	// Hitting either bars
	if (ball.getGlobalBounds().intersects(bar2.getGlobalBounds())
		|| ball.getGlobalBounds().intersects(bar1.getGlobalBounds()))
	{
		ballSpeedX *= -1.f;
		float dif, maxDif, dirAdd, ratioSign;

		// Hitting left player
		if (ball.getGlobalBounds().intersects(bar1.getGlobalBounds()))
		{
			dirAdd = 0.f;
			ratioSign = 1.f;

			dif = bar1.getPosition().y + bar1.getSize().y / 2.f - ball.getPosition().y;
			maxDif = bar1.getSize().y / 2.f;
			soundBallHit.play();
		}
		else
		// Hitting right player
		{
			dirAdd = 180.f;
			ratioSign = -1.f;

			dif = bar2.getPosition().y + bar2.getSize().y / 2.f - ball.getPosition().y;
			maxDif = bar2.getSize().y / 2.f;
			soundBallHit.play();
		}

		float ratio = dif / maxDif * 45.f;

		ballDir = dirAdd + ratioSign*ratio;

		ballSpeed += speedIncrease;

		ballSpeedX = ballSpeed * static_cast<float>(cos(ballDir * M_PI / 180));
		ballSpeedY = ballSpeed * -static_cast<float>(sin(ballDir * M_PI / 180));
	}
	
	if (notPaused) ball.move(ballSpeedX, ballSpeedY);
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
	// Esc key
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		window->close();
		running = false;
	}

	// R Key
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
	{
		resetAll();
	}

	// Space Key
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !pausePressed)
	{
		notPaused = !notPaused;
		pausePressed = true;
	}
	else if (pausePressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		pausePressed = false;
	}

	// Up key
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		pressedUp = 1.f;
	else
		pressedUp = 0.f;

	// Down key
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
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

	// Drawing text
	if (!notPaused) window->draw(txtPause);
	window->draw(txtScore);

	window->display();
}

void Game::initWindow()
{
	window = new sf::RenderWindow(sf::VideoMode(width, height), title);
	window->setFramerateLimit(fps);
	running = true;
}
