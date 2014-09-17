#pragma once

#ifndef _MY_STRATEGY_H_
#define _MY_STRATEGY_H_

#include "Strategy.h"

using namespace model;

struct GameData{
	GameData(const Hockeyist& self, const World& world, const Game& game, Move& move) :
		self(self), world(world), game(game), move(move){
	}

	const Hockeyist& self;
	const World& world;
	const Game& game;
	Move& move;
};

struct Point{
	Point(double x, double y):x(x), y(y){	
	}

	double x;
	double y;
};

class MyStrategy : public Strategy {
public:
    MyStrategy();

    void move(const model::Hockeyist& self, const model::World& world, const model::Game& game, model::Move& move);

private:
	static const model::Hockeyist* getNearestOpponent(double x, double y, const World &world);

	void moveToPuck(GameData& gameData);

	// returns net point to aim for
	Point getNetPoint(GameData& gameData);
};

#endif
