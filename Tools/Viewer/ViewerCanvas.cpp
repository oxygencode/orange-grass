#include <Opengl2.h>
#include <wx/wx.h>
#include "ViewerCanvas.h"
#include <OrangeGrass.h>
#include <IOGGraphicsHelpers.h>
#include <IOGMath.h>


#define TIMER_ID    1000

BEGIN_EVENT_TABLE(CViewerCanvas, wxGLCanvas)
    EVT_TIMER(TIMER_ID, CViewerCanvas::OnTimer)
    EVT_SIZE(CViewerCanvas::OnSize)
    EVT_PAINT(CViewerCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(CViewerCanvas::OnEraseBackground)
    EVT_KEY_DOWN( CViewerCanvas::OnKeyDown )
    EVT_KEY_UP( CViewerCanvas::OnKeyUp )
	EVT_RESSWITCH( wxID_ANY, CViewerCanvas::OnResourceSwitch )
	EVT_TOOLCMD( wxID_ANY, CViewerCanvas::OnToolCmdEvent )
	EVT_ENTER_WINDOW( CViewerCanvas::OnMouseEnter )
	EVT_LEAVE_WINDOW( CViewerCanvas::OnMouseLeave )
	EVT_MOTION( CViewerCanvas::OnMouseMove )
	EVT_LEFT_DOWN( CViewerCanvas::OnLMBDown )
	EVT_LEFT_UP( CViewerCanvas::OnLMBUp )
	EVT_RIGHT_DOWN( CViewerCanvas::OnRMBDown )
	EVT_RIGHT_UP( CViewerCanvas::OnRMBUp )
END_EVENT_TABLE()


MATRIX	m_mProjection;
MATRIX	m_mView;
IOGModel*	m_pCurModel = NULL;
IOGMesh*	m_pCurMesh = NULL;
IOGTerrain*	m_pCurTerrain = NULL;
IOGSprite*	m_pCurSprite = NULL;

bool bRmb = false;
bool bLmb = false;
int  mouse_x = 0, mouse_y = 0;

/// @brief Constructor.
/// @param parent - parent window.
/// @param id - window ID.
/// @param pos - window position.
/// @param size - window size.
/// @param style - window style.
/// @param name - window name.
CViewerCanvas::CViewerCanvas (  wxWindow *parent, 
                                wxWindowID id,
                                const wxPoint& pos, 
                                const wxSize& size, 
                                long style, 
                                const wxString& name) : wxGLCanvas(parent, (wxGLCanvas*)NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name),
                                                        m_timer(this, TIMER_ID)
{
    m_init = false;
	m_bLoaded = false;
	GetEventHandlersTable()->AddEventHandler(EVENTID_RESSWITCH, this);
	GetEventHandlersTable()->AddEventHandler(EVENTID_TOOLCMD, this);
    m_timer.Start(100);

	m_bShowGrid = false;
	m_bShowAABB = false;
}


/// @brief Destructor.
CViewerCanvas::~CViewerCanvas()
{
}


/// @brief Render.
void CViewerCanvas::Render()
{
    wxPaintDC dc(this);
    dc.GetWindow()->GetHandle();

    if (!GetContext()) 
        return;

    SetCurrent();

    // Init OpenGL once, but after SetCurrent
    if (!m_init)
    {
        InitGL();
		GetResourceMgr()->Init("resources.xml");
		m_pLoadProgressDlg = new wxProgressDialog (wxT("Loading resources"), wxT("Loading..."), 100, this->GetParent());
		m_pLoadProgressDlg->Show();
		m_init = true;
    }

	if (!m_bLoaded)
	{
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFlush();
		SwapBuffers();
		LoadNextResource();
		this->Refresh();
		return;
	}

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_mProjection.f);
	
	glMatrixMode(GL_MODELVIEW);
	m_mView = GetCamera()->Update();
	glLoadMatrixf(m_mView.f);
	VECTOR4 vDiffuse;
	vDiffuse.x = 1.0f;
	vDiffuse.y = 1.0f;
	vDiffuse.z = 1.0f;
	vDiffuse.w = 1.0f;
	VECTOR4 vLightDirection;
	vLightDirection.x = 0.0f;
	vLightDirection.y = 1.0f;
	vLightDirection.z = 0.0f;
	vLightDirection.w = 0;
	glLightfv(GL_LIGHT0, GL_POSITION, (VERTTYPE*)&vLightDirection);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, (VERTTYPE*)&vDiffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, (VERTTYPE*)&vDiffuse);
	glEnable(GL_NORMALIZE);

	if (m_pCurModel)
		m_pCurModel->Render(m_mView);
	if (m_pCurTerrain)
		m_pCurTerrain->Render(m_mView);
	if (m_pCurMesh)
	{
		glDisable(GL_TEXTURE_2D);
		//glPolygonMode(GL_FRONT, GL_LINE);
		//glPolygonMode(GL_BACK, GL_LINE);
		m_pCurMesh->Render(m_mView);
		glEnable(GL_TEXTURE_2D);
		//glPolygonMode(GL_FRONT, GL_FILL);
		//glPolygonMode(GL_BACK, GL_FILL);
	}
	if (m_pCurSprite)
	{
		m_pCurSprite->Render();
	}

    RenderHelpers();

    glFlush();
    SwapBuffers();
}


/// @brief Timer handler.
/// @param event - event structute.
void CViewerCanvas::OnTimer(wxTimerEvent& event)
{
}


/// @brief Paint handler.
/// @param event - event structute.
void CViewerCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
}


/// @brief Resizing handler.
/// @param event - event structute.
void CViewerCanvas::OnSize(wxSizeEvent& event)
{
    wxGLCanvas::OnSize(event);
    GetClientSize(&m_ResX, &m_ResY);
    if (GetContext())
    {
        SetCurrent();
        glViewport(0, 0, m_ResX, m_ResY);
		MatrixPerspectiveFovRH(m_mProjection, 1.0f, float(m_ResX)/float(m_ResY), 4.0f, 2500.0f, true);
    }
}


/// @brief Initialize renderer.
void CViewerCanvas::InitGL()
{
	glewInit();

    SetupCamera ();

	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


/// @brief Load next resource bulk.
/// @return false if finished loading.
bool CViewerCanvas::LoadNextResource()
{
	if (GetResourceMgr()->LoadNext())
	{
		IOGResourceInfo resInfo;
		int progress = (int)(GetResourceMgr()->GetLoadProgress(resInfo));
		wxString msg(wxT("Loading: "));
		msg += resInfo.m_pResource;
		m_pLoadProgressDlg->Update (progress, msg);

		CommonToolEvent<ResLoadEventData> cmd(EVENTID_RESLOAD);
		cmd.SetEventCustomData(ResLoadEventData(wxT(resInfo.m_pResource), wxT(resInfo.m_pResourceGroup), wxT(resInfo.m_pResourceIcon)));
        GetEventHandlersTable()->FireEvent(EVENTID_RESLOAD, &cmd);
		return true;
	}
	m_bLoaded = true;
	return false;
}


/// @brief Key down handler.
/// @param event - event structute.
void CViewerCanvas::OnKeyDown( wxKeyEvent& event )
{
    switch (event.GetKeyCode())
    {
    case WXK_UP:
		if (m_pCurTerrain)
		{
			GetCamera()->Strafe(5.5f, Vec3(0, 0, -1));
			this->Refresh();
		}
        break;

	case WXK_DOWN:
		if (m_pCurTerrain)
		{
			GetCamera()->Strafe(5.5f, Vec3(0, 0, 1));
			this->Refresh();
		}
        break;

	case WXK_LEFT:
		if (m_pCurTerrain)
		{
			GetCamera()->Strafe(5.5f, Vec3(-1, 0, 0));
			this->Refresh();
		}
        break;

	case WXK_RIGHT:
		if (m_pCurTerrain)
		{
			GetCamera()->Strafe(5.5f, Vec3(1, 0, 0));
			this->Refresh();
		}
        break;
    }
    event.Skip();
}


/// @brief Key up handler.
/// @param event - event structute.
void CViewerCanvas::OnKeyUp( wxKeyEvent& event )
{
    event.Skip();
}


/// @brief Resource switching event handler
/// @param event - event structute.
void CViewerCanvas::OnResourceSwitch ( CommonToolEvent<ResSwitchEventData>& event )
{
	const ResSwitchEventData& evtData = event.GetEventCustomData();
	switch (evtData.m_ResourceType)
	{
	case RESTYPE_MODEL:
		m_pCurModel = GetResourceMgr()->GetModel(evtData.m_Resource);
		m_pCurTerrain = NULL;
		m_pCurMesh = NULL;
		delete m_pCurSprite; m_pCurSprite = NULL;
		break;

	case RESTYPE_MESH:
		m_pCurModel = NULL;
		m_pCurTerrain = NULL;
		m_pCurMesh = GetResourceMgr()->GetMesh(evtData.m_Resource);
		delete m_pCurSprite; m_pCurSprite = NULL;
		break;

	case RESTYPE_TEXTURE:
		{
			m_pCurModel = NULL;
			m_pCurTerrain = NULL;
			m_pCurMesh = NULL;
			if (m_pCurSprite) delete m_pCurSprite;
			m_pCurSprite = CreateSprite(evtData.m_Resource);
			int SprSize = (m_ResX < m_ResY) ? m_ResX : m_ResY;
			m_pCurSprite->SetPosition (0, 0, m_ResX, m_ResY);
		}
		break;
	}
    SetupCamera ();
	Refresh();
}


/// @brief Tool command event handler
/// @param event - event structute.
void CViewerCanvas::OnToolCmdEvent ( CommonToolEvent<ToolCmdEventData>& event )
{
	const ToolCmdEventData& evtData = event.GetEventCustomData();
	switch (evtData.m_CmdType)
	{
	case CMD_COORDGRID:
		m_bShowGrid = evtData.m_bSwitcher;
		break;

	case CMD_AABB:
		m_bShowAABB = evtData.m_bSwitcher;
		break;
	}
	Refresh ();
}


/// @brief Setup camera.
void CViewerCanvas::SetupCamera()
{
	float fDist = 600.0f;

	Vec3 vTarget (0, 0, 0);
	Vec3 vDir (0, 0.7f, 1.0f);
	vDir = vDir.normalize();
	Vec3 vUp = vDir.cross (Vec3(0, 1, 0));

	GetCamera()->Setup (vDir * fDist, vTarget, vUp);
}


/// @brief Render scene helpers.
void CViewerCanvas::RenderHelpers()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_mView.f);
    glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	if (m_bShowGrid)
		DrawCoordGrid (400, 10, 100);

	if (m_bShowAABB)
	{
		if (m_pCurModel)
		{
			DrawAABB (m_pCurModel->GetAABB());
		}
		if (m_pCurMesh)
		{
			DrawAABB (m_pCurMesh->GetAABB());
		}
	}

    glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}


/// @brief Mouse enter handler.
/// @param event - event structute.
void CViewerCanvas::OnMouseEnter(wxMouseEvent& event)
{
    SetFocus();
}


/// @brief Mouse leave handler.
/// @param event - event structute.
void CViewerCanvas::OnMouseLeave(wxMouseEvent& event)
{
}


/// @brief Mouse move handler.
/// @param event - event structute.
void CViewerCanvas::OnMouseMove(wxMouseEvent& event)
{
}


/// @brief Mouse Left button up handler.
/// @param event - event structute.
void CViewerCanvas::OnLMBUp(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bLmb = false;
}


/// @brief Mouse Left button up handler.
/// @param event - event structute.
void CViewerCanvas::OnLMBDown(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bLmb = true;
}


/// @brief Mouse Right button up handler.
/// @param event - event structute.
void CViewerCanvas::OnRMBUp(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bRmb = false;
}


/// @brief Mouse Right button up handler.
/// @param event - event structute.
void CViewerCanvas::OnRMBDown(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bRmb = true;
}