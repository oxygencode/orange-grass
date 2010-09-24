/*
 *  OGEffect.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef OGEFFECT_H_
#define OGEFFECT_H_

#include "IOGEffect.h"
#include "IOGTexture.h"
#include "IOGMaterial.h"


struct COGBillboard
{
	struct BBVert
	{
		Vec3 p;
		Vec2 t;
		Vec4 c;
	};

	float	scale;
	Vec3	offset;
	BBVert*	pVertices;
};


class COGEffect : public IOGEffect
{
public:
	COGEffect();
	virtual ~COGEffect();

	// Initialize effect.
	virtual void Init(OGEffectType _Type) = 0;

	// Set billboard basis vectors.
	virtual void SetBillboardVectors (const Vec3& _vUp, const Vec3& _vRight);

	// Set direction.
	virtual void SetDirection (const Vec3& _vDir);

	// Get direction.
	virtual const Vec3& GetDirection () const { return m_Direction; }

	// Update.
	virtual void Update (unsigned long _ElapsedTime) = 0;

	// Render.
	virtual void Render (const MATRIX& _mView) = 0;

	// Render.
	virtual void Render (const MATRIX& _mView, unsigned int _Part);

    // Get num renderable parts.
    virtual unsigned int GetNumRenderables () const {return 1;}

	// Get combined AABB
    virtual const IOGAabb& GetAABB () const {return m_AABB;}

	// Start.
	virtual void Start ();

	// Stop.
	virtual void Stop ();

	// Get effect run status.
	virtual OGEffectStatus GetStatus() const { return m_Status; }

	// Get effect type.
	virtual OGEffectType GetType() const { return m_Type; }

	// Get type of the renderable.
	virtual RenderableType GetRenderableType () const { return OG_RENDERABLE_EFFECT; }

protected:
	
    IOGAabb         m_AABB;
	Vec3			m_vCameraUp;
	Vec3			m_vCameraRight;
	Vec3			m_Direction;
	OGEffectStatus	m_Status;
	OGEffectType	m_Type;
	IOGTexture*		m_pTexture;
	IOGMaterial*	m_pMaterial;
};


#endif