/*
 *  OGActorPlasmaMissile.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef OGACTORPLASMAMISSILE_H_
#define OGACTORPLASMAMISSILE_H_

#include "ogactorbullet.h"
#include "ogmissileflightworker.h"


class COGActorPlasmaMissile : public COGActorBullet
{
public:
	COGActorPlasmaMissile();
	virtual ~COGActorPlasmaMissile();

	// Create actor.
	virtual bool Create (
		IOGActorParams* _pParams,
		const OGVec3& _vPos,
		const OGVec3& _vRot,
        const OGVec3& _vScale);

	// Adding to actor manager event handler.
	virtual void OnAddedToManager ();

	// Set active state
	virtual void Activate (bool _bActive);

	// Fire.
	virtual void Fire ();

    // collision event handler
	virtual bool OnCollision (const IOGCollision& _Collision);

protected:

	// Update alive actor.
	virtual void UpdateAlive (unsigned long _ElapsedTime);

	// Update falling actor.
	virtual void UpdateFalling (unsigned long _ElapsedTime);

protected:

    IOGEffect*				m_pHeadEffect;
	COGMissileFlightWorker	m_FlightWorker;
};


#endif
