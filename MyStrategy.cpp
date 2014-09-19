#include "MyStrategy.h"

#define PI 3.14159265358979323846
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>


#include "Logger.h"

using namespace std;

const double STRIKE_ANGLE = 1.0 * PI / 180.0;
const double degreesPerRadian = 57.2957;

MyStrategy::MyStrategy() { }


void MyStrategy::move(const Hockeyist& self, const World& world, const Game& game, Move& move) {
	GameData gd(self, world, game, move);

	// DEBUG - so that only one moves
	if(gd.world.getOpponentPlayer().getId() == 0){
		return;
	}

	if (self.getState() == SWINGING) {
        move.setAction(STRIKE);
        return;
    }

    if (ownPuck(gd)) {
        if (unitWithPuck(gd, self)){

			Point strikePoint = getStikeAreaPoint(gd);
			if(inStrikeArea(gd, strikePoint)){
				// if already there
				turnAndSwing(gd);
			}else{
				// TODO - тут можно подумать еще насчет того, что если ты около ворот, то тебе лучше ехать задом, чем повернуться поехать, потом повернуться и ударить
				// тоесть в функции moveTo еще учитывать угол который должен быть в рузальтате и смотря на это выбрать как ехать
				moveTo(gd, strikePoint);
			}          

        } else {
			attackNearest(gd);
            
        }
    } else {
		// TODO если нужно повернуться на несколько градусов то делать этою
		// TODO возможно не лучшая идея, если они летят в сторону шайбы.
		strikeEnemyInRange(gd);
		// если можно ехать и бить одновременно
		// Note: will take puck instead if in range
		moveToPuck(gd);
    }
}

Point MyStrategy::getStikeAreaPoint(GameData& gd)
{
	double targetX = 0;
	if(enemyOnLeft(gd)){
		targetX = 300;
	}else{
		targetX = 900;
	}

	double targetY = 0;
	if( gd.self.getY() > getEnemyNetCenterY(gd)){
		targetY = 610;
	}else{
		targetY = 310;
	}

	return Point(targetX, targetY);
}

bool MyStrategy::inStrikeArea(GameData& gd, Point& strikePoint)
{
	const double okRadius = 75.0;
	return gd.self.getDistanceTo(strikePoint.x, strikePoint.y) <= okRadius;
}

bool MyStrategy::ownPuck(GameData& gd)
{
	return gd.world.getPuck().getOwnerPlayerId() == gd.self.getPlayerId();
}

bool MyStrategy::unitWithPuck(GameData& gd, const Hockeyist& hockeist)
{
	return gd.world.getPuck().getOwnerHockeyistId() == hockeist.getId();
}

bool MyStrategy::strikeEnemyInRange(GameData& gd)
{
	const Hockeyist* nearestOpponent = getNearestOpponent(gd.self.getX(), gd.self.getY(), gd.world);
	if (nearestOpponent != 0) {
		if(inStickRange(gd, *nearestOpponent)){
			gd.move.setAction(STRIKE);
			return true;
		}
	}
	return false;
}

bool MyStrategy::inStickRange(GameData& gd, const Unit& unit)
{
	if (gd.self.getDistanceTo(unit) <= gd.game.getStickLength()) {
		if (abs(gd.self.getAngleTo(unit)) < 0.5 * gd.game.getStickSector()) {
			return true;
		}
	}

	return false;
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

double MyStrategy::getEnemyNetCenterY(GameData& gd)
{
	Player opponentPlayer = gd.world.getOpponentPlayer();
	return 0.5 * (opponentPlayer.getNetBottom() + opponentPlayer.getNetTop());
}

Point MyStrategy::getNetPoint(GameData& gd)
{
	Player opponentPlayer = gd.world.getOpponentPlayer();
	double netX = 0.5 * (opponentPlayer.getNetBack() + opponentPlayer.getNetFront());
    double netY = getEnemyNetCenterY(gd);
	
	double netYShift = 0.5 * gd.game.getGoalNetHeight();
	netYShift *= gd.self.getY() < netY ? 1 : -1;
	netY += netYShift;

	/*double netYShift = 0.5 * gd.game.getGoalNetHeight() - gd.world.getPuck().getRadius();
	netYShift *= gd.self.getY() < netY ? 1 : -1;
	netY += netYShift;*/
	
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

	// slow down
	gd.move.setSpeedUp(getSlowDownSpeed(gd));

	

    if (abs(angleToNet) < STRIKE_ANGLE) {
        gd.move.setAction(SWING);
    }
}


double MyStrategy::linear_interpolation(double Kstart, double Kend, double Vstart, double Vend, double K)
{
	if (K <= Kstart)
		return Vstart;
	else if (K >= Kend)
		return Vend;
	else
		return Vstart + ((K - Kstart) / (Kend - Kstart)) * (Vend - Vstart);
}

double MyStrategy::getSlowDownSpeed(GameData& gd)
{
	double unitVectorAngle = atan2(gd.self.getSpeedY(),gd.self.getSpeedX()) * degreesPerRadian;
	double unitActualAngle = gd.self.getAngle() * degreesPerRadian;

	double max = std::max(fabs(unitVectorAngle), fabs(unitActualAngle));
	double min = std::min(fabs(unitVectorAngle), fabs(unitActualAngle));

	double delta = max - min;

	// если направление и скорость совпадат, то должно быть -1
	// если они противоположны - то 1

	double newSpeed = linear_interpolation( 0, 180, -1, 1, delta);

	return newSpeed;
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

	if(inStickRange(gd, gd.world.getPuck())){
		gd.move.setAction(TAKE_PUCK);
	}
 }


 void MyStrategy::moveTo(GameData& gd, double x, double y)
 {
	gd.move.setSpeedUp(getSpeed(gd.self.getAngleTo(x, y)));
    gd.move.setTurn(gd.self.getAngleTo(x, y));
 }

 void MyStrategy::moveTo(GameData& gd, const Unit& unit)
 {
	 moveTo(gd, unit.getX(), unit.getY());
 }

 void MyStrategy::moveTo(GameData& gd, const Point& point)
 {
	 moveTo(gd, point.x, point.y);
 }

 double MyStrategy::getSpeed(double angle)
 {
	return std::cos(angle * (PI / 180.0));
 }

 bool MyStrategy::enemyOnLeft(GameData& gd)
 {
	 return gd.world.getOpponentPlayer().getNetBack() <= 100;
 }