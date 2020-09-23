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
	

	// Инициализируем все улючи клавиотуры.
	for(i=0; i<256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
{
	// Если клавиша нажата то сохраняем ее состояние в массиве.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// Если клавиша отпущенна, то очищяем ее состояние в массиве.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// Возвращяем в каком состояниии находиться клавиша.
	return m_keys[key];
}