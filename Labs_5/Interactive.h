#pragma once
#include <windows.h>
#include <dinput.h>

#pragma comment ( lib , "dinput8.lib" )
#pragma comment ( lib , "dxguid.lib" )


class Keyboard {
	
private:

	LPDIRECTINPUTDEVICE8 DIKeyboard;                                  // клавиатура
	char *Buffer;

public:

	Keyboard(void) { DIKeyboard = NULL; Buffer = NULL; }
	virtual bool InitKeyboard(LPDIRECTINPUT8 &, HWND);
	virtual bool GetKeyboardState(void);
	virtual bool GetButtonState(char);
	virtual void Release(void);
	virtual	~Keyboard() {}

};




