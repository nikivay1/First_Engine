#include "inputclass.h"


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	int i;
	// »нициализируем все клавиши, чтобы они были отпущены и не нажаты. 
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	// ≈сли клавиша отпущена, возвращаем ее состо€ние в исходное.
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	// ¬озвращает состо€ние, в котором находитс€ клавиша(нажата / не нажата).
	return m_keys[key];
}