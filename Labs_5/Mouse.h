#pragma once
#include <dinput.h>

#pragma comment ( lib , "dinput8.lib" )
#pragma comment ( lib , "dxguid.lib" )

#define			DI_X		0
#define			DI_Y		1
#define			DI_Z		2

DIMOUSESTATE2	STD_DIMOUSEINPUT2;


class Mouse {

private:

	LPDIRECTINPUTDEVICE8 DIMouse;

public:

	bool InitMouse(LPDIRECTINPUT8 &, HWND);
	void GetMouseState(void);
	bool GetButtonState(int);
	int	GetOffset(int);
	void Release(void);
	~Mouse() {}
};

void Mouse::GetMouseState(void) {

	if (!DIMouse) {

		MessageBox(0, L"Мышь не инициализирована.", L"Ошибка", 0);
		exit(0);
	}

	ZeroMemory(&STD_DIMOUSEINPUT2, sizeof(STD_DIMOUSEINPUT2));

	if (DIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &STD_DIMOUSEINPUT2) != DI_OK) {

		MessageBox(0, L"Ошибка чтения состояния мыши.", L"Ошибка", 0);
		exit(0);
	}
}

bool Mouse::GetButtonState(int mb) {

	if (!DIMouse) {

		MessageBox(0, L"Мышь не инициализирована.", L"Ошибка", 0);
		exit(0);
	}

	return (STD_DIMOUSEINPUT2.rgbButtons[mb] & 0x80);
}

int Mouse::GetOffset(int _MODE) {

	if (!DIMouse) {

		MessageBox(0, L"Мышь не инициализирована.", L"Ошибка", 0);
		exit(0);
	}

	switch (_MODE) {

	case DI_X: return STD_DIMOUSEINPUT2.lX; break;
	case DI_Y: return STD_DIMOUSEINPUT2.lY; break;
	case DI_Z: return STD_DIMOUSEINPUT2.lZ; break;
	}

	MessageBox(0, L"Mouse::GetOffset(...) : кривое значение _MODE.", L"Ошибка", 0);
	return 0;
}

bool Mouse::InitMouse(LPDIRECTINPUT8 &DIObject, HWND hWnd) {

	//создание девайса
	if (DIObject->CreateDevice(GUID_SysMouse, &DIMouse, NULL) != DI_OK) {

		MessageBox(0, L"Ошибка создания девайса для мыши.", L"Ошибка", 0);
		return false;
	}

	if (DIMouse->SetDataFormat(&c_dfDIMouse2) != DI_OK) {

		MessageBox(0, L"Ошибка установки формата мыши.", L"Ошибка", 0);
		return false;
	}

	//установка режима работы мыши
	if (DIMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK) {

		MessageBox(0, L"Ошибка установки Cooperative Level'а для мыши.", L"Ошибка", 0);
		return false;
	}

	//получаем доступ к мыше
	if (DIMouse->Acquire() != DI_OK) {

		MessageBox(0, L"Ошибка получения доступа к мыши.", L"Ошибка.", 0);
		return false;
	}

	return true;
}


void Mouse::Release(void) {

	if (DIMouse) { DIMouse->Release(); }
}