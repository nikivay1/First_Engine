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


	// Обнуляем ширину и высоту экрана перед отправкой переменных в функцию. 
	screenWidth = 0;
	screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	// Создаем объект для чтения ввода с клавиатуры.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Инициализируем вхродной объект.
	m_Input->Initialize();

	// Создаем графический объект. Этот объект будет обрабатывать всю графику для этого приложения. 
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// Инициализируем графический объект.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------
// Функция выполняет чистку и закрывает окно.
//--------------------------------------------------------------------------------------

void SystemClass::Shutdown()
{
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	ShutdownWindows();
	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		// Приверка сообщений windows
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//Если windows сигнализирует о закрытии приложения, то выходим.
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			// Выполняем отрисовку кадра
			result = Frame();
			if (!result) 
			{
				done = true;
			}
		}
	}
}

//--------------------------------------------------------------------------------------
// Основная функция приложения в которой выполняется вся обработка.
//--------------------------------------------------------------------------------------

bool SystemClass::Frame() {
	bool result;
	// Проверяем нажал ли пользователь кнопку выхода
	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		return false;
	}
	// Выполняем обработку кадра для гораф объекта
	result = m_Graphics->Frame();
	if (!result) 
	{
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------
// В функцию направляются все сообщения Windows, 
// для прослушки интересующеей информации.
//--------------------------------------------------------------------------------------

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Проверяем, была ли нажата клавиша на клавиатуре. 
		case WM_KEYDOWN:
		{
			// Если клавиша нажата, отправьте ее объекту ввода, чтобы он мог записать это состояние. 
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Проверяем, была ли отпущена клавиша на клавиатуре. 
		case WM_KEYUP:
		{
			// Если клавиша отпущена, отправьте ее объекту ввода, чтобы он мог сбросить состояние для этой клавиши. 
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Любые другие сообщения отправляются обработчику сообщений по умолчанию, поскольку наше приложение не будет их использовать. 
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

//--------------------------------------------------------------------------------------
// Функция для создания окна для рендеринга
//--------------------------------------------------------------------------------------

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;
	m_hinstance = GetModuleHandle(NULL);
	m_applicationName = L"Engine";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Зарегистрируем класс окна. 
	RegisterClassEx(&wc);

	// Определяем разрешение экрана рабочего стола клиента. 
	screenWidth = GetSystemMetrics (SM_CXSCREEN); 
	screenHeight = GetSystemMetrics (SM_CYSCREEN); 

	// Устанавливаем параметры экрана в зависимости от того, работает он в полноэкранном или оконном режиме. 
	if (FULL_SCREEN) 
	{ 
		// Если полноэкранный режим, установите максимальный размер экрана для рабочего стола пользователя и 32 бита.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Измените настройки отображения на полный экран. 
		ChangeDisplaySettings (& dmScreenSettings, CDS_FULLSCREEN); 

		// Устанавливаем положение окна в левый верхний угол. 
		posX = posY = 0; 
	} 
	else 
	{ 
		// Если в окне, то установите разрешение 800x600. 
		screenWidth = 800;
		screenHeight = 600; 

		// Поместите окно в середину экрана. 
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	} 

	// Создаем окно с настройками экрана и получаем его дескриптор. 
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Вывести окно на экран и сделать его основным. 
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Скрываем курсор мыши.
	ShowCursor(false);

	return;
}

//--------------------------------------------------------------------------------------
// Функция возвращает настройки экрана в нормальное состояние 
// и освобождает окно и связанные с ним дескрипторы.
//--------------------------------------------------------------------------------------

void SystemClass::ShutdownWindows()
{
	// Показать курсор мыши. 
	ShowCursor(true);

	// Исправляем настройки отображения при выходе из полноэкранного режима. 
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Удаляем окно. 
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Удаляем экземпляр приложения. 
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Отпускаем указатель на этот класс. 
	ApplicationHandle = NULL;

	return;
}

//--------------------------------------------------------------------------------------
// Сюда поступают все сообщения от окон.
//--------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}