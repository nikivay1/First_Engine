#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;
	
	
	// Создаем системный объект.
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// Инициализируем системный объект.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// Удаляем системный объект.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}