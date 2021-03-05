/*//////////////////////////////
File : Frame GL
Make : 160725
VER  : 0.05
*///////////////////////////////

#pragma once

#include <Prelude.h>

#include _CH_ADVANCEDCAMERA_
#include _CH_GUIINPUT_
#include _CH_FRAME_
#include _CH_GLUT_
#include _CH_PARTICLE_
#include _CH_PARTICLEMANAGER_
#include _CH_PMFUNCTION_
#include _CH_DRAW_
#include _CH_WAVE_

AdvencedCamera camera;
Camera FlatCamera[3];

Frame* MainView;

extern Object3D Teapot;
extern ParticleManager pm;
//extern pm_forcefield_cube Test1;

Wave wavRecoder;

float divide[2];

bool Pause;

#define MainViewGap 10
#define MainViewShelfGap 50
#define RefreshTime 25 //밀리세컨드 단위, 화면이 다시 드로우되는 간격 조정(fps)

extern int CalcTime;

//Particle get_particle(int i);
void calculation();
//int main(int argc, char **argv);
void GLStartingFunc();
//void TimerFunction(unsigned int time_mill, void(*function)());
void TimerFunction(unsigned int time_mill, void(*function)(int value), int value);

void PerspectiveReshape()
{
	glViewport(0, 0, MainView[1].GetWindowWidth(), MainView[1].GetWindowHeight());
	camera.SetSmoothMode(CAM_ADV_FIXED);
	camera.SetAspectRatio(MainView[1].GetWindowWidth(), MainView[1].GetWindowHeight());
	camera.SetCameraOrigin(CAM_TARGET);
	camera.SetClipDistance(1, 100000);
	camera.SetSmoothMode(CAM_ADV_SMOOTH);
}
void Camera1Reshape()
{
	glViewport(0, 0, MainView[2].GetWindowWidth(), MainView[2].GetWindowHeight());
	FlatCamera[0].SetAspectRatio(MainView[2].GetWindowWidth(), MainView[2].GetWindowHeight());
}
void Camera2Reshape()
{
	glViewport(0, 0, MainView[3].GetWindowWidth(), MainView[3].GetWindowHeight());
	FlatCamera[1].SetAspectRatio(MainView[3].GetWindowWidth(), MainView[3].GetWindowHeight());
}
void Camera3Reshape()
{
	glViewport(0, 0, MainView[4].GetWindowWidth(), MainView[4].GetWindowHeight());
	FlatCamera[2].SetAspectRatio(MainView[4].GetWindowWidth(), MainView[4].GetWindowHeight());
}

void PerspectiveCamera()
{
	camera.SetupCamera();
}
void Camera1()
{
	FlatCamera[0].SetupAllMatrix();
}
void Camera2()
{
	FlatCamera[1].SetupAllMatrix();
}
void Camera3()
{
	FlatCamera[2].SetupAllMatrix();
}

void DrawGrid(void)
{
	Vector3 MinPos = -((Vector3)pm.GetNumberOfGrid()) / 2.0 * KERNEL_DISTANCE + (pm.GetMaxPosition()+pm.GetMinPosition())/2.0;
	Vector3 MaxPos = ((Vector3)pm.GetNumberOfGrid()) / 2.0 * KERNEL_DISTANCE + (pm.GetMaxPosition() + pm.GetMinPosition()) / 2.0;
	Vector3 NumberOfGrid = pm.GetNumberOfGrid();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glColor4f(1, 1, 1, 0.2);
	glBegin(GL_LINE_STRIP);


	for (int i = 0; i <= NumberOfGrid.x; i++)
	{
		glVertex3f(MinPos.x + i*KERNEL_DISTANCE, MinPos.y, MinPos.z);
		glVertex3f(MinPos.x + i*KERNEL_DISTANCE, MaxPos.y, MinPos.z);
		glVertex3f(MinPos.x + i*KERNEL_DISTANCE, MaxPos.y, MaxPos.z);
		glVertex3f(MinPos.x + i*KERNEL_DISTANCE, MinPos.y, MaxPos.z);
		glVertex3f(MinPos.x + i*KERNEL_DISTANCE, MinPos.y, MinPos.z);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= NumberOfGrid.y; i++)
	{
		glVertex3f(MinPos.x, MinPos.y + i*KERNEL_DISTANCE, MinPos.z);
		glVertex3f(MaxPos.x, MinPos.y + i*KERNEL_DISTANCE, MinPos.z);
		glVertex3f(MaxPos.x, MinPos.y + i*KERNEL_DISTANCE, MaxPos.z);
		glVertex3f(MinPos.x, MinPos.y + i*KERNEL_DISTANCE, MaxPos.z);
		glVertex3f(MinPos.x, MinPos.y + i*KERNEL_DISTANCE, MinPos.z);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= NumberOfGrid.z; i++)
	{
		glVertex3f(MinPos.x, MinPos.y, MinPos.z + i*KERNEL_DISTANCE);
		glVertex3f(MaxPos.x, MinPos.y, MinPos.z + i*KERNEL_DISTANCE);
		glVertex3f(MaxPos.x, MaxPos.y, MinPos.z + i*KERNEL_DISTANCE);
		glVertex3f(MinPos.x, MaxPos.y, MinPos.z + i*KERNEL_DISTANCE);
		glVertex3f(MinPos.x, MinPos.y, MinPos.z + i*KERNEL_DISTANCE);
	}
	glEnd();
	glColor4f(0, 1, 0, 0.2);
	MinPos = pm.GetMinPosition();
	MaxPos = pm.GetMaxPosition();
	glBegin(GL_LINES);
	{
		glVertex3f(MinPos.x, MinPos.y, MaxPos.z);
		glVertex3f(MinPos.x, MaxPos.y, MaxPos.z);
		glVertex3f(MinPos.x, MaxPos.y, MaxPos.z);
		glVertex3f(MaxPos.x, MaxPos.y, MaxPos.z);
		glVertex3f(MaxPos.x, MaxPos.y, MaxPos.z);
		glVertex3f(MaxPos.x, MinPos.y, MaxPos.z);
		glVertex3f(MaxPos.x, MinPos.y, MaxPos.z);
		glVertex3f(MinPos.x, MinPos.y, MaxPos.z);

		glVertex3f(MinPos.x, MinPos.y, MinPos.z);
		glVertex3f(MinPos.x, MaxPos.y, MinPos.z);
		glVertex3f(MinPos.x, MaxPos.y, MinPos.z);
		glVertex3f(MaxPos.x, MaxPos.y, MinPos.z);
		glVertex3f(MaxPos.x, MaxPos.y, MinPos.z);
		glVertex3f(MaxPos.x, MinPos.y, MinPos.z);
		glVertex3f(MaxPos.x, MinPos.y, MinPos.z);
		glVertex3f(MinPos.x, MinPos.y, MinPos.z);

		glVertex3f(MinPos.x, MinPos.y, MinPos.z);
		glVertex3f(MinPos.x, MinPos.y, MaxPos.z);
		glVertex3f(MinPos.x, MaxPos.y, MinPos.z);
		glVertex3f(MinPos.x, MaxPos.y, MaxPos.z);
		glVertex3f(MaxPos.x, MaxPos.y, MinPos.z);
		glVertex3f(MaxPos.x, MaxPos.y, MaxPos.z);
		glVertex3f(MaxPos.x, MinPos.y, MinPos.z);
		glVertex3f(MaxPos.x, MinPos.y, MaxPos.z);
	}
	glEnd();
}

void DrawParticle(void)
{
	//glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	DrawParticles(pm,0,DRAW_POINTS, true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawParticles(pm, 1, DRAW_POINTS, false);
	//glColor3f(0, 1, 0);
	//glBegin(GL_LINES);
	static int select;
	if (mouse.isDown(false,MOUSE_LEFT|MOUSE_MIDDLE|MOUSE_RIGHT))
	{
		select = ((select + 1) % pm.GetParticles(0).count());
		std::cout << pm.GetSimpleDensity(0) << std::endl;
		wavRecoder.Flush();
	}
	/*
	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i < pm.size(Wall); i++)
	{
		glVertex3f(pm.wallData(i).position);
	}
	glEnd();*/

	/*
	if (pm.GetParticles(0).count() > 0)
	{
		unsigned int* &neighbor = pm.GetNearGridIndex(pm.GetParticles(0)[select].position);
		
		//근처 그리드를 조사하여
		for (unsigned char neighborhood = 1; neighborhood-1 < neighbor[0]; neighborhood++)
		{
			Vector3 tempMin = (((Vector3)pm.ConvertGridIndex(neighbor[neighborhood])) - ((Vector3)pm.GetNumberOfGrid() / 2)) * KERNEL_DISTANCE +(pm.GetMaxPosition() + pm.GetMinPosition()) / 2.0;
			Vector3 tempMax = tempMin + KERNEL_DISTANCE;
			
			glColor4f(1, 1, 1, 1);
			glBegin(GL_POINTS);
			{
				glVertex3f(pm.GetParticles(0)[select].position);
			}
			glEnd();
			glBegin(GL_LINE_LOOP);
			{
				glVertex3f(tempMin.x, tempMin.y, tempMin.z);
				glVertex3f(tempMax.x, tempMin.y, tempMin.z);
				glVertex3f(tempMax.x, tempMax.y, tempMin.z);
				glVertex3f(tempMin.x, tempMax.y, tempMin.z);
			}
			glEnd();
			glBegin(GL_LINE_LOOP);
			{
				glVertex3f(tempMin.x, tempMin.y, tempMax.z);
				glVertex3f(tempMax.x, tempMin.y, tempMax.z);
				glVertex3f(tempMax.x, tempMax.y, tempMax.z);
				glVertex3f(tempMin.x, tempMax.y, tempMax.z);
			}
			glBegin(GL_LINES);
			{
				glVertex3f(tempMin.x, tempMin.y, tempMax.z);
				glVertex3f(tempMin.x, tempMin.y, tempMin.z);
				glVertex3f(tempMax.x, tempMin.y, tempMax.z);
				glVertex3f(tempMax.x, tempMin.y, tempMin.z);
				glVertex3f(tempMax.x, tempMax.y, tempMax.z);
				glVertex3f(tempMax.x, tempMax.y, tempMin.z);
				glVertex3f(tempMin.x, tempMax.y, tempMax.z);
				glVertex3f(tempMin.x, tempMax.y, tempMin.z);
			}
			glEnd();
		}
		//std::cout << pm.GetParticles(0)[select].density << std::endl;
	}
	*/
	/*GLfloat arMat4[] = { 1, 1, 1, 1 };
	for (int i = 0; i < NUMBER_OF_PARTICLES; i++)
	{
		arMat4[0] = (float)i / NUMBER_OF_PARTICLES;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, arMat4);
		ParticleFunction(get_particle(i));
	}*/
}
void DrawAim()
{
	glDisable(GL_LIGHTING);
	Vector3 temp;
	glPointSize(15);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	temp = camera.GetTargetPosition();
	glVertex3f(temp.x, temp.y, temp.z);
	//temp = camera.GetTargetPosition() + 5 * camera.GetCameraX();
	temp = camera.GetTargetPosition() + Vector3(5, 0, 0);
	glVertex3f(temp.x, temp.y, temp.z);
	glColor3f(0, 1, 0);
	temp = camera.GetTargetPosition() + Vector3(0, 5, 0);
	glVertex3f(temp.x, temp.y, temp.z);
	glColor3f(0, 0, 1);
	temp = camera.GetTargetPosition() + Vector3(0, 0, 5);
	glVertex3f(temp.x, temp.y, temp.z);
	glEnd();
}


void Allclick()
{
	if (mouse.isDown(false, MOUSE_LEFT | MOUSE_RIGHT)){
		camera.SetViewMode(CAM_PERSPECTIVE);
	}
	if (mouse.isDown(false, MOUSE_MIDDLE | MOUSE_RIGHT))
	{
		camera.SetViewMode(CAM_ISOMETRIC);
	}

}

void AllMotion()
{
	float transfScale = 0.002*camera.GetFocusDistance();
	float rotateScale = 0.2;
	if (mouse.isDown(false, MOUSE_LEFT)) { camera.AddCameraLocalPosition(-mouse.GetDeltaX() * transfScale, mouse.GetDeltaY() * transfScale, 0); }
	if (mouse.isDown(false, MOUSE_MIDDLE)){ camera.AddFocusDistance(-mouse.GetDeltaY() * transfScale); }
	if (mouse.isDown(false, MOUSE_LEFT | MOUSE_MIDDLE)) { camera.AddCameraLocalRotation(0, 0, mouse.GetDeltaX()); }
	if (mouse.isDown(false, MOUSE_RIGHT)){ camera.AddCameraLocalRotation(-mouse.GetDeltaY() * rotateScale, -mouse.GetDeltaX()  * rotateScale, 0); }
}

void Camera1Motion()
{
	float transfScale = 0.002*FlatCamera[0].GetFocusDistance();
	
	if (mouse.isDown(false, MOUSE_LEFT)) { FlatCamera[0].AddCameraLocalPosition(-mouse.GetDeltaX() * transfScale, mouse.GetDeltaY() * transfScale, 0); }
	if (mouse.isDown(false, MOUSE_MIDDLE)){ FlatCamera[0].AddFocusDistance(-mouse.GetDeltaY() * transfScale); }
	//if (mouse.isDown(false, MOUSE_RIGHT)){ FlatCamera[0].AddCameraLocalRotation(-mouse.GetDeltaY() * rotateScale, -mouse.GetDeltaX()  * rotateScale, 0); }
}
void Camera2Motion()
{
	float transfScale = 0.002*FlatCamera[1].GetFocusDistance();
	if (mouse.isDown(false, MOUSE_LEFT)) { FlatCamera[1].AddCameraLocalPosition(-mouse.GetDeltaX() * transfScale, mouse.GetDeltaY() * transfScale, 0); }
	if (mouse.isDown(false, MOUSE_MIDDLE)){ FlatCamera[1].AddFocusDistance(-mouse.GetDeltaY() * transfScale); }
}
void Camera3Motion()
{
	float transfScale = 0.002*FlatCamera[2].GetFocusDistance();
	if (mouse.isDown(false, MOUSE_LEFT)) { FlatCamera[2].AddCameraLocalPosition(-mouse.GetDeltaX() * transfScale, mouse.GetDeltaY() * transfScale, 0); }
	if (mouse.isDown(false, MOUSE_MIDDLE)){ FlatCamera[2].AddFocusDistance(-mouse.GetDeltaY() * transfScale); }
}


static int times;

void CalcTimer(int value)
{
	if (!Pause) {
		times++;
		TimerFunction(CalcTime, CalcTimer, 50);
	}
}
void CalcRunner(int value)
{
	if (!Pause) {
		if (times>0)
		{
			//	std::cout  << times << "times" << std::endl;
			times--;
			static double time = 0;
			//time = clock();
			calculation();
			wavRecoder.Append(pm.GetSimpleDensity(0, Vector3(MAX_X, 20, -130), Vector3(MIN_X, -20, -150)));
			//std::cout << ( (clock() - time) / CLOCKS_PER_SEC )<< std::endl;
			std::cout << time++ << std::endl;
		}
		if (times > 0)
		{
			CalcRunner(0);
		}
		else
		{
			glutTimerFunc(50, CalcRunner, 50);
		}
	}
}
void Redisplay(int value)
{
	for (int i = 0; i < 6; i++)
	{
		MainView[i].UpdateDisplay();
	}
	TimerFunction(RefreshTime, Redisplay, 0);

}


void Main5Draw()
{
	glViewport(0, 0, MainView[5].GetWindowWidth(), MainView[5].GetWindowHeight());
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	//glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glColor3f(0.7, 0.7, 0.7);
	glVertex2f(-1, 1);
	glColor3f(1, 1, 1);
	glVertex2f(1, -1);
	glColor3f(0.7, 0.7, 0.7);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();
}

void Main5Draw1()
{
	if (Pause)
	{
		glColor3f(1, 0, 0);
	}
	else
	{
		glColor3f(0, 1, 0);
	}
	
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0.25, 0);
	glVertex2f(0.25, 0.5);
	glVertex2f(0, 0.5);
	glEnd();
}


double Z_MOVE = 0;
void DrawTeapot()
{
	/*
	if (mouse.isDown(false, MOUSE_LEFT | MOUSE_MIDDLE))
	{
		Test1.Switch(true);
	}
	else
	{
		Test1.Switch(false);
	}*/
	/*//if (mouse.isDown(true,MOUSE_LEFT))
	{
		for (int i = 0; i < pm.size(); i++)
		{
			if (pm.isInside(i, Vector3(20, -80, -80), Vector3(-20, -100, -100)))
			{
				pm.removeParticle(i);
			}
		}
	}
	//if (mouse.isDown(true,MOUSE_RIGHT))
	{
		pm.createParticle(Vector3(rand() % 10000 / 10000.0 * 40 - 20, 95, rand() % 10000 / 10000.0 * 20 + 80), Vector3(0, -rand() % 10000 / 10000.0, 0), PARTICLE_MASS);
	}
	
	for (unsigned int i = 0; i < pm.GetParticles(0).count(); i++)
	{
		if (pm.GetParticles(0)[i].position > Vector3(MIN_X, MIN_Y, MIN_Z) && pm.GetParticles(0)[i].position < Vector3(MIN_X, MIN_Y, MIN_Z) + 30)
		{
			pm.RemoveParticle(0,i);
		}
	}*/
	if (mouse.isDown(false, MOUSE_LEFT | MOUSE_MIDDLE))
	{
		
		/*
		for (int i = 0; i < 10; i++)
		{
			Particle temp;
			temp.position = v3rand<phys_t>(pm.GetMaxPosition(),pm.GetMinPosition() );
			temp.velocity = Vector3(0, 0, 0);
			temp.viscosity = 1;
			temp.mass = PARTICLE_MASS;
			pm.CreateParticle(0, temp);
			
		}
		std::cout << pm.GetParticles(0).count() << std::endl;
		*/
	}
	/*
	if (mouse.isDown(false, MOUSE_LEFT | MOUSE_MIDDLE))
	{
		//std::cout << "퍄";
		static double time;
		time -= 0.1*UNIT_TIME;
		Z_MOVE = sin(time) * KERNEL_DISTANCE*2;
	}*/
	//pm[0].pressure = 100.0;
	//Test.AddRotation(Vector3(0, 3, 3), false);
	if (Pause)
	{
		//glEnable(GL_LIGHTING);
		glBegin(GL_TRIANGLES);
		DrawMesh(Teapot);
		glEnd();
	}
}
void GLwindowInitFunction()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_DEPTH_TEST);

	//glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light[] = { 100, 100, 100, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light);
	light[0] = 0.1; light[1] = 0.1; light[2] = 0.1; light[3] = 1;
	glLightfv(GL_LIGHT0, GL_AMBIENT, light);
	light[0] = 0.5; light[1] = 0.5; light[2] = 0.5; light[3] = 1;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
	
}

void PauseFunc()
{
	if (mouse.isDown(true, MOUSE_LEFT)){
		if (Pause) {
			Pause = false;
			CalcTimer(0);
			CalcRunner(0);
		}
		else
		{
			Pause = true;
		}
	}
}

void WindowResize()
{
	int temp_divide_x = (divide[0]) * MainView[0].GetWindowWidth();
	int temp_divide_x_other = (1 - divide[0]) * MainView[0].GetWindowWidth();
	int temp_divide_y = (divide[1]) * (MainView[0].GetWindowHeight() - MainViewShelfGap);
	int temp_divide_y_other = (1 - divide[1]) * (MainView[0].GetWindowHeight() - MainViewShelfGap);

	MainView[1].SetWindowSize(temp_divide_x - MainViewGap, temp_divide_y - MainViewGap);
	MainView[2].SetWindowSize(temp_divide_x_other - MainViewGap, temp_divide_y - MainViewGap);
	MainView[3].SetWindowSize(temp_divide_x - MainViewGap, temp_divide_y_other - MainViewGap);
	MainView[4].SetWindowSize(temp_divide_x_other - MainViewGap, temp_divide_y_other - MainViewGap);

	MainView[1].SetWindowPosition(MainViewGap / 2, MainViewShelfGap + MainViewGap / 2);
	MainView[2].SetWindowPosition(temp_divide_x + MainViewGap / 2, MainViewShelfGap + MainViewGap / 2);
	MainView[3].SetWindowPosition(MainViewGap / 2, MainViewShelfGap + temp_divide_y + MainViewGap / 2);
	MainView[4].SetWindowPosition(temp_divide_x + MainViewGap / 2, MainViewShelfGap + temp_divide_y + MainViewGap / 2);

	MainView[5].SetWindowPosition(0, 0);
	MainView[5].SetWindowSize(MainView[0].GetWindowWidth(), MainViewShelfGap);
}
void WindowMove()
{
	int temp_divide_x = (divide[0]) * MainView[0].GetWindowWidth();
	int temp_divide_y = (divide[1]) * (MainView[0].GetWindowHeight() - MainViewShelfGap);
	int local_mouse_x = mouse.GetX() - MainView[0].GetWindowX();
	int local_mouse_y = mouse.GetY() - MainView[0].GetWindowY();

	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	if (local_mouse_x > temp_divide_x - MainViewGap / 2 && local_mouse_x < temp_divide_x + MainViewGap / 2)
	{
		glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
	}
	if (local_mouse_y > MainViewShelfGap + temp_divide_y - MainViewGap / 2 && local_mouse_y < MainViewShelfGap + temp_divide_y + MainViewGap / 2)
	{
		if (glutGet(GLUT_WINDOW_CURSOR) == GLUT_CURSOR_LEFT_RIGHT)
		{
			glutSetCursor(GLUT_CURSOR_SPRAY);
		}
		else
		{
			glutSetCursor(GLUT_CURSOR_UP_DOWN);
		}
	}
}
bool move[2];
void WindowClick()
{
	int temp_divide_x = (divide[0]) * MainView[0].GetWindowWidth();
	int temp_divide_y = (divide[1]) * (MainView[0].GetWindowHeight() - MainViewShelfGap);
	int local_mouse_x = mouse.GetX() - MainView[0].GetWindowX();
	int local_mouse_y = mouse.GetY() - MainView[0].GetWindowY();

	move[0] = false;
	move[1] = false;
	if (mouse.isDown(true, MOUSE_LEFT))
	{

		if (local_mouse_x > temp_divide_x - MainViewGap / 2 && local_mouse_x < temp_divide_x + MainViewGap / 2)
		{
			move[0] = true;
		}
		if (local_mouse_y > MainViewShelfGap + temp_divide_y - MainViewGap / 2 && local_mouse_y < MainViewShelfGap + temp_divide_y + MainViewGap / 2)
		{
			move[1] = true;
		}
	}
}
void WindowDrag()
{


	if (move[0])
	{
		divide[0] += mouse.GetDeltaX() / ((float)MainView[0].GetWindowWidth());
		//std::cout << divide[0] << std::endl;
	}
	if (move[1])
	{
		divide[1] += mouse.GetDeltaY() / ((float)(MainView[0].GetWindowHeight() - MainViewShelfGap));

	}
	if (divide[0] > 0.95) { divide[0] = 0.95; }
	if (divide[0] < 0.05) { divide[0] = 0.05; }
	if (divide[1] > 0.95) { divide[1] = 0.95; }
	if (divide[1] < 0.05) { divide[1] = 0.05; }

	WindowResize();
}

void GLInitialize(int value)
{
	divide[0] = 0.5;
	divide[1] = 0.5;
	Pause = false;

	for (int i = 1; i < 6; i++)
	{
		MainView[0].SetChild(MainView + i);
		MainView[i].PushInitAfterCreateFunc(GLwindowInitFunction, 0);
		MainView[i].SetLink(true);
		MainView[i].SetSubWindow(true);
		MainView[i].WindowCreate();
		MainView[i].PushMouseMoveFunc([](){glutSetCursor(GLUT_CURSOR_LEFT_ARROW); }, 0);
	}
	WindowResize();
	for (int i = 1; i < 5; i++)
	{
		MainView[i].PushDisplayFunc(DrawParticle, 1);
		MainView[i].PushDisplayFunc(DrawGrid, 0);
	}
	MainView[0].PushReshapeFunc(WindowResize, -1);
	MainView[0].PushMouseDragFunc(WindowDrag, -1);
	MainView[0].PushMouseMoveFunc(WindowMove, -1);
	MainView[0].PushMouseClickFunc(WindowClick, 0);

	MainView[1].PushReshapeFunc(PerspectiveReshape, 0);
	MainView[1].PushDisplayFunc(PerspectiveCamera, -1);
	MainView[1].PushDisplayFunc(DrawAim, 2);
	MainView[1].PushDisplayFunc(DrawTeapot, 3);
	MainView[1].PushMouseDragFunc(AllMotion, 0);
	MainView[1].PushMouseClickFunc(Allclick, 0);

	MainView[2].PushReshapeFunc(Camera1Reshape, 0);
	MainView[3].PushReshapeFunc(Camera2Reshape, 0);
	MainView[4].PushReshapeFunc(Camera3Reshape, 0);

	MainView[2].PushDisplayFunc(Camera1, -1);
	MainView[3].PushDisplayFunc(Camera2, -1);
	MainView[4].PushDisplayFunc(Camera3, -1);

	MainView[5].PushDisplayFunc(Main5Draw, 0);
	MainView[5].PushDisplayFunc(Main5Draw1, 1);
	MainView[5].PushMouseClickFunc(PauseFunc, 0);
	
	MainView[2].PushMouseDragFunc(Camera1Motion, 0);
	MainView[3].PushMouseDragFunc(Camera2Motion, 0);
	MainView[4].PushMouseDragFunc(Camera3Motion, 0);

	for (int i = 0; i < 3; i++)
	{
		//FlatCamera[i].SetViewMode(CAM_ISOMETRIC);
		FlatCamera[i].SetFocusDistance(100);
		FlatCamera[i].SetClipDistance(1, 10000);
		FlatCamera[i].SetViewMode(CAM_ISOMETRIC);
		FlatCamera[i].SetCameraRotationLimit(false);
	}
	FlatCamera[0].SetCameraRotation(0, 90, 0);
	FlatCamera[1].SetCameraRotation(90, 0, 0);
	FlatCamera[2].SetCameraRotation(0, 0, 0);

	camera.SetSmoothMode(CAM_ADV_FIXED);
	camera.AddFocusDistance(100);
	camera.SetViewMode(CAM_PERSPECTIVE);
	camera.SetCameraOrigin(CAM_TARGET);
	camera.SetSmoothMode(CAM_ADV_SMOOTH);

	CalcTimer(10);
	CalcRunner(10);
	Redisplay(10);

	wavRecoder.Open("TESTWAVE");
}
void GLStartingFunc()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_ACCUM | GLUT_DOUBLE);
	MainView = new Frame[6];

	//Mainview[0] : Container
	MainView[0].SetWindowTitle("입자, 소리, 관악기 그리고 GPU");
	MainView[0].SetLink(true);
	MainView[0].SetWindowPosition(100, 100);
	MainView[0].SetWindowSize(600, 600);
	MainView[0].WindowCreate();
	TimerFunction(10, GLInitialize, 0);
	glutMainLoop();


}


void TimerFunction(unsigned int time_mill, void(*function)(int value), int value)
{
	glutTimerFunc(time_mill, function, value);
}