/*
 *  IOGAirPhysicalObject.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "orangegrass.h"
#include "OGAirPhysicalObject.h"


COGAirPhysicalObject::COGAirPhysicalObject () : COGPhysicalObject()
{
}


COGAirPhysicalObject::~COGAirPhysicalObject ()
{
}


// create object
void COGAirPhysicalObject::Create (const IOGAabb& _Aabb)
{
	m_Type = OG_PHYSICS_AIRBOT;
	m_Aabb = _Aabb;
    m_Obb.Create(m_Aabb);
}