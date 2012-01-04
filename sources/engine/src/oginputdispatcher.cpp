/*
 *  ogactorplayer.cpp
 *  OrangeGrass
 *
 *  Created by Viacheslav Bogdanov on 12.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "OrangeGrass.h"
#include "oginputdispatcher.h"
#include <algorithm>


COGInputDispatcher::COGInputDispatcher() :  m_fPrevX(0),
                                            m_fPrevY(0),
                                            m_bInMove(false)
{
}


COGInputDispatcher::~COGInputDispatcher()
{
	m_Receivers.clear();
}


// Register input receiver.
void COGInputDispatcher::RegisterReceiver (IOGInputReceiver* _pReceiver)
{
	std::vector<IOGInputReceiver*>::iterator iter = std::find(m_Receivers.begin(), m_Receivers.end(), _pReceiver);
	if (iter == m_Receivers.end())
	{
		m_Receivers.push_back(_pReceiver);
	}
}


// Unregister input receiver.
void COGInputDispatcher::UnregisterReceiver (IOGInputReceiver* _pReceiver)
{
	std::vector<IOGInputReceiver*>::iterator iter = std::find(m_Receivers.begin(), m_Receivers.end(), _pReceiver);
	if (iter != m_Receivers.end())
	{
		m_Receivers.erase(iter);
	}
}


// Pointing device press handler.
void COGInputDispatcher::OnPointerDown ( int _X, int _Y )
{
    //m_bInMove = true;
    //m_fPrevX = (float)_X;
    //m_fPrevY = (float)_Y;

    std::vector<IOGInputReceiver*>::iterator iter = m_Receivers.begin();
    for (; iter != m_Receivers.end(); ++iter)
    {
		if ((*iter)->OnTouch(OGVec2((float)_X, (float)_Y), OG_TOUCH_DOWN))
        {
            break;
        }
	}
}


// Pointing device release handler.
void COGInputDispatcher::OnPointerUp ( int _X, int _Y )
{
    //m_bInMove = false;
    //m_fPrevX = (float)_X;
    //m_fPrevY = (float)_Y;

    std::vector<IOGInputReceiver*>::iterator iter = m_Receivers.begin();
    for (; iter != m_Receivers.end(); ++iter)
    {
		if ((*iter)->OnTouch(OGVec2((float)_X, (float)_Y), OG_TOUCH_UP))
        {
            break;
        }
	}
}


// Pointing device move handler.
void COGInputDispatcher::OnPointerMove ( int _X, int _Y )
{
    if (m_bInMove)
    {
        OGVec3 vVec((float)_X - m_fPrevX, 0, (float)_Y - m_fPrevY);
        if (vVec.length() > 0.01f)
        {
            std::vector<IOGInputReceiver*>::iterator iter = m_Receivers.begin();
            for (; iter != m_Receivers.end(); ++iter)
            {
                if ((*iter)->OnVectorChanged(vVec))
                {
                    break;
                }
            }
        }

        m_fPrevX = (float)_X;
        m_fPrevY = (float)_Y;
    }
    else
    {
        m_fPrevX = (float)_X;
        m_fPrevY = (float)_Y;
        m_bInMove = true;
    }
}


// Accelerometer handler.
void COGInputDispatcher::OnAccelerometer ( float _X, float _Y, float _Z )
{
    std::vector<IOGInputReceiver*>::iterator iter = m_Receivers.begin();
    for (; iter != m_Receivers.end(); ++iter)
    {
        if ((*iter)->OnVectorChanged(OGVec3(_X, _Y, _Z)))
        {
            break;
        }
    }
}
