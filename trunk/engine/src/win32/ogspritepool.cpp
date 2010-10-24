/*
 *  ogspritepool.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "ogspritepool.h"
#include "ogsprite.h"
#include "ogguibutton.h"


COGSpritePool::COGSpritePool ()
{
}


COGSpritePool::~COGSpritePool ()
{
}


// add sprite.
void COGSpritePool::AddSprite (IOGSprite* _pSprite)
{
}


// remove sprite.
void COGSpritePool::RemoveSprite (IOGSprite* _pSprite)
{
	OG_SAFE_DELETE(_pSprite);
}


// create button.
IOGGuiButton* COGSpritePool::CreateButton ()
{
    return new COGGuiButton();
}


// desroy button.
void COGSpritePool::DestroyButton (IOGGuiButton* _pBtn)
{
    OG_SAFE_DELETE(_pBtn);
}
