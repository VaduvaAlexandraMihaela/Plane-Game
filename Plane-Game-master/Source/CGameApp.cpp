//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <dos.h>
#include <fstream>

#define ENEMY true

extern HINSTANCE g_hInst;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_pPlayer2		= NULL;
	m_pEnemy		= NULL;
	m_pEnemy2		= NULL;
	
	m_LastFrameRate = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	HMONITOR hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi)) return NULL;

	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= mi.rcMonitor.right;
	USHORT			Height			= mi.rcMonitor.bottom;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	
	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				m_pPlayer->Explode();
				break;
			case 0X51:
				fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				m_pPlayer2->Explode();
				break;
			case VK_SPACE:
				m_pPlayer->Shoot(m_pBBuffer, countLeftRotation);
				break;
			case VK_X:
				m_pPlayer2->Shoot(m_pBBuffer, countLeftRotation2);
				break;
			case VK_O:
				{
					m_pPlayer->RotateLeft();

					if (countLeftRotation < 3)
						countLeftRotation++;
					else
						countLeftRotation = 0;
				}
				break;
			/*case 'P':
				{
					m_pPlayer->RotateRight();

					if (countRightRotation < 4)
						countRightRotation++;
					else
						countRightRotation = 0;
				}
				break;*/
			case 'E':
			{
				m_pPlayer2->RotateLeft();

			if (countLeftRotation2 < 3)
				countLeftRotation2++;
			else
				countLeftRotation2 = 0;
			}
				break;
			/*case 'R':
				{
					m_pPlayer2->RotateRight();

					if (countRightRotation2 < 4)
						countRightRotation2++;
					else
						countRightRotation2 = 0;
				}
				break;*/
			case '1':
				saveGame();
			case '2':
				loadGame();

			}
			break;


		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if(!m_pPlayer->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
			case 2:
				if (!m_pPlayer2->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
			}
			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer,0);
	m_pPlayer2 = new CPlayer(m_pBBuffer,800);
	m_pEnemy = new CEnemy(m_pBBuffer,1);
	m_pEnemy2 = new CEnemy(m_pBBuffer,2);

	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;
	
	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(700, 500);
	m_pPlayer2->Position() = Vec2(300, 400);
	m_pEnemy->Position() = Vec2(200, 300);
	m_pEnemy2->Position() = Vec2(200, 450);
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;

	}

	if (m_pPlayer2 != NULL)
	{
		delete m_pPlayer2;
		m_pPlayer2 = NULL;

	}

	if (m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;

	}

}
	

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 500 ];
	static TCHAR TitleBuffer[ 255 ];
	
	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s"), FrameRate );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	if (!m_bActive) return;

	if (!m_pPlayer->isAlive())
	{
		MessageBox(0, _T("Primul jucator a castigat!"), _T("End of game"), MB_OK);
		m_pBBuffer->~BackBuffer();
		ShutDown();
		return;
	}
	else if (!m_pPlayer2->isAlive())
	{
		MessageBox(0, _T("Al doilea jucator a castigat!"), _T("End of game"), MB_OK);
		m_pBBuffer->~BackBuffer();
		ShutDown();
		return;
	}

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	ULONG		Direction = 0;
	ULONG		Direction2 = 0;
	ULONG		Direction3 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if (!GetKeyboardState(pKeyBuffer)) return;

	// Check the relevant keys
	if (pKeyBuffer[VK_UP] & 0xF0) Direction |= CPlayer::DIR_FORWARD;
	if (pKeyBuffer[VK_DOWN] & 0xF0) Direction |= CPlayer::DIR_BACKWARD;
	if (pKeyBuffer[VK_LEFT] & 0xF0) Direction |= CPlayer::DIR_LEFT;
	if (pKeyBuffer[VK_RIGHT] & 0xF0) Direction |= CPlayer::DIR_RIGHT;

	if (pKeyBuffer[0x57] & 0xF0) Direction2 |= CPlayer::DIR_FORWARD;
	if (pKeyBuffer[0x53] & 0xF0) Direction2 |= CPlayer::DIR_BACKWARD;
	if (pKeyBuffer[0x41] & 0xF0) Direction2 |= CPlayer::DIR_LEFT;
	if (pKeyBuffer[0x44] & 0xF0) Direction2 |= CPlayer::DIR_RIGHT;

	static UCHAR pKeyBuffer2[256];
	if (!GetKeyboardState(pKeyBuffer2)) return;
		
	
	Direction3 |= CEnemy::DIR_LEFT;
	m_pEnemy->Move(Direction3);
	m_pEnemy2->Move2(Direction3);
	
		
	// Move the player
	m_pPlayer->Move(Direction);
	m_pPlayer2->Move(Direction2);
	

	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}


//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed(), this->m_nViewWidth, this->m_nViewHeight);
	m_pPlayer2->Update(m_Timer.GetTimeElapsed(), this->m_nViewWidth, this->m_nViewHeight);
	enemyShoot();
	m_pEnemy->Update(m_Timer.GetTimeElapsed());
	m_pEnemy2->Update(m_Timer.GetTimeElapsed());
	
	ManageCollisions();
}


void CGameApp::DrawBackground()
{
	static int currentY = m_imgBackground.Height();

	static size_t lastTime = ::GetTickCount();
	size_t currentTime = ::GetTickCount();

	if (currentTime - lastTime > 150)
	{
		lastTime = currentTime;
		currentY -= 20;
		if (currentY < 0)
			currentY = m_imgBackground.Height();
	}

	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, currentY);
}

void CGameApp::DrawObjects()
{
	
	static UINT			fTimer;

	m_pBBuffer->reset();
	
	DrawBackground();

	//m_imgBackground.Paint(m_pBBuffer->getDC(), 0, 0);

	m_pPlayer->Draw();

	m_pPlayer2->Draw();

	m_pEnemy->Draw();

	m_pEnemy2->Draw();

	m_pEnemy->fireEnemy(0, 1);

	m_pEnemy2->fireEnemy(0, -1);

	m_pBBuffer->present();

}

bool CGameApp::CheckCollision(CPlayer &obj1, CBullet &obj2)
{
	bool OutsideBottom = obj1.Position().y + obj1.Height() / 2 <
		obj2.Position().y - obj2.Height() / 2;//Rect1.Bottom < Rect2.Top

	bool OutsideTop = obj1.Position().y - obj1.Height() / 2 >
		obj2.Position().y + obj2.Height() / 2;//Rect1.Top > Rect2.Bottom

	bool OutsideLeft = obj1.Position().x - obj1.Width() / 2 >
		obj2.Position().x + obj2.Width() / 2;//Rect1.Left > Rect2.Right

	bool OutsideRight = obj1.Position().x + obj1.Width() / 2 <
		obj2.Position().x - obj2.Width() / 2;//Rect1.Right < Rect2.Left

	bool ok = !(OutsideBottom || OutsideTop || OutsideLeft || OutsideRight);
	if (ok)
	{
		return true;
	}
	return false;
}

bool CGameApp::CheckCollision2(CPlayer &obj1, CPlayer &obj2)
{
	bool OutsideBottom = obj1.Position().y + obj1.Height() / 2 <
		obj2.Position().y - obj2.Height() / 2;//Rect1.Bottom < Rect2.Top

	bool OutsideTop = obj1.Position().y - obj1.Height() / 2 >
		obj2.Position().y + obj2.Height() / 2;//Rect1.Top > Rect2.Bottom

	bool OutsideLeft = obj1.Position().x - obj1.Width() / 2 >
		obj2.Position().x + obj2.Width() / 2;//Rect1.Left > Rect2.Right

	bool OutsideRight = obj1.Position().x + obj1.Width() / 2 <
		obj2.Position().x - obj2.Width() / 2;//Rect1.Right < Rect2.Left

	bool ok = !(OutsideBottom || OutsideTop || OutsideLeft || OutsideRight);
	if (ok)
	{
		return true;
	}
	return false;
}

bool CGameApp::CheckCollision3(CEnemy &obj1, CBullet &obj2)
{
	bool OutsideBottom = obj1.Position().y + obj1.Height() / 2 <
		obj2.Position().y - obj2.Height() / 2;//Rect1.Bottom < Rect2.Top

	bool OutsideTop = obj1.Position().y - obj1.Height() / 2 >
		obj2.Position().y + obj2.Height() / 2;//Rect1.Top > Rect2.Bottom

	bool OutsideLeft = obj1.Position().x - obj1.Width() / 2 >
		obj2.Position().x + obj2.Width() / 2;//Rect1.Left > Rect2.Right

	bool OutsideRight = obj1.Position().x + obj1.Width() / 2 <
		obj2.Position().x - obj2.Width() / 2;//Rect1.Right < Rect2.Left

	bool ok = !(OutsideBottom || OutsideTop || OutsideLeft || OutsideRight);
	if (ok)
	{
		return true;
	}
	return false;
}

void CGameApp::ManageCollisions()
{
	static UINT	fTimer;

	//player-player collision

	if (CheckCollision2(*m_pPlayer, *m_pPlayer2) )
	{
		fTimer = SetTimer(m_hWnd, 1, 100, NULL);

		m_pPlayer->Explode();
		m_pPlayer2->Explode();
		m_pPlayer->Position() = Vec2(300, 900);
		m_pPlayer2->Position() = Vec2(100, 500);

	}

	//player-player`s bullets collision

	for (unsigned int i = 0; i < m_pPlayer->NoOfBullets(); i++)
	{
		if (CheckCollision(*m_pPlayer2, *m_pPlayer->getBullet(i)))
		{
			m_pPlayer->incrementScore(10);
			m_pPlayer->deleteBullet(i);
			fTimer = SetTimer(m_hWnd, 1, 100, NULL);
			m_pPlayer2->Explode();
			break;
		}
	}

	for (unsigned int j = 0; j < m_pPlayer2->NoOfBullets(); j++)
	{
		if (CheckCollision(*m_pPlayer, *m_pPlayer2->getBullet(j)))
		{
			m_pPlayer2->incrementScore(10);
			m_pPlayer2->deleteBullet(j);
			fTimer = SetTimer(m_hWnd, 1, 100, NULL);
			m_pPlayer->Explode();
			break;
		}
	}

	//player-enemy`s bullets collision

	for (unsigned int i = 0; i < m_pPlayer->NoOfBullets(); i++)
	{
		if (CheckCollision3(*m_pEnemy, *m_pPlayer->getBullet(i)))
		{
			m_pPlayer->incrementScore(500);
			m_pPlayer->deleteBullet(i);
			fTimer = SetTimer(m_hWnd, 1, 100, NULL);
			m_pEnemy->Position() = Vec2(2900, 2900);
			break;
		}
	}

	for (unsigned int i = 0; i < m_pPlayer->NoOfBullets(); i++)
	{
		if (CheckCollision3(*m_pEnemy2, *m_pPlayer->getBullet(i)))
		{
			m_pPlayer->incrementScore(500);
			m_pPlayer->deleteBullet(i);
			fTimer = SetTimer(m_hWnd, 1, 100, NULL);
			m_pEnemy2->Position() = Vec2(2900, 2900);
			break;
		}
	}

	for (unsigned int j = 0; j < m_pPlayer2->NoOfBullets(); j++)
	{
		if (CheckCollision3(*m_pEnemy, *m_pPlayer2->getBullet(j)))
		{
			m_pPlayer2->incrementScore(500);
			m_pPlayer2->deleteBullet(j);
			fTimer = SetTimer(m_hWnd, 1, 100, NULL);
			m_pEnemy->Position() = Vec2(2900, 2900);
			break;
		}
	}

	for (unsigned int j = 0; j < m_pPlayer2->NoOfBullets(); j++)
	{
		if (CheckCollision3(*m_pEnemy2, *m_pPlayer2->getBullet(j)))
		{
			m_pPlayer2->incrementScore(500);
			m_pPlayer2->deleteBullet(j);
			fTimer = SetTimer(m_hWnd, 1, 100, NULL);
			m_pEnemy2->Position() = Vec2(2900, 2900);
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------


void CGameApp::enemyShoot()
{
	if (m_pEnemy->fireCooldown < 30) {

		m_pEnemy->enemyShoot(-1);

		m_pEnemy->fireCooldown = 500 + rand() % 100;
	}

	if (m_pEnemy2->fireCooldown < 30) {

		m_pEnemy2->enemyShoot(1);

		m_pEnemy2->fireCooldown = 300 + rand() % 200;
	}

}


void CGameApp::saveGame()
{
	std::ofstream save("savegame/savegame.save");

	save << m_pPlayer->Position().x << " " << m_pPlayer->Position().y << " " << m_pPlayer->GetLives() << " ";
	save << m_pPlayer->getScore() << "\n";
	save << m_pPlayer2->Position().x << " " << m_pPlayer2->Position().y << " " << m_pPlayer2->GetLives() << " ";
	save << m_pPlayer2->getScore() << "\n";
	save << m_pEnemy->Position().x << " " << m_pEnemy->Position().y << "\n";
	save << m_pEnemy2->Position().x << " " << m_pEnemy2->Position().y << "\n";

	save.close();
}

void CGameApp::loadGame()
{
	std::ifstream save("savegame/savegame.save");

	double cdx, cdy;
	int livesP1, livesP2, score;

	save >> cdx >> cdy >> livesP1 >> score;
	m_pPlayer->Position() = Vec2(cdx, cdy);
	m_pPlayer->setScore(score);
	m_pPlayer->setLives(m_pBBuffer,livesP1);

	save >> cdx >> cdy >> livesP2 >> score;
	m_pPlayer2->Position() = Vec2(cdx, cdy);
	m_pPlayer2->setScore(score);
	m_pPlayer2->setLives(m_pBBuffer,livesP2);

	save >> cdx >> cdy;

	m_pEnemy->Position() = Vec2(cdx, cdy);

	save >> cdx >> cdy;
	
	m_pEnemy2->Position() = Vec2(cdx, cdy);


}