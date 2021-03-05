// GUI Input. 마우스, 키보드 입력을 총괄해서 처리하는 함수

#pragma once

#include <Prelude.h>
#include _CH_MOUSE_
#include _CH_GLUT_

//extern Mouse mouse;
Mouse mouse;

void UpdateMouseDrag(int x, int y);
void UpdateMousePassiveDrag(int x, int y);
void UpdateMouseClick(int button, int updown, int x, int y);


void UpdateMouseDrag(int x, int y)
{
	mouse.SetDrag(true);
	mouse.SetMouseMove(glutGet(GLUT_WINDOW_X) + x, glutGet(GLUT_WINDOW_Y) + y);
}
void UpdateMousePassiveDrag(int x, int y)
{
	mouse.SetDrag(false);
	mouse.SetMouseMove(glutGet(GLUT_WINDOW_X) + x, glutGet(GLUT_WINDOW_Y) + y);
}
void UpdateMouseClick(int button, int updown, int x, int y)
{
	if (!(mouse.isDrag())){ mouse.SetMouseClick(glutGet(GLUT_WINDOW_X) + x, glutGet(GLUT_WINDOW_Y) + y); }
	mouse.SetMouseMove(glutGet(GLUT_WINDOW_X) + x, glutGet(GLUT_WINDOW_Y) + y);
	if (button == GLUT_LEFT_BUTTON) { mouse.SetDown(MOUSE_LEFT, updown == GLUT_DOWN); }
	if (button == GLUT_MIDDLE_BUTTON) { mouse.SetDown(MOUSE_MIDDLE, updown == GLUT_DOWN); }
	if (button == GLUT_RIGHT_BUTTON) { mouse.SetDown(MOUSE_RIGHT, updown == GLUT_DOWN); }
}

/*
static void keyboard(unsigned char key, int x, int y)
{
std::cout << (key == 'z') << std::endl;
std::cout << "Key :" << key << "," << x << "," << y << "||" << std::endl;
}
static void keyboardup(unsigned char key, int x, int y)
{
std::cout << "KUP :" << key << "," << x << "," << y << "||" << std::endl;
}
static void spacial(int key, int x, int y)
{
std::cout << "Spa :" << key << "," << x << "," << y << "||" << std::endl;
}
static void spacialup(int key, int x, int y)
{
std::cout << "SUP :" << key << "," << x << "," << y << "||" << std::endl;
}
*/