/*
 *  IOGActorParams.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IOGACTORPARAMS_H_
#define IOGACTORPARAMS_H_

#include "IOGPhysicalParams.h"
#include <string>


enum OGActorType
{
	OG_ACTOR_NONE = -1,
	OG_ACTOR_STATIC,
	OG_ACTOR_LANDBOT,
	OG_ACTOR_AIRBOT,
	OG_ACTOR_PLAYER,
	OG_ACTOR_PLASMAMISSILE,
	OG_ACTOR_MISSILE,
	OG_ACTOR_BONUS
};


enum OGTeam
{
	TEAM_NEUTRAL = 0,
	TEAM_PLAYER,
	TEAM_ENEMY
};


enum OGWeaponPos
{
	POS_CENTER	= 0,
	POS_LEFT,
	POS_RIGHT,
	POS_LEFTRIGHT,
	POS_CENTERLEFTRIGHT
};


struct IOGWeaponParams
{
	std::string		alias;
	unsigned int	hitpoints;
	unsigned int	cooldown;
	OGWeaponPos		pos;
	std::string		actor;
	std::string		icon_texture;
};


struct IOGGameplayParams
{
	IOGGameplayParams()
	{
		max_hitpoints = 0;
		hitpoints = max_hitpoints;
		team = TEAM_NEUTRAL;
	}

	unsigned int	max_hitpoints;
	unsigned int	hitpoints;
	OGTeam			team;
};


struct IOGActorParams
{
	IOGPhysicalParams	physics;
	IOGGameplayParams	gameplay;
	OGActorType			type;
	std::string			alias;
	std::string			model_alias;
	std::string			model_propeller_alias;
	std::string			model_destruction;
	std::string			icon;
};


// Parse the actor type string and convert it to internal type
inline OGActorType ParseActorType (const std::string& _ActorTypeStr)
{
	if (_ActorTypeStr.compare(std::string("static")) == 0)
    {
        return OG_ACTOR_STATIC;
    }
    else if (_ActorTypeStr.compare(std::string("land_bot")) == 0)
    {
        return OG_ACTOR_LANDBOT;
    }
    else if (_ActorTypeStr.compare(std::string("air_bot")) == 0)
    {
        return OG_ACTOR_AIRBOT;
    }
    else if (_ActorTypeStr.compare(std::string("player")) == 0)
    {
        return OG_ACTOR_PLAYER;
    }
    else if (_ActorTypeStr.compare(std::string("plasma_missile")) == 0)
    {
        return OG_ACTOR_PLASMAMISSILE;
    }
    else if (_ActorTypeStr.compare(std::string("missile")) == 0)
    {
        return OG_ACTOR_MISSILE;
    }
    else if (_ActorTypeStr.compare(std::string("bonus")) == 0)
    {
        return OG_ACTOR_BONUS;
    }
    return OG_ACTOR_NONE;
}


#endif