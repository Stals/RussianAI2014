#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>


using namespace std;

const double STRIKE_ANGLE = 1.0 * PI / 180.0;


MyStrategy::MyStrategy() { }


void MyStrategy::move(const Hockeyist& self, const World& world, const Game& game, Move& move) {
	GameData gd(self, world, game, move);
	
	if (self.getState() == SWINGING) {
        move.setAction(STRIKE);
        return;
    }

    if (ownPuck(gd)) {
        if (unitWithPuck(gd, self)){
            turnAndSwing(gd);

        } else {
			attackNearest(gd);
            
        }
    } else {
		moveToPuck(gd);
    }
}

bool MyStrategy::ownPuck(GameData& gd)
{
	return gd.world.getPuck().getOwnerPlayerId() == gd.self.getPlayerId();
}

bool MyStrategy::unitWithPuck(GameData& gd, const Hockeyist& hockeist)
{
	return gd.world.getPuck().getOwnerHockeyistId() == hockeist.getId();
}

void MyStrategy::attackNearest(GameData& gd)
{
	const Hockeyist* nearestOpponent = getNearestOpponent(gd.self.getX(), gd.self.getY(), gd.world);
    if (nearestOpponent != 0) {
        if (gd.self.getDistanceTo(*nearestOpponent) > gd.game.getStickLength()) {
            moveTo(gd, *nearestOpponent);

        } else if (abs(gd.self.getAngleTo(*nearestOpponent)) < 0.5 * gd.game.getStickSector()) {
            gd.move.setAction(STRIKE);
        }
        //move.setTurn(self.getAngleTo(*nearestOpponent));
    }
}

Point MyStrategy::getNetPoint(GameData& gd)
{
	Player opponentPlayer = gd.world.getOpponentPlayer();
	double netX = 0.5 * (opponentPlayer.getNetBack() + opponentPlayer.getNetFront());
    double netY = 0.5 * (opponentPlayer.getNetBottom() + opponentPlayer.getNetTop());
	
	double netYShift = 0.5 * gd.game.getGoalNetHeight() - gd.world.getPuck().getRadius();
	netYShift *= gd.self.getY() < netY ? 1 : -1;

	netY += netYShift;
	
	// TODO also change X - depenging on the side
	//netX += 

	return Point(netX, netY);
	// TODO нужно либо учитывать свой вектор скорости
	// либо перед тем как бить полностью останавливаться (достаточно близко к 0)
}

void MyStrategy::turnAndSwing(GameData& gd)
{
	Point netPoint = getNetPoint(gd);
    double angleToNet = gd.self.getAngleTo(netPoint.x, netPoint.y);
    gd.move.setTurn(angleToNet);

    if (abs(angleToNet) < STRIKE_ANGLE) {
        gd.move.setAction(SWING);
    }
}

 const Hockeyist* MyStrategy::getNearestOpponent(double x, double y, const World &world) {
        const Hockeyist* nearestOpponent = 0;
        double nearestOpponentRange = 0.0;

		const std::vector<model::Hockeyist>& hockeists = world.getHockeyists();
		size_t size = hockeists.size();

		for(size_t i = 0; i < size; ++i){
			const Hockeyist& hockeyist = hockeists[i];
       
            if (hockeyist.isTeammate() || hockeyist.getType() == GOALIE
                    || hockeyist.getState() == KNOCKED_DOWN
                    || hockeyist.getState() == RESTING) {
                continue;
            }

            double opponentRange = hypot(x - hockeyist.getX(), y - hockeyist.getY());

            if (nearestOpponent == 0 || opponentRange < nearestOpponentRange) {
                nearestOpponent = &hockeyist;
                nearestOpponentRange = opponentRange;
            }
        }

        return nearestOpponent;
    }


 void MyStrategy::moveToPuck(GameData& gd)
 {
	moveTo(gd, gd.world.getPuck());
    gd.move.setAction(TAKE_PUCK);
 }

 void MyStrategy::moveTo(GameData& gd, double x, double y)
 {
	gd.move.setSpeedUp(1.0);
    gd.move.setTurn(gd.self.getAngleTo(x, y));
 }

 void MyStrategy::moveTo(GameData& gd, const Unit& unit)
 {
	 moveTo(gd, unit.getX(), unit.getY());
 }