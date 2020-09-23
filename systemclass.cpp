#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Инициализируем высоту и ширину экрана равными нулю.
	screenWidth = 0;
	screenHeight = 0;

	// Инициализируем WindowsAPI.
	InitializeWindows(screenWidth, screenHeight);

	// Создаем входной объект.Этот объект будет использоваться для обработки и чтения ввода с клавиатуры от пользователя.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Инициализируем объект ввода.
	m_Input->Initialize();

	// Создаем графический объект. Этот объект будет обрабатывать всю графику для этого приложения.
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	// Инициализируем графический объект.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}
	
	return true;
}


void SystemClass::Shutdown()
{
	// Очищяем графический объект.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Очищяем объект ввода.
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Закрываем окно.
	ShutdownWindows();
	
	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Инициализируем структуру сообщения.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Цикл, пока не появится сообщение о выходе из окна или пользователя.
	done = false;
	while(!done)
	{
		// Обработка сообщений от Windows.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Если Windows сигнализирует о завершении приложения, выйдите из него.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Иначе деляем обработку кадра.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}


bool SystemClass::Frame()
{
	bool result;


	// 	Проверяем, нажал ли пользователь escape и хочет ли выйти из приложения.
	if(m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Выполняем обработку кадра графического класса.
	result = m_Graphics->Frame();
	if(!result)
	{
		return false;
	}

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Проверяем, была ли нажата клавиша на клавиатуре.
		case WM_KEYDOWN:
		{
			// Если клавиша нажата, отправляем ее объекту ввода, чтобы он мог записать это состояние.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Проверьте, была ли отпущена клавиша на клавиатуре.
		case WM_KEYUP:
		{
			// Если клавиша отпущена, отправляем ее объекту ввода, чтобы он мог сбросить состояние для этой клавиши..
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Любые другие сообщения, отправляемые обработчику сообщений по умолчанию, поскольку приложение их не использует.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Получаем указатель на этот объект.	
	ApplicationHandle = this;

	// Получаем экземпляр этого объекта.
	m_hinstance = GetModuleHandle(NULL);

	// Задаем имя приложению.
	m_applicationName = L"Engine";

	// Устанавливаем окно с настройками по умолчанию.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Регистрируем класс окна.
	RegisterClassEx(&wc);

	// Определяем размеры экрана (высоту и ширину).
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Устанавливаем настройки экрана в зависимости от того, работает ли он в полноэкранном или оконном режиме.
	if(FULL_SCREEN)
	{
		// Если в полноэкранном режиме установите максимальный размер экрана для рабочего стола пользователя и 32 - битный.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Меняем настройки отображения на полный экран.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Устанавливаем начальную позицию экрана в 0.
		posX = posY = 0;
	}
	else
	{
		// Если в окне  то устанавливаем дефолтные размеры.
		screenWidth  = 800;
		screenHeight = 600;

		// Ставим окно в центр экрана.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Создаем окно с выставленными настройками экрана и получаем его дискриптор.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Поднимаем его на верх экрана и устанавливаем фокус.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Скрываем курсор на экране.
	ShowCursor(false);

	return;
}


void SystemClass::ShutdownWindows()
{
	// Показываем курсор.
	ShowCursor(true);

	// Исправляем настройки дисплея при выходе из полноэкранного режима.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Убираем окно.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Удаляем экземпляр приложения.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Очищаем указатель на этот класс.
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Проверяем разрушилось ли окно.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Проверяем закрывется ли оно.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// Остальные сообщения передаем дальше системному классу.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}