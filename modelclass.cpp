#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;


	// Инициализируем Вершинный буфер.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Загружаем текстуру для этой модели.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Очищяем текстуру.
	ReleaseTexture();

	// Очищяем буферы.
	ShutdownBuffers();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Устанавливаем уоличество вершин в массиве вершин.
	m_vertexCount = 3;

	// Устанавливаем количество индексов в массиве.
	m_indexCount = 3;

	// Создаем вершинный массив.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Создаем массив индексов.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Загружаем массив.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Нижний левый.
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Середина.
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);  // Нижний правый.
	vertices[2].texture = D3DXVECTOR2(1.0f, 0.0f);
	vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[3].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Нижний правый.
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// Загрузка массива индексов.
	indices[0] = 0;
	indices[1] = 1; 
	indices[2] = 3;
	indices[3] = 3;
	indices[4] = 1;
	indices[5] = 2;

	// Настройка описания статического вершинного буфера.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Создаем буффер вершин.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Указывае описание буфера индекса.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Создаем буффер индексов.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Очищяем массивы после использования.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Очищяем буферы индексов и вершин.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Шстанавливаем шаг буфера вершины и смещение.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Устанавливаем буфер вершин и индексов в активное состояние во входном устройстве
	// чтобы их можно было отрендерить
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Указываем тип примитива, который должен выводиться из этого буфера (сечас треугольники).
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Создаем объект текстуры.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void ModelClass::ReleaseTexture()
{
	// Очищяем объект текстуры.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}