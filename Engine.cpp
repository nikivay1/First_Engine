// Engine.cpp : Определяет точку входа для приложения.

#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	// Создание объекта SystemClass.
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// Инициализируем и запускаем его.
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Завершение работы и освобождение системного объекта.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
};
