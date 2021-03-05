#pragma once

#include <Prelude.h>

#define MOUSE_LEFT 1//(bitnum 1)
#define MOUSE_MIDDLE 2//(bitnum 2)
#define MOUSE_RIGHT 4//(bitnum 3)
#define MOUSE_WHEELUP 8 //(bitnum 4)
#define MOUSE_WHEELDOWN 16 //(bitnum 5)

class Mouse {
private:
	bool GetBit(char bitnum, char data);

	bool bLeftClick;	bool bMiddleClick;	bool bRightClick;
	bool bWheelUp;		bool bWheelDown;

	bool bDrag;
	int  _x;			int  _y; // 현재 마우스 위치
	int  _deltax;		int  _deltay; // 마우스 이동 변위
	int  _clickx;		int  _clicky; // 마우스 드래그 시작 지점


public:
	Mouse();
	/////////////////////////////////////////////
	void PrintMouse();
	/////////////////////////////////////////////
	void SetDown(char key, bool isDown);
	void SetDrag(bool Drag);
	void SetMouseMove(int x, int y);//마우스 이동
	void SetMouseClick(int x, int y);
	void SetMouseWarp(int x, int y, bool ClickPosMove = false);//외부 API등으로 마우스가 순간이동한 경우
	/////////////////////////////////////////////
	bool isDown(bool ignoreOther, char key);
	bool isDrag();

	int GetX();
	int GetY();

	int GetDeltaX();
	int GetDeltaY();

	int GetClickX();
	int GetClickY();

	int GetDragX();
	int GetDragY();
};

//void UpdateMouseClick(Mouse& mouse, int button, int updown, int x, int y);


/////////////////////////////////////////////////////

Mouse::Mouse()
{
	bLeftClick = false;	bMiddleClick = false;	bRightClick = false;
	bWheelUp = false;	bWheelDown = false;

	bDrag = false;
	_x = 0;				_y = 0;
	_deltax = 0;		_deltay = 0;
	_clickx = 0;		_clicky = 0;
}


void Mouse::PrintMouse()
{
	std::cout << "bLeftClick \t bMiddleClick \t bRightClick" << std::endl;
	std::cout << bLeftClick << "\t" << bMiddleClick << "\t" << bRightClick << std::endl;
	std::cout << "_x \t _deltax \t _clickx" << std::endl;
	std::cout << _x << "\t" << _deltax << "\t" << _clickx << std::endl;
	std::cout << "_y \t _deltay \t _clicky" << std::endl;
	std::cout << _y << "\t" << _deltay << "\t" << _clicky << std::endl;
	std::cout << std::endl;
}

bool Mouse::GetBit(char bitnum, char data)
{
	if (bitnum > 8 || bitnum < 0){ return false; }

	for (char i = 1; i < bitnum; i++) { data /= 2; }
	data %= 2;
	return 1 == data;
}

void Mouse::SetDown(char key, bool isDown)
{
	if (GetBit(1, key)) { bLeftClick = isDown; }
	else if (GetBit(2, key)){ bMiddleClick = isDown; }
	else if (GetBit(3, key)){ bRightClick = isDown; }
	else if (GetBit(4, key)){ bWheelUp = isDown; }
	else if (GetBit(5, key)){ bWheelDown = isDown; }
}
void Mouse::SetDrag(bool Drag){ bDrag = Drag; }
void Mouse::SetMouseMove(int x, int y)
{
	_deltax = x - _x;	_deltay = y - _y;
	_x = x;				_y = y;
}
void Mouse::SetMouseClick(int x, int y) { _clickx = x;	_clicky = y; }
void Mouse::SetMouseWarp(int x, int y, bool ClickPosMove)
{
	if (ClickPosMove) { _clickx += x - _x; _clicky += y - _y; }
	_x = x;				_y = y;
}

bool Mouse::isDown(bool ignoreOther, char key)
{
	if (key < 0) { return false; }
	bool Down = true;
	//Left mouse button

	if (GetBit(1, key)) { Down &= bLeftClick; }
	else if (!ignoreOther){ Down &= !bLeftClick; }
	//Middle mouse button
	if (GetBit(2, key)) { Down &= bMiddleClick; }
	else if (!ignoreOther){ Down &= !bMiddleClick; }
	//Right mouse button
	if (GetBit(3, key)){ Down &= bRightClick; }
	else if (!ignoreOther){ Down &= !bRightClick; }
	//Wheel Up button
	if (GetBit(4, key)){ Down &= bWheelUp; }
	else if (!ignoreOther){ Down &= !bWheelUp; }
	//Wheel Down button
	if (GetBit(5, key)){ Down &= bWheelDown; }
	else if (!ignoreOther){ Down &= !bWheelDown; }
	return Down;
}
bool Mouse::isDrag() { return bDrag; }

int Mouse::GetX() { return _x; }
int Mouse::GetY() { return _y; }

int Mouse::GetDeltaX() { return _deltax; }
int Mouse::GetDeltaY() { return _deltay; }

int Mouse::GetClickX() { return _clickx; }
int Mouse::GetClickY() { return _clicky; }

int Mouse::GetDragX(){ return _x - _clickx; }
int Mouse::GetDragY(){ return _y - _clicky; }

