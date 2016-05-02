#pragma once
#include "TankBattleHeaders.h"
#include "Vector2.h"
#include <math.h>
#include <iostream>
#include "sfwdraw.h"

class Agent
{
	tankNet::TankBattleStateData *current;
	tankNet::TankBattleCommand tbc, prevtbc;
	
	enum Turret {SCAN,AIM,FIRE} turret = SCAN;
	enum Tank {EVADE, CHASE, WANDER, RETREAT, UNSTICK } tank = WANDER;
	size_t aimTarget;

	float randTimer = 0;
	float framecount;
	float stickTimer = 2;
	// Active target location to pursue
	Vector2 target;

	//Turn the turret until we see an enemy
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
	//Aim the turret at the enemy
	void Aim()
	{

		Vector2 dir = normal(Vector2(current->position[0], current->position[2]) - Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		float dis = distance(Vector2(current->position[0], current->position[2]), Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		float boop = dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2]));

		std::cout << dis << "\n";

		if (dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2])) > 0)
		{
			tbc.cannonMove = tankNet::CannonMovementOptions::LEFT;
		}
		if (dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2])) < 0)
		{
			tbc.cannonMove = tankNet::CannonMovementOptions::RIGHT;
		}

		if (dot(perp(dir), Vector2(current->cannonForward[0], current->cannonForward[2])) > .01 && dis < 19)
		{
			turret = FIRE;
		}
	}
	//Fire the turret 
	void Fire()
	{
		tbc.cannonMove = tankNet::CannonMovementOptions::HALT;
		tbc.fireWish = 1;
		turret = SCAN;
	}
	//Wander around until we see an enemy
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
		if (current->tacticoolData[aimTarget].inSight)
		{
			tank = CHASE;
		}
		if (dot(cf, tf) > .87f) // if the dot product is close to lining up, move forward
			tbc.tankMove = tankNet::TankMovementOptions::FWRD;
		else // otherwise turn right until we line up!
			tbc.tankMove = tankNet::TankMovementOptions::RIGHT;
	}
	//Move toward the enemy
	void Chase()
	{
		Vector2 dir = normal(Vector2(current->position[0], current->position[2]) - Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		float dis = distance(Vector2(current->position[0], current->position[2]), Vector2(current->tacticoolData->lastKnownPosition[0], current->tacticoolData->lastKnownPosition[2]));
		Vector2 cp = Vector2::fromXZ(current->position); // current position
		Vector2 cf = Vector2::fromXZ(current->forward);  // current forward
		target = Vector2::fromXZ(current->tacticoolData[aimTarget].lastKnownPosition);

		// determine the forward to the target (which is the next waypoint in the path)
		Vector2 tf = normal(target - cp);

		if (dot(cf, tf) > .87f) // if the dot product is close to lining up, move forward
			tbc.tankMove = tankNet::TankMovementOptions::FWRD;
		else // otherwise turn right until we line up!
			tbc.tankMove = tankNet::TankMovementOptions::RIGHT;
		
	};
	//If we're stuck on something, get us out
	void Unstick()
	{
		stickTimer -= sfw::getDeltaTime();

		if (stickTimer > 0)
		{
			tbc.tankMove = tankNet::TankMovementOptions::LEFT;
		}
		else
		{
			stickTimer = 2;
			tank = WANDER;
		}
	}
public:
	tankNet::TankBattleCommand update(tankNet::TankBattleStateData *state)
	{
		


		//state machine
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
		case WANDER: Wander(); break;
		case UNSTICK: Unstick(); break;
		}
		////avoid shit
		//prevtbc = tbc;
		//Vector2 previousposition = Vector2(current->position[0], current->position[2]);
		//if (prevtbc.tankMove == tbc.tankMove)
		//{
		//	framecount++;
		//}
		//else
		//{
		//	framecount = 0;
		//}
		//if (framecount > 8 && distance(previousposition, Vector2(current->position[0], current->position[2])) < 4)
		//{
		//	tank = UNSTICK;
		//}
		return tbc;
		
	    
	}
};