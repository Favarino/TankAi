#pragma once
#include "TankBattleHeaders.h"
#include "Vector2.h"
#include <math.h>
#include <iostream>


class Agent
{
	tankNet::TankBattleStateData *current;
	tankNet::TankBattleCommand tbc;
	
	enum Turret {SCAN,AIM,FIRE} turret = SCAN;
	enum Tank {EVADE, CHASE, RANDOMIZE, RETREAT } tank = CHASE;
	size_t aimTarget;
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
		tbc.cannonMove = tankNet::CannonMovementOptions::HALT;
		tbc.fireWish = 1;
		turret = SCAN;
	}
	
	void Chase()
	{
		Vector2 dir = normal(Vector2(current->position[0], current->position[2]) - Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		float dis = distance(Vector2(current->position[0], current->position[2]), Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		std::cout << dis << "\n";
		if (dis > 13)
		{
			//movement shit
		}
	}
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