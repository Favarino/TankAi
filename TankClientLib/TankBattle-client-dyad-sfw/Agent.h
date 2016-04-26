#pragma once
#include "TankBattleHeaders.h"
#include "Vector2.h"
#include <math.h>
#include <iostream>
#include "sfwdraw.h"

class Agent
{
	tankNet::TankBattleStateData *current;
	tankNet::TankBattleCommand tbc;
	
	enum Turret {SCAN,AIM,FIRE} turret = SCAN;
	enum Tank {EVADE, CHASE, WANDER, RETREAT } tank = CHASE;
	size_t aimTarget;

	float randTimer = 0;

	// Active target location to pursue
	Vector2 target;

	void Scan()
	{
		
		tbc.cannonMove = tankNet::CannonMovementOptions::RIGHT;
		tbc.fireWish = 0;
		for (aimTarget = 0; aimTarget < 3; ++aimTarget)
		{
			if (current->tacticoolData[aimTarget].inSight)
			{
				turret = AIM;
				tbc.cannonMove = tankNet::CannonMovementOptions::HALT;
			}
		}
	}	
	void Aim()
	{

		Vector2 dir = normal(Vector2(current->position[0], current->position[2]) - Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));

				float boop = dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2]));

				//std::cout << boop << "\n";

				if (dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2])) > 0)
				{
					tbc.cannonMove = tankNet::CannonMovementOptions::LEFT;
				}
				if (dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2])) < 0)
				{
					tbc.cannonMove = tankNet::CannonMovementOptions::RIGHT;
				}

				if (dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2])) > .01)
				{
					turret = FIRE;
				}
	}
	void Fire()
	{
		//tbc.cannonMove = tankNet::CannonMovementOptions::HALT;
		//tbc.fireWish = 1;
		turret = SCAN;
	}
	
	void Wander()
	{
		Vector2 dir = normal(Vector2(current->position[0], current->position[2]) - Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		float dis = distance(Vector2(current->position[0], current->position[2]), Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		Vector2 cp = Vector2::fromXZ(current->position); // current position
		Vector2 cf = Vector2::fromXZ(current->forward);  // current forward


		randTimer -= sfw::getDeltaTime();

		// If we're pretty close to the target, get a new target           
		if (distance(target, cp) < 20 || randTimer < 0)
		{
			target = Vector2::random() * Vector2 { 50, 50 };
			randTimer = rand() % 4 + 2; // every 2-6 seconds randomly pick a new target
		}



		// determine the forward to the target (which is the next waypoint in the path)
		Vector2 tf = normal(target - cp);

		if (dot(cf, tf) > .87f) // if the dot product is close to lining up, move forward
			tbc.tankMove = tankNet::TankMovementOptions::FWRD;
		else // otherwise turn right until we line up!
			tbc.tankMove = tankNet::TankMovementOptions::RIGHT;
	}

	void Chase()
	{
		Vector2 dir = normal(Vector2(current->position[0], current->position[2]) - Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		float dis = distance(Vector2(current->position[0], current->position[2]), Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		Vector2 cp = Vector2::fromXZ(current->position); // current position
		Vector2 cf = Vector2::fromXZ(current->forward);  // current forward
		
		

		target = Vector2::fromXZ(current->tacticoolData[aimTarget].lastKnownPosition);
		float blueg = (cp.getAngleBetween(target) - cf.angle());
		
		std::cout << blueg << "\n";
		
		if (blueg > 0)
		{
			tbc.tankMove = tankNet::TankMovementOptions::LEFT;
		}
		else
		{
			tbc.tankMove = tankNet::TankMovementOptions::RIGHT;
		}
		
	};
public:
	tankNet::TankBattleCommand update(tankNet::TankBattleStateData *state)
	{
		
		current = state;

		switch (turret)
		{
			case SCAN : Scan(); break;
			case AIM: Aim(); break;
			case FIRE: Fire(); break;
		}
		switch (tank)
		{
		case CHASE: Chase(); break;
		}
		return tbc;
		
	    
	}
};