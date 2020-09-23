#include "textureshaderclass.h"


TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}


TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}


TextureShaderClass::~TextureShaderClass()
{
}


bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	WCHAR vs[] = L"../Engine/texturevs.hlsl";
	WCHAR ps[] = L"../Engine/textureps.hlsl";

	// Инициализируем вершинный и пиксельный шейдеры. 
	result = TextureShaderClass::InitializeShader(device, hwnd,vs, ps);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextureShaderClass::Shutdown()
{
	// Завершение работы вершинных и пиксельных шейдеров, а также связанных объектов. 
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	// Устанавливаем параметры шейдера, которые он будет использовать для рендеринга. 
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// Теперь визуализируем подготовленные буферы с помощью шейдера. 
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Инициализируем указатели, которые эта функция будет использовать, равными нулю. 
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Компилируем код вершинного шейдера. 
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// Если шейдер не смог скомпилировать, он должен был что-то записать в сообщение об ошибке. 
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// Если в сообщении об ошибке ничего не было, значит, он просто не смог найти сам файл шейдера. 
		else
		{
			MessageBox(hwnd, vsFilename, L"Отсутствует файл вершинного шейдера.", MB_OK);
		}

		return false;
	}

	// Компилируем код пиксельного шейдера.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//  Если шейдер не смог скомпилировать, он должен был что-то записать в сообщение об ошибке. 
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//Если в сообщении об ошибке ничего не было, значит, он просто не смог найти сам файл. 
		else
		{
			MessageBox(hwnd, psFilename, L"Отсутствует файл пиксельного шейдера.", MB_OK);
		}

		return false;
	}

	// Создаем вертексный шейер из буфера.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Создаем пиксельный шейдер из буфера.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Теперь настраиваем макет данных, которые поступают в шейдер. 
	// ** Эта настройка должна соответствовать структуре VertexType в ModelClass и шейдере. 
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Получаем количество элементов в макете. 
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Создаем макет ввода вершины. 
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Освобождаем буфер вершинного шейдера и буфер пиксельного шейдера, поскольку они больше не нужны. 
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Устанавливаем описание буфера констант динамической матрицы, который находится в вершинном шейдере. 
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Создаем указатель константного буфера, чтобы мы могли получить доступ к константному буферу вершинного шейдера из этого класса. 
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Создаем описание состояния сэмплера текстуры. 
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	// Очистка состояния семплера.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Очистка константного буфера.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Отчистка пиксельного шейдера
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Отчистка шейдера вершин
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* ShaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Получаем указатель на текстовый буфер сообщения об ошибке. 
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Получаем длину сообщения. 
	bufferSize = errorMessage->GetBufferSize();

	// Открываем файл, в который нужно записать сообщение об ошибке. 
	fout.open("shader-error.txt");
	
	// Записываем сообщение об ошибке. 
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Закройте файл. 
	fout.close();


	// Отпускаем сообщение об ошибке.
	errorMessage->Release();
	errorMessage = 0;

	// Отображает сообщение на экране, чтобы уведомить пользователя о необходимости проверить текстовый файл на наличие ошибок компиляции. 
	MessageBox(hwnd, L"Ошибка компиляции шейдера.Проверьте shader-error.txt на наличие сообщения.", ShaderFilename, MB_OK);

	return;
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Транспонируем матрицы, чтобы подготовить их для шейдера. 
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Блокируем буфер констант, чтобы в него можно было записать. 
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Получаем указатель на данные в постоянном буфере. 
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Копируем матрицы в постоянный буфер. 
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Разблокировать постоянный буфер. 
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Устанавливаем положение буфера констант в вершинном шейдере. 
	bufferNumber = 0;

	// Устанавливаем буфер констант в вершинном шейдере с обновленными значениями. 
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Устанавливаем ресурс текстуры шейдера в пиксельном шейдере. 
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Устанавливаем макет ввода вершины. 
	deviceContext->IASetInputLayout(m_layout);

	// Устанавливаем вершинные и пиксельные шейдеры, которые будут использоваться для визуализации этого треугольника. 
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	
	// Устанавливаем состояние семплера в пиксельном шейдере.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Отрисовываем треугольник. 
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}