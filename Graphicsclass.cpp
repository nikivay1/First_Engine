#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// —оздаем объект Direct3D.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// »нициализируем объект Direct3D.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// —оздаем объект камеры.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// ”станавливаем позицию камеры.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// —оздаем объект модели.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// »нициализируем объект модели.
	result = m_Model->Initialize(m_D3D->GetDevice(), L"../Engine/data/seafloor.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// —оздаем объект шейдера света.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// »нициализируем объект шейдера света.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// —оздаем объект освещени€.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// »нициализируем объект освещени€.
	m_Light->SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}


void GraphicsClass::Shutdown()
{
	// ќчищ€ем объект освещени€.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// ќчищ€ем объект шейдера света.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// ќчищ€ем объект модели.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// ќчищ€ем объект камеры.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// ќчищ€ем D3D объект.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0f;

	rotation += (float)D3DX_PI * 0.01f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// ќчищ€ем буферы перед отрисовкой сцены.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// √енерируем матрицу вида на основе приложени€.
	m_Camera->Render();

	// ѕолучите матрицы мира, вида и проекции с камеры и объектов d3d.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ѕоворачиваем мировую матрицу на величину поворота так, чтобы треугольник вращалс€.
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// ѕомещ€ем вершинные и индексные буферы модели на графический конвейер, чтобы подготовить их к рисованию.
	m_Model->Render(m_D3D->GetDeviceContext());

	// –ендерим модель использу€ шейдер света.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
								   m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}

	// ѕредоставл€ем отрендереную свену в передний буфер.
	m_D3D->EndScene();

	return true;
}