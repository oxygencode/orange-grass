/*
 *  ogactortargetedmissile.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "ogactortargetedmissile.h"


COGActorTargetedMissile::COGActorTargetedMissile()
{
    m_pHeadEffect = NULL;
}


COGActorTargetedMissile::~COGActorTargetedMissile()
{
}


// Create actor.
bool COGActorTargetedMissile::Create (IOGActorParams* _pParams,
                                      const Vec3& _vPos,
                                      const Vec3& _vRot,
                                      const Vec3& _vScale)
{
	m_pParams = _pParams;

	if (m_pParams->type == OG_ACTOR_NONE)
	{
		OG_LOG_ERROR("Creating COGActorTargetedMissile from model %s failed, actor type is OG_ACTOR_NONE", m_pParams->model_alias.c_str());
		return false;
	}

	m_pHeadEffect = GetEffectsManager()->CreateEffect(OG_EFFECT_MISSILESMOKE);
	if (!m_pHeadEffect)
	{
		OG_LOG_ERROR("Creating COGActorTargetedMissile failed, cannot get effect %s", m_pParams->model_alias.c_str());
		return false;
	}
	
    m_pPhysicalObject = m_pPhysics->CreateObject(&m_pParams->physics, m_pHeadEffect->GetAABB(), this);
    if (!m_pPhysicalObject)
	{
		OG_LOG_ERROR("Creating COGActorTargetedMissile failed, cannot create physical object");
        return false;
	}
	m_pPhysicalObject->SetWorldTransform(_vPos, _vRot, _vScale);

	m_pNode = m_pSg->CreateNode(m_pHeadEffect, m_pPhysicalObject);
	if (!m_pNode)
	{
		OG_LOG_ERROR("Creating COGActorTargetedMissile failed, cannot create SG node");
		return false;
	}

    m_pCollisionEffect = GetEffectsManager()->CreateEffect(OG_EFFECT_COLLISION);
	m_pCollisionEffectNode = m_pSg->CreateNode(m_pCollisionEffect, m_pPhysicalObject);

	m_FlightWorker.Create(this);
    Activate(false);

	return true;
}


// Adding to actor manager event handler.
void COGActorTargetedMissile::OnAddedToManager ()
{
    COGActor::OnAddedToManager();

	m_pSg->AddEffectNode(m_pNode);
    m_pPhysicalObject->AddCollisionListener(this);

    m_pSg->AddEffectNode(m_pCollisionEffectNode);
    m_pCollisionEffectNode->Activate(false);
}


// Update alive actor.
void COGActorTargetedMissile::UpdateAlive (unsigned long _ElapsedTime)
{
	if (m_FlightWorker.IsActive())
	{
        m_pHeadEffect->UpdatePosition(m_pPhysicalObject->GetPosition());
		m_FlightWorker.Update(_ElapsedTime);
		if (m_FlightWorker.IsFinished())
		{
			m_pPhysicalObject->Activate(false);
			m_pHeadEffect->Stop();
			m_Status = OG_ACTORSTATUS_FALLING;
		}
	}
	else
	{
		m_pPhysicalObject->Activate(false);
		m_pHeadEffect->Stop();
		m_Status = OG_ACTORSTATUS_FALLING;
	}
}


// Update falling actor.
void COGActorTargetedMissile::UpdateFalling (unsigned long _ElapsedTime)
{
	if (m_pHeadEffect->GetStatus() == OG_EFFECTSTATUS_INACTIVE && 
        m_pCollisionEffect->GetStatus() == OG_EFFECTSTATUS_INACTIVE)
	{
		Activate(false);
	}
}


// Set active state
void COGActorTargetedMissile::Activate (bool _bActive)
{
    if (_bActive == m_bActive)
        return;

	COGActorBullet::Activate(_bActive);

	if (m_bActive)
	{
		m_Status = OG_ACTORSTATUS_ALIVE;
		m_FlightWorker.Reset();
		m_FlightWorker.Activate(true);
		m_pHeadEffect->Start();
		m_pHeadEffect->SetDirection(Vec3(0,0,1));
        m_pPhysicalObject->Update(1);
	}
	else
	{
        m_pCollisionEffectNode->Activate(false);
	}
}


// Fire.
void COGActorTargetedMissile::Fire ()
{
	COGActorBullet::Fire();

    Vec3 vRot = m_pOwner->GetPhysicalObject()->GetRotation();
	m_pPhysicalObject->SetWorldTransform(m_vLaunchOffset, vRot, Vec3(1,1,1));
    Activate(true);
}


// collision event handler
bool COGActorTargetedMissile::OnCollision (const IOGCollision& _Collision)
{
	if (!m_bActive || m_Status != OG_ACTORSTATUS_ALIVE)
		return false;

	m_pPhysicalObject->Activate(false);
	m_FlightWorker.Activate(false);
	m_Status = OG_ACTORSTATUS_FALLING;
	m_pHeadEffect->Stop();

    m_pCollisionEffectNode->Activate(true);
	m_pCollisionEffect->SetDirection(Vec3(0,0,1));
    m_pCollisionEffect->Start();

    return true;
}