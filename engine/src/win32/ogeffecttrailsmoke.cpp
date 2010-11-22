/*
 *  ogeffecttrailsmoke.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "ogeffecttrailsmoke.h"
#include "OrangeGrass.h"


float			COGEffectTrailSmoke::m_fAlphaFade;
float			COGEffectTrailSmoke::m_fInitialScale;
float			COGEffectTrailSmoke::m_fScaleInc;
unsigned int	COGEffectTrailSmoke::m_numVertsAtOnce;
float			COGEffectTrailSmoke::m_fRotateInc;
Vec4			COGEffectTrailSmoke::m_color;
std::string     COGEffectTrailSmoke::m_Texture;
unsigned int    COGEffectTrailSmoke::m_MappingId;


COGEffectTrailSmoke::~COGEffectTrailSmoke()
{
}



// Load params.
bool COGEffectTrailSmoke::LoadParams ()
{
	IOGSettingsReader* pReader = GetSettingsReader();

    IOGSettingsSource* pSource = pReader->OpenSource(GetResourceMgr()->GetFullPath("Effects/trailsmoke.xml"));
	if (!pSource)
		return false;

	IOGGroupNode* pRoot = pReader->OpenGroupNode(pSource, NULL, "Effect");
	if (pRoot)
	{
		m_fAlphaFade = pReader->ReadFloatParam(pRoot, "alpha_fade");
		m_fScaleInc = pReader->ReadFloatParam(pRoot, "scale_inc");
		m_fRotateInc = pReader->ReadFloatParam(pRoot, "rotate_inc");
		m_fInitialScale = pReader->ReadFloatParam(pRoot, "initial_scale");
		m_numVertsAtOnce = (unsigned int)pReader->ReadIntParam(pRoot, "particles_at_once");
		m_Texture = pReader->ReadStringParam(pRoot, "texture");
		m_MappingId = (unsigned int)pReader->ReadIntParam(pRoot, "mapping");
		m_color = pReader->ReadVec4Param(pRoot, "r", "g", "b", "a");
    	pReader->CloseGroupNode(pRoot);
	}
	pReader->CloseSource(pSource);
	return true;
}


// Initialize effect.
void COGEffectTrailSmoke::Init(OGEffectType _Type)
{
	m_pTexture = GetResourceMgr()->GetTexture(m_Texture);
	m_pMapping = m_pTexture->GetMapping(m_MappingId);
    m_Blend = OG_BLEND_ALPHABLEND;

	m_bPositionUpdated = false;
    m_BBList.reserve(60);
    m_AABB.SetMinMax(Vec3(-1,-1,-1), Vec3(1,1,1));
}


// Update.
void COGEffectTrailSmoke::Update (unsigned long _ElapsedTime)
{
	if (m_Status == OG_EFFECTSTATUS_INACTIVE)
		return;

    std::vector<ParticleFormat>::iterator iter = m_BBList.begin();
    while (iter != m_BBList.end())
    {
        ParticleFormat& particle = (*iter);
        if (particle.pVertices[0].c.w >= m_fAlphaFade)
        {
            particle.scale += m_fScaleInc;
            particle.angle += m_fRotateInc;
    		particle.pVertices[0].c.w -= m_fAlphaFade;
    		particle.pVertices[1].c.w -= m_fAlphaFade;
    		particle.pVertices[2].c.w -= m_fAlphaFade;
    		particle.pVertices[3].c.w -= m_fAlphaFade;
            ++iter;
        }
        else
        {
            iter = m_BBList.erase(iter);
            if (m_BBList.empty())
            {
                m_Status = OG_EFFECTSTATUS_INACTIVE;
                return;
            }
        }
    }

	if (m_Status == OG_EFFECTSTATUS_STARTED && m_bPositionUpdated && m_vCurPosition != m_vPrevPosition)
	{
		Vec3 vDir = m_vPrevPosition - m_vCurPosition;
		float fDist = vDir.length();
		vDir.normalize();

		for (unsigned int n = 0; n < m_numVertsAtOnce; ++n)
		{
			ParticleFormat particle;
			particle.offset = vDir * (fDist * (float)n);
			particle.scale = m_fInitialScale;
			particle.angle = rand() * 0.01f;
			particle.bDirty = true;
			particle.pVertices[0].c = m_color;
			particle.pVertices[1].c = m_color;
			particle.pVertices[2].c = m_color;
			particle.pVertices[3].c = m_color;
			m_BBList.push_back(particle);
		}
	}
}


// Update position.
void COGEffectTrailSmoke::UpdatePosition (const Vec3& _vPosition)
{
	if (m_Status == OG_EFFECTSTATUS_INACTIVE)
		return;

	if (!m_bPositionUpdated)
	{
		m_vCurPosition = _vPosition;
		m_vPrevPosition = _vPosition;
		m_bPositionUpdated = true;
	}
	else
	{
		m_vPrevPosition = m_vCurPosition;
		m_vCurPosition = _vPosition;
	}
}


// Render.
void COGEffectTrailSmoke::Render (const MATRIX& _mWorld)
{
	if (m_Status == OG_EFFECTSTATUS_INACTIVE)
		return;

    MATRIX mId; 
    MatrixIdentity(mId);
    m_pRenderer->SetModelMatrix(mId);
	m_pRenderer->SetBlend(m_Blend);
	m_pRenderer->SetTexture(m_pTexture);

    MATRIX mR;
	BBVert* pVert = NULL;
    std::vector<ParticleFormat>::iterator iter = m_BBList.begin();
    for (; iter != m_BBList.end(); ++iter)
    {
        ParticleFormat& particle = (*iter);
        if (particle.bDirty)
        {
            particle.offset += m_vCurPosition;
            particle.bDirty = false;
        }

        MatrixRotationAxis(mR, particle.angle, m_vCameraLook.x, m_vCameraLook.y, m_vCameraLook.z);

        Vec3 vSUp = m_vCameraUp * particle.scale;
		Vec3 vSRight = m_vCameraRight * particle.scale;

		pVert = &particle.pVertices[0];
        MatrixVecMultiply(pVert->p, vSRight + vSUp, mR);
		pVert->p += particle.offset;
        pVert->t.x = m_pMapping->t1.x; pVert->t.y = m_pMapping->t0.y;

		pVert = &particle.pVertices[1];
        MatrixVecMultiply(pVert->p, -vSRight + vSUp, mR);
		pVert->p += particle.offset;
        pVert->t.x = m_pMapping->t0.x; pVert->t.y = m_pMapping->t0.y;

		pVert = &particle.pVertices[2];
        MatrixVecMultiply(pVert->p, vSRight - vSUp, mR);
		pVert->p += particle.offset;
        pVert->t.x = m_pMapping->t1.x; pVert->t.y = m_pMapping->t1.y;

		pVert = &particle.pVertices[3];
        MatrixVecMultiply(pVert->p, -vSRight - vSUp, mR);
		pVert->p += particle.offset;
        pVert->t.x = m_pMapping->t0.x; pVert->t.y = m_pMapping->t1.y;

		m_pRenderer->DrawEffectBuffer(&particle.pVertices[0], 0, 4);
    }
}


// Start.
void COGEffectTrailSmoke::Start ()
{
	m_Status = OG_EFFECTSTATUS_STARTED;
    m_BBList.clear();
}


// Stop.
void COGEffectTrailSmoke::Stop ()
{
	if (m_Status == OG_EFFECTSTATUS_INACTIVE)
		return;

	m_Status = OG_EFFECTSTATUS_STOPPED;
}