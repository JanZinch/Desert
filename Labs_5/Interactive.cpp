#include "interactive.h"


bool Keyboard::InitKeyboard(LPDIRECTINPUT8 &DIObject, HWND hWnd) {
	
	Buffer = new char[256];

	if (DIObject->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL) != DI_OK) {

		MessageBox(0, L"Ошибка создания девайса для клавиатуры.", L"Ошибка", 0);
		return false;
	}

	if (DIKeyboard->SetDataFormat(&c_dfDIKeyboard) != DI_OK) {

		MessageBox(0, L"Ошибка установки формата клавиатуры.", L"Ошибка", 0);
		return false;
	}

	if (DIKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK) {

		MessageBox(0, L"Ошибка установки Cooperative Level'а для клавиатуры.", L"Ошибка", 0);
		return false;
	}

	if (DIKeyboard->Acquire() != DI_OK) {

		MessageBox(0, L"Ошибка получения доступа к клавиатуры.", L"Ошибка.", 0);
		return false;
	}

	return true;
}

bool Keyboard::GetKeyboardState(void) {
	
	if (!Buffer) {
		
		MessageBox(0, L"Буффер клавиатуры не инициализирован.", L"Ошибка", 0);
		return false;
	}
	if (DIKeyboard->GetDeviceState(256, (LPVOID)Buffer) != DI_OK) {

		MessageBox(0, L"Ошибка получения состояния клавиатуры.", L"Ошибка", 0);
		return false;
	}
	return true;
}

bool Keyboard::GetButtonState(char Button) {
	
	if (!Buffer) {
		
		MessageBox(0, L"Буффер клавиатуры не инициализирован.", L"Ошибка", 0);
		return false;
	}
	return (Buffer[Button] & 0x80);
}

void Keyboard::Release(void) {
	
	if (DIKeyboard) {

		DIKeyboard->Release();
	}
	if (Buffer) {

		delete[] Buffer;
	}
}






