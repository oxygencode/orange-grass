/*
 *  IOGSpritePool.h
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 11.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IOGSPRITEPOOL_H_
#define IOGSPRITEPOOL_H_

#include "IOGSprite.h"


class IOGSpritePool
{
public:
	virtual ~IOGSpritePool () {}

	// add sprite.
	virtual void AddSprite (IOGSprite* _pSprite) = 0;

	// remove sprite.
	virtual void RemoveSprite (IOGSprite* _pSprite) = 0;
};

#endif
