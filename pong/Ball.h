#pragma once

#include "Game.h"

class Ball
{
private:
	sf::CircleShape sprite;
	int degreeMax, speedIncrement, lastHit;
	float speed, speedOriginal, speedX, speedY, dir, speedIncrease;

	sf::SoundBuffer bufferBallHit, bufferPointUp;													// Sounds
	sf::Sound soundHit, soundPointUp;

public:
	void Init(int winW, int winH);
	void Reset(int winW, int winH);
	void Update(int winW, int winH, bool notPaused, int scoreL, int scoreR);
};

