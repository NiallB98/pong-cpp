#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>

class Game
{
private:
	int width, height, fps, ballDegreeMax, scoreL, scoreR, speedIncrement, lastHit;
	float moveSpeed, pressedUp, pressedDown, aiOffset, ballSpeed,
		ballSpeedOriginal, ballSpeedX, ballSpeedY, ballDir, speedIncrease;
	const char* title;
	bool notPaused, pausePressed;

	sf::SoundBuffer bufferBallHit, bufferPointUp;													// Sounds
	sf::Sound soundBallHit, soundPointUp;
	float volumeFX;

	sf::RenderWindow* window;
	sf::Event ev;

	sf::RectangleShape bar1;
	sf::RectangleShape bar2;
	sf::CircleShape ball;
	sf::Text txtPause;
	sf::Font font;
	sf::Text txtScore;

	void initWindow();
	void initBars();
	void initBall();
	void resetAll();
	void initText();
	void initAudio();

public:
	Game();
	~Game();

	bool running = false;

	void updateBars();
	void updateBall();
	void pollEvents();
	void update();
	void render();

	sf::Clock clock;																				// To keep track of delta time use "sf::Time dt = clock.getElapsedTime();" (Use dt.asMilliseconds())
};

