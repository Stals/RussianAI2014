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

    if (world.getPuck().getOwnerPlayerId() == self.getPlayerId()) {
        if (world.getPuck().getOwnerHockeyistId() == self.getId()) {
            Player opponentPlayer = world.getOpponentPlayer();

            double netX = 0.5 * (opponentPlayer.getNetBack() + opponentPlayer.getNetFront());
            double netY = 0.5 * (opponentPlayer.getNetBottom() + opponentPlayer.getNetTop());
            netY += (self.getY() < netY ? 0.5 : -0.5) * game.getGoalNetHeight();

            double angleToNet = self.getAngleTo(netX, netY);
            move.setTurn(angleToNet);

            if (abs(angleToNet) < STRIKE_ANGLE) {
                move.setAction(SWING);
            }
        } else {
            const Hockeyist* nearestOpponent = getNearestOpponent(self.getX(), self.getY(), world);
            if (nearestOpponent != 0) {
                if (self.getDistanceTo(*nearestOpponent) > game.getStickLength()) {
                    move.setSpeedUp(1.0);
                } else if (abs(self.getAngleTo(*nearestOpponent)) < 0.5 * game.getStickSector()) {
                    move.setAction(STRIKE);
                }
                move.setTurn(self.getAngleTo(*nearestOpponent));
            }
        }
    } else {
		moveToPuck(gd);
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
    gd.move.setSpeedUp(1.0);
    gd.move.setTurn(gd.self.getAngleTo(gd.world.getPuck()));
    gd.move.setAction(TAKE_PUCK);
 }
