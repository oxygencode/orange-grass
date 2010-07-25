/*
 *  IScreenController.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ISCREENCONTROLLER_H_
#define ISCREENCONTROLLER_H_

#include "IPlatform.h"


enum ControllerState
{
	CSTATE_NO,
	CSTATE_ACTIVE,
	CSTATE_INACTIVE
};

class IScreenController
{
public:
	virtual ~IScreenController() {}
	
	// Initialize controller
	virtual bool Init (IPlatform* _pPlatform) = 0;
	
	// Get controller state
	virtual ControllerState GetState () const = 0;
	
	// Update controller
	virtual void Update (unsigned int _ElapsedTime) = 0;
	
	// Render controller scene
	virtual void RenderScene () = 0;
	
	// Activate
	virtual void Activate () = 0;
	
	// deactivate
	virtual void Deactivate () = 0;
};

#endif