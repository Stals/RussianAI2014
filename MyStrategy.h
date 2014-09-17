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

	void moveToPuck(GameData& gd);

	// returns net point to aim for
	Point getNetPoint(GameData& gd);
	void turnAndSwing(GameData& gd);

	// TODO implement better algoritm
	void moveTo(GameData& gd, double x, double y);
	void moveTo(GameData& gd, const Unit& unit);

	void attackNearest(GameData& gd);

	// we control puck
	bool ownPuck(GameData& gd);
	// this is the unit with puck
	bool unitWithPuck(GameData& gd, const Hockeyist& hockeist);

	double getSpeed(double angle);
};

#endif
