//-----------------------------------------------------------------------------
// File: CGameApp.h
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CGAMEAPP_H_

#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44
#define VK_W 0x57
#define VK_X 0x58
#define VK_Q 0x51
#define VK_O 0x4F

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "CTimer.h"
#include "CPlayer.h"
#include "CEnemy.h"
#include "BackBuffer.h"
#include "ImageFile.h"
#include "ResizeEngine.h"
#include <time.h>
#include <fstream>



//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp (Class)
// Desc : Central game engine, initialises the game and handles core processes.
//-----------------------------------------------------------------------------
class CGameApp
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CGameApp();
	virtual ~CGameApp();
	void		saveGame();
	void		loadGame();
	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	LRESULT	 DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	bool		InitInstance( LPCTSTR lpCmdLine, int iCmdShow );
	int		 BeginGame( );
	bool		ShutDown( );
	BackBuffer*				m_pBBuffer;
	HWND					m_hWnd;			 // Main window HWND
private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class
	//-------------------------------------------------------------------------
	bool		BuildObjects	  ( );
	void		ReleaseObjects	( );
	void		FrameAdvance	  ( );
	bool		CreateDisplay	 ( );
	void		ChangeDevice	  ( );
	void		SetupGameState	( );
	void		AnimateObjects	( );
	void		DrawObjects	   ( );
	void		ProcessInput	  ( );
	void		ManageCollisions();
	void		DrawBackground();
	
	bool		CheckCollision(CPlayer &obj1, CBullet &obj2);
	bool		CheckCollision2(CPlayer &obj1, CPlayer &obj2);
	bool		CheckCollision3(CEnemy &obj1, CBullet &obj2);
	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer				  m_Timer;			// Game timer
	ULONG				   m_LastFrameRate;	// Used for making sure we update only when fps changes.
	
	
	HICON				   m_hIcon;			// Window Icon
	HMENU				   m_hMenu;			// Window Menu
	
	bool					m_bActive;		  // Is the application active ?

	ULONG				   m_nViewX;		   // X Position of render viewport
	ULONG				   m_nViewY;		   // Y Position of render viewport
	ULONG				   m_nViewWidth;	   // Width of render viewport
	ULONG				   m_nViewHeight;	  // Height of render viewport

	POINT				   m_OldCursorPos;	 // Old cursor position for tracking
	HINSTANCE				m_hInstance;
	  
	CImageFile				m_imgBackground;

	int						nrBullets;
	int						counterBullets;
	
	int countLeftRotation;
	int countRightRotation;
	int countLeftRotation2;
	int countRightRotation2;

	CPlayer*				m_pPlayer;
	CPlayer*				m_pPlayer2;
	CEnemy*					m_pEnemy;
	CEnemy*					m_pEnemy2;
	void					enemyShoot();
};

#endif // _CGAMEAPP_H_