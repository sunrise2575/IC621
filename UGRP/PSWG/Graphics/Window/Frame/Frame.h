//
//최대한 각 프레임들은 따로 따로 동작하게 설계함. 다른 개체
//단지 링크를 통해 연결되어 작업할 수 있을 뿐.
//따라 상위 프레임을 복제해도 하위 프레임과 그 링크는 복제되지 않는다.
//
#pragma once

#include <Prelude.h>
#include _CH_GLUT_
#include _CH_GUIINPUT_


#define FRAME_INIT_BEFORE 0
#define FRAME_INIT_AFTER 1
#define FRAME_DISPLAY 2
#define FRAME_RESHAPE 3
#define FRAME_MOUSE_CLICK 4
#define FRAME_MOUSE_MOTION 5
#define FRAME_MOUSE_PASSIVE 6
#define FRAME_FUNC_NUM 7   //총 func number

class Frame
{
private:

	
	bool bCreated;//창이 생성되어있는지 여부. 생성되었으면 true
	bool bShown;//창이 나타나있는지 여부. Hide 상태면 false

	char* _title; int _windowHandle;//생성된 창이 있다면, 그창의 glutGetWindow(); 에서 반환된 값
	short _x;	short _y;	short _width;	short _height;
	bool bInitializePosition; bool bInitializeSize;

	bool bSubWindow;//창이 SubWindow인지 여부.
	bool bCreateLink; //true일 경우 ParentFrame이 생성될 경우 자동으로 생성됨.
	Frame* _ParentFrame;//NULL일 경우, main window, 아닐경우 Subwindow
	std::vector<Frame*> _ChildFrame;// short _ChildNumber; short _ChildFrameSize;

	void(***FuncArrayRack)();	short* ArrayNumberRack;	short** ArrayOrderRack;

	void RunFuncArrayRack(int FRAME_func);

protected:

public:
	Frame();
	~Frame();
	Frame(const Frame &Frame);
	Frame& operator=(const Frame &Frame);

	void WindowCreate();
	void WindowDestroy();
	void WindowHide();
	void WindowIconfiy();
	void WindowShow();
	void WindowPush();
	void WindowPop();

	void SetParent(Frame* beParent);
	void SetChild(Frame* beChild);
	void SetLink(bool LinkOn);

	void SetSubWindow(bool isSubWindow);

	void SetWindowTitle(const char* name);
	void SetWindowPosition(const int &x, const int &y);
	void SetWindowSize(const int &width, const int &height);

	void PushFunc(short FRAME_func, void(*func)(), short order);
	void PopFunc(short FRAME_func, void(*func)(), short order);

	void PushInitBeforeCreateFunc(void(*Initialize)(), short order);
	void PushInitAfterCreateFunc(void(*Initialize)(), short order);
	void PushDisplayFunc(void(*Display)(), short order);
	void PushReshapeFunc(void(*Reshape)(), short order);
	void PushMouseClickFunc(void(*MouseClick)(), short order);
	void PushMouseDragFunc(void(*MouseDrag)(), short order);
	void PushMouseMoveFunc(void(*MouseMove)(), short order);

	void PopInitBeforeCreateFunc(void(*Initialize)(), short order);
	void PopInitAfterCreateFunc(void(*Initialize)(), short order);
	void PopDisplayFunc(void(*Display)(), short order);
	void PopReshapeFunc(void(*Reshape)(), short order);
	void PopMouseClickFunc(void(*MouseClick)(), short order);
	void PopMouseDragFunc(void(*MouseDrag)(), short order);
	void PopMouseMoveFunc(void(*MouseMove)(), short order);

	void UpdateDisplay();

	char* GetWindowTitle();
	short GetWindowX();
	short GetWindowY();
	short GetWindowWidth();
	short GetWindowHeight();
	int GetWindow();//만약 윈도우가 생성되지 않았을 경우 0을 반환
	bool GetWindowShown();
	

	//직접 호출할 필요가 없는 함수들.
	void _VirtualBeforeFunc();
	void _VirtualAfterFunc();
	void _VirtualDisplayFunc();
	void _VirtualReshapeFunc(int x, int y);
	void _VirtualMouseClickFunc(int button, int updown, int x, int y);
	void _VirtualMouseMotionFunc(int x,int y);
	void _VirtualMousePassiveFunc(int x, int y);
};

/*
class FrameManage
{


};

FrameManage FrameManager;
*/


int FrameWindowforGlutFuncSize = 128;
Frame** FrameWindowforGlutFunc = new Frame*[FrameWindowforGlutFuncSize];
//Glut에서 멤버 함수를 쓸 수 없기 때문에, glutGetwindow를 통해 부른 Frame을 알아내려함.


void DisplayFunc(){ FrameWindowforGlutFunc[glutGetWindow()]->_VirtualDisplayFunc(); }
void ReshapeFunc(int x, int y){ FrameWindowforGlutFunc[glutGetWindow()]->_VirtualReshapeFunc(x, y); }
void MouseClickFunc(int button, int updown, int x, int y){ FrameWindowforGlutFunc[glutGetWindow()]->_VirtualMouseClickFunc(button, updown, x, y); }
void MouseMotionFunc(int x, int y){ FrameWindowforGlutFunc[glutGetWindow()]->_VirtualMouseMotionFunc(x, y); }
void MousePassiveFunc(int x, int y){ FrameWindowforGlutFunc[glutGetWindow()]->_VirtualMousePassiveFunc(x, y); }

Frame::Frame()
{
	bCreated = false;//창이 생성되어있는지 여부. 생성되었으면 true
	bShown = true;//창이 나타나있는지 여부. Hide 상태면 false

	_title = new char[9];
	strcpy_s(_title, 9, "Untitled");

	_windowHandle = NULL;//생성된 창이 있다면, 그창의 glutGetWindow(); 에서 반환된 값
	_x = 0;
	_y = 0;
	_width = 0;
	_height = 0;
	bInitializePosition = false; bInitializeSize = false;

	bSubWindow = false;
	bCreateLink = true;
	_ParentFrame = NULL;
	_ChildFrame.clear(); //_ChildNumber = 0; _ChildFrameSize = 0;

	FuncArrayRack = new (void(**[FRAME_FUNC_NUM])());
	ArrayNumberRack = new short[FRAME_FUNC_NUM];
	ArrayOrderRack = new short*[FRAME_FUNC_NUM];

	for (int i = 0; i < FRAME_FUNC_NUM; i++)
	{
		FuncArrayRack[i] = NULL;
		ArrayNumberRack[i] = 0;
		ArrayOrderRack[i] = NULL;
	}
}
Frame::~Frame()
{
	WindowDestroy();
	delete[] _title;
	for (size_t i = 0; i < _ChildFrame.size(); i++)
	{
		_ChildFrame[i]->SetParent(NULL);
	}
	_ChildFrame.clear();


	for (int i = 0; i < FRAME_FUNC_NUM; i++)
	{
		delete[] FuncArrayRack[i];
		delete[] ArrayOrderRack[i];
	}

	delete[] FuncArrayRack;
	delete[] ArrayNumberRack;
	delete[] ArrayOrderRack;

}
Frame::Frame(const Frame &oFrame)
{
	bCreated = false;//창이 생성되어있는지 여부. 생성되었으면 true
	bShown = oFrame.bShown;//창이 나타나있는지 여부. Hide 상태면 false

	_title = new char[strlen(oFrame._title) + 1];
	strcpy_s(_title, strlen(oFrame._title) + 1, oFrame._title);

	_windowHandle = NULL;//생성된 창이 있다면, 그창의 glutGetWindow(); 에서 반환된 값
	_x = oFrame._x;
	_y = oFrame._y;
	_width = oFrame._width;
	_height = oFrame._height;
	bInitializePosition = oFrame.bInitializePosition;
	bInitializeSize = oFrame.bInitializeSize;

	bSubWindow = false;
	bCreateLink = true;
	_ParentFrame = NULL;
	_ChildFrame.clear();// _ChildNumber = 0; _ChildFrameSize = 0;

	FuncArrayRack = new (void(**[FRAME_FUNC_NUM])());
	ArrayNumberRack = new short[FRAME_FUNC_NUM];
	ArrayOrderRack = new short*[FRAME_FUNC_NUM];

	for (int i = 0; i < FRAME_FUNC_NUM; i++)
	{
		ArrayNumberRack[i] = oFrame.ArrayNumberRack[i];
		FuncArrayRack[i] = new (void(*[ArrayNumberRack[i]])());
		ArrayOrderRack[i] = new short[ArrayNumberRack[i]];
		memcpy(FuncArrayRack[i], oFrame.FuncArrayRack[i], sizeof(void((*)())) * ArrayNumberRack[i]);
		memcpy(ArrayOrderRack[i], oFrame.ArrayOrderRack[i], sizeof(short) * ArrayNumberRack[i]);
	}
}
Frame& Frame::operator=(const Frame &oFrame)
{
	if (&oFrame == this) { return *this; }

	this->WindowDestroy();

	bCreated = false;//창이 생성되어있는지 여부. 생성되었으면 true
	bShown = oFrame.bShown;//창이 나타나있는지 여부. Hide 상태면 false

	_title = new char[strlen(oFrame._title) + 1];
	strcpy_s(_title, strlen(oFrame._title) + 1, oFrame._title);

	_windowHandle = NULL;//생성된 창이 있다면, 그창의 glutGetWindow(); 에서 반환된 값
	_x = oFrame._x;
	_y = oFrame._y;
	_width = oFrame._width;
	_height = oFrame._height;
	bInitializePosition = oFrame.bInitializePosition;
	bInitializeSize = oFrame.bInitializeSize;

	bSubWindow = false;
	bCreateLink = true;
	_ParentFrame = NULL;
	_ChildFrame.clear();// _ChildNumber = 0; _ChildFrameSize = 0;

	for (int i = 0; i < FRAME_FUNC_NUM; i++)
	{
		ArrayNumberRack[i] = oFrame.ArrayNumberRack[i];
		FuncArrayRack[i] = new (void(*[ArrayNumberRack[i]])());
		ArrayOrderRack[i] = new short[ArrayNumberRack[i]];
		memcpy(FuncArrayRack[i], oFrame.FuncArrayRack[i], sizeof(void((*)())) * ArrayNumberRack[i]);
		memcpy(ArrayOrderRack[i], oFrame.ArrayOrderRack[i], sizeof(short) * ArrayNumberRack[i]);
	}

	return *this;
}

void Frame::WindowCreate()
{
	if (!bCreated)
	{
		int temp_setWindowNumber = glutGetWindow();

		//창 생성부
		if (!bInitializePosition){ bInitializePosition = true; _x = glutGet(GLUT_INIT_WINDOW_X); _y = glutGet(GLUT_INIT_WINDOW_Y); }
		if (!bInitializeSize){ bInitializeSize = true; _width = glutGet(GLUT_INIT_WINDOW_WIDTH); _height = glutGet(GLUT_INIT_WINDOW_HEIGHT); }

		if (bSubWindow)
		{
			if (_ParentFrame == NULL) { std::cout << "부모 프레임을 찾을 수 없습니다. (Frame == NULL)" << std::endl; }
			else if ((*_ParentFrame).GetWindow() == NULL) { std::cout << "부모 윈도우를 찾을 수 없습니다. (Window == NULL)" << std::endl; }
			else
			{
				_VirtualBeforeFunc();
				_windowHandle = glutCreateSubWindow((*_ParentFrame).GetWindow(), _x, _y, _width, _height);
			}
		}
		else
		{
			_VirtualBeforeFunc();
			_windowHandle = glutCreateWindow(_title);
			glutPositionWindow(_x, _y);
			glutReshapeWindow(_width, _height);
		}

		//창 등록
		if (_windowHandle >= FrameWindowforGlutFuncSize)
		{
			Frame** tempMemAddress = new Frame*[_windowHandle + 1];
			memcpy(tempMemAddress, FrameWindowforGlutFunc, FrameWindowforGlutFuncSize*sizeof(Frame*));
			delete[] FrameWindowforGlutFunc;
			FrameWindowforGlutFunc = tempMemAddress;
			FrameWindowforGlutFuncSize = _windowHandle + 1;
		}
		FrameWindowforGlutFunc[_windowHandle] = this;

		//함수등록
		_VirtualAfterFunc();
		glutDisplayFunc(DisplayFunc);
		glutReshapeFunc(ReshapeFunc);
		glutMouseFunc(MouseClickFunc);
		glutMotionFunc(MouseMotionFunc);
		glutPassiveMotionFunc(MousePassiveFunc);

		if (!bShown) { glutHideWindow(); }

		if (temp_setWindowNumber != 0) { glutSetWindow(temp_setWindowNumber); }
		bCreated = true;

		if (bCreateLink)
		{
			for (size_t i = 0; i < _ChildFrame.size(); i++)
			{
				if (_ChildFrame[i]->bCreateLink) { _ChildFrame[i]->WindowCreate(); };
			}
		}
	}
}
void Frame::WindowDestroy()
{
	if (bCreated)
	{
		for (size_t i = 0; i < _ChildFrame.size(); i++)
		{
			_ChildFrame[i]->WindowDestroy();
		}
		bCreated = false;
		glutDestroyWindow(_windowHandle);
	}
}
void Frame::WindowHide()
{
	bShown = false;
	if (bCreated)
	{
		int temp_setWindowNumber = glutGetWindow();
		glutSetWindow(_windowHandle);
		glutHideWindow();
		glutSetWindow(temp_setWindowNumber);
	}
}
void Frame::WindowIconfiy()
{
	if (bSubWindow){ WindowHide(); }
	else
	{
		bShown = false;
		if (bCreated)
		{
			int temp_setWindowNumber = glutGetWindow();
			glutSetWindow(_windowHandle);
			glutIconifyWindow();
			glutSetWindow(temp_setWindowNumber);
		}
	}
}
void Frame::WindowShow()
{
	bShown = true;
	if (bCreated)
	{
		int temp_setWindowNumber = glutGetWindow();
		glutSetWindow(_windowHandle);
		glutShowWindow();
		glutSetWindow(temp_setWindowNumber);
	}
}
void Frame::WindowPush()
{
	if (bCreated)
	{
		int temp_setWindowNumber = glutGetWindow();
		glutSetWindow(_windowHandle);
		glutPushWindow();
		glutSetWindow(temp_setWindowNumber);
	}
}
void Frame::WindowPop()
{
	if (bCreated)
	{
		int temp_setWindowNumber = glutGetWindow();
		glutSetWindow(_windowHandle);
		glutPopWindow();
		glutSetWindow(temp_setWindowNumber);
	}
}

void Frame::SetParent(Frame* beParent)
{
	if (bSubWindow){ WindowDestroy(); }
	if (_ParentFrame != NULL)
	{
		for (size_t i = 0; i < (_ParentFrame->_ChildFrame.size()); i++)
		{
			if (_ParentFrame->_ChildFrame[i] == this) { _ParentFrame->_ChildFrame.erase((_ParentFrame->_ChildFrame).begin() + i); }
		}
	}
	_ParentFrame = beParent;
	if (beParent != NULL){ _ParentFrame->_ChildFrame.push_back(this); }
}
void Frame::SetChild(Frame *beChild)
{
	if (beChild != NULL){ beChild->SetParent(this); }
}
void Frame::SetLink(bool LinkOn){ bCreateLink = LinkOn; }

void Frame::SetSubWindow(bool isSubWindow)
{
	if (bSubWindow != isSubWindow)
	{
		WindowDestroy();
		bSubWindow = isSubWindow;
	}
}

void Frame::SetWindowTitle(const char* name)
{
	delete[] _title;
	_title = new char[strlen(name) + 1];
	strcpy_s(_title, strlen(name) + 1, name);
	if (bCreated && (_ParentFrame == NULL))
	{
		int temp_setWindowNumber = glutGetWindow();
		glutSetWindow(_windowHandle);
		glutSetWindowTitle(_title);
		glutSetWindow(glutGetWindow());
	}
}
void Frame::SetWindowPosition(const int &x, const int &y)
{
	_x = x; _y = y;
	bInitializePosition = true;
	if (bCreated)
	{
		int temp_setWindowNumber = glutGetWindow();
		glutSetWindow(_windowHandle);
		glutPositionWindow(x, y);
		glutSetWindow(glutGetWindow());
	}
}
void Frame::SetWindowSize(const int &width, const int &height)
{
	_width = width; _height = height;
	bInitializeSize = true;
	if (bCreated)
	{
		int temp_setWindowNumber = glutGetWindow();
		glutSetWindow(_windowHandle);
		glutReshapeWindow(_width, _height);
		glutSetWindow(temp_setWindowNumber);
	}
}

void Frame::PushFunc(short FRAME_func, void(*func)(), short order)
{
	/*std::cout << "Push--------------" << std::endl;
	std::cout << "Func" << func << std::endl;
	std::cout << "Order" << order << std::endl;
	std::cout << std::endl;
	std::cout << "currentFunc : ";
	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	{
	std::cout << ", \t" << FuncArrayRack[FRAME_func][i];
	}
	std::cout << std::endl;
	std::cout << "currentOrder : ";
	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	{
	std::cout << ", \t" << ArrayOrderRack[FRAME_func][i];
	}
	std::cout << std::endl;
	std::cout << std::endl;*/

	short* tempOrder = new short[ArrayNumberRack[FRAME_func] + 1];
	void((**tempFunc)()) = new (void(*[ArrayNumberRack[FRAME_func] + 1])());
	memcpy(tempOrder, ArrayOrderRack[FRAME_func], sizeof(short)*ArrayNumberRack[FRAME_func]);
	memcpy(tempFunc, FuncArrayRack[FRAME_func], sizeof(void(*)())*ArrayNumberRack[FRAME_func]);
	tempOrder[ArrayNumberRack[FRAME_func]] = order;
	tempFunc[ArrayNumberRack[FRAME_func]] = func;

	short tempOrderforSwap;
	void(*tempFuncforSwap)();
	for (int i = ArrayNumberRack[FRAME_func]; i > 0; i--)
	{
		if (tempOrder[i - 1] > tempOrder[i])
		{
			tempOrderforSwap = tempOrder[i]; tempFuncforSwap = tempFunc[i];
			tempOrder[i] = tempOrder[i - 1]; tempFunc[i] = tempFunc[i - 1];
			tempOrder[i - 1] = tempOrderforSwap; tempFunc[i - 1] = tempFuncforSwap;
		}
		else{ break; }
	}
	ArrayNumberRack[FRAME_func]++;

	delete[] ArrayOrderRack[FRAME_func];
	delete[] FuncArrayRack[FRAME_func];
	ArrayOrderRack[FRAME_func] = tempOrder;
	FuncArrayRack[FRAME_func] = tempFunc;

	/*std::cout << "ChangeFunc : ";
	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	{
	std::cout << ", \t" << FuncArrayRack[FRAME_func][i];
	}
	std::cout << std::endl;
	std::cout << "ChangeOrder : ";
	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	{
	std::cout << ", \t" << ArrayOrderRack[FRAME_func][i];
	}
	std::cout << std::endl;
	std::cout << "-------------------" << std::endl;
	std::cout << std::endl;*/
}
void Frame::PopFunc(short FRAME_func, void(*func)(), short order)
{
	/*std::cout << "Pop---------------" << std::endl;
	std::cout << "Func" << func << std::endl;
	std::cout << "Order" << order << std::endl;
	std::cout << std::endl;
	std::cout << "currentFunc : ";
	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	{
	std::cout << ", \t" << FuncArrayRack[FRAME_func][i];
	}
	std::cout << std::endl;
	std::cout << "currentOrder : ";
	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	{
	std::cout << ", \t" << ArrayOrderRack[FRAME_func][i];
	}
	std::cout << std::endl;
	std::cout << std::endl;*/

	int DiscardFunc = -1;

	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	{
		if (FuncArrayRack[FRAME_func][i] == func && ArrayOrderRack[FRAME_func][i] == order)
		{
			DiscardFunc = i;
			break;
		}
	}
	if (DiscardFunc > -1)
	{
		short* tempOrder = new short[ArrayNumberRack[FRAME_func] - 1];
		void((**tempFunc)()) = new (void(*[ArrayNumberRack[FRAME_func] - 1])());
		ArrayNumberRack[FRAME_func]--;
		if (DiscardFunc != 0) {
			memcpy(tempOrder, ArrayOrderRack[FRAME_func], sizeof(short)* DiscardFunc);
			memcpy(tempFunc, FuncArrayRack[FRAME_func], sizeof(void(*)())*DiscardFunc);
		}
		if (DiscardFunc != ArrayNumberRack[FRAME_func])
		{
			memcpy(tempOrder + DiscardFunc, ArrayOrderRack[FRAME_func] + DiscardFunc + 1, sizeof(short)*(ArrayNumberRack[FRAME_func] - DiscardFunc));
			memcpy(tempFunc + DiscardFunc, FuncArrayRack[FRAME_func] + DiscardFunc + 1, sizeof(void(*)())*(ArrayNumberRack[FRAME_func] - DiscardFunc));
		}

		delete[] ArrayOrderRack[FRAME_func];
		delete[] FuncArrayRack[FRAME_func];
		ArrayOrderRack[FRAME_func] = tempOrder;
		FuncArrayRack[FRAME_func] = tempFunc;
	}

	//std::cout << "ChangeFunc : ";
	//for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	//{
	//	std::cout << ", \t" << FuncArrayRack[FRAME_func][i];
	//}
	//std::cout << std::endl;
	//std::cout << "ChangeOrder : ";
	//for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++)
	//{
	//	std::cout << ", \t" << ArrayOrderRack[FRAME_func][i];
	//}
	//std::cout << std::endl;
	//std::cout << "-------------------" << std::endl;
	//std::cout << std::endl;
}

void Frame::PushInitBeforeCreateFunc(void(*Initialize)(), short order){ PushFunc(FRAME_INIT_BEFORE, Initialize, order); }
void Frame::PushInitAfterCreateFunc(void(*Initialize)(), short order){ PushFunc(FRAME_INIT_AFTER, Initialize, order); }
void Frame::PushDisplayFunc(void(*Display)(), short order){ PushFunc(FRAME_DISPLAY, Display, order); }
void Frame::PushReshapeFunc(void(*Reshape)(), short order){ PushFunc(FRAME_RESHAPE, Reshape, order); }
void Frame::PushMouseClickFunc(void(*MouseClick)(), short order){ PushFunc(FRAME_MOUSE_CLICK, MouseClick, order); }
void Frame::PushMouseDragFunc(void(*MouseDrag)(), short order){ PushFunc(FRAME_MOUSE_MOTION, MouseDrag, order); }
void Frame::PushMouseMoveFunc(void(*MouseMove)(), short order){ PushFunc(FRAME_MOUSE_PASSIVE, MouseMove, order); }

void Frame::PopInitBeforeCreateFunc(void(*Initialize)(), short order){ PopFunc(FRAME_INIT_BEFORE, Initialize, order); }
void Frame::PopInitAfterCreateFunc(void(*Initialize)(), short order){ PopFunc(FRAME_INIT_AFTER, Initialize, order); }
void Frame::PopDisplayFunc(void(*Display)(), short order){ PopFunc(FRAME_DISPLAY, Display, order); }
void Frame::PopReshapeFunc(void(*Reshape)(), short order){ PopFunc(FRAME_RESHAPE, Reshape, order); }
void Frame::PopMouseClickFunc(void(*MouseClick)(), short order){ PopFunc(FRAME_MOUSE_CLICK, MouseClick, order); }
void Frame::PopMouseDragFunc(void(*MouseDrag)(), short order){ PopFunc(FRAME_MOUSE_MOTION, MouseDrag, order); }
void Frame::PopMouseMoveFunc(void(*MouseMove)(), short order){ PopFunc(FRAME_MOUSE_PASSIVE, MouseMove, order); }

void Frame::UpdateDisplay()
{
	if (bCreated) { glutPostWindowRedisplay(_windowHandle); }
}

char* Frame::GetWindowTitle()
{//이 함수에서 return으로 받은 값은 쓰고나서 직접 delete로 해제해 줄 필요가 있음
	char* temp = new char[strlen(_title)];
	return temp;
}
short Frame::GetWindowX()
{
	if (bCreated)
	{
		int tempSetWindowHandle = glutGetWindow();
		glutSetWindow(_windowHandle);
		_x = glutGet(GLUT_WINDOW_X);
		if (bSubWindow)
		{
			glutSetWindow(_ParentFrame->_windowHandle);
			_x -= glutGet(GLUT_WINDOW_X);
		}
		glutSetWindow(tempSetWindowHandle);
	}
	return _x;
}
short Frame::GetWindowY()
{
	if (bCreated)
	{
		int tempSetWindowHandle = glutGetWindow();
		glutSetWindow(_windowHandle);
		_y = glutGet(GLUT_WINDOW_Y);
		if (bSubWindow)
		{
			glutSetWindow(_ParentFrame->_windowHandle);
			_y -= glutGet(GLUT_WINDOW_Y);
		}
		glutSetWindow(tempSetWindowHandle);
	}
	return _y;
}
short Frame::GetWindowWidth(){ return _width; }
short Frame::GetWindowHeight(){ return _height; }
int Frame::GetWindow()
{
	if (!bCreated){ return NULL; }
	/*	else if (bSubWindow)
	{
	if (_ParentFrame == NULL){ std::cout << "부모 프레임을 찾을 수 없습니다. (NULL)" << std::endl; }
	else if ((_ParentFrame->GetWindow()) == NULL) { bCreated=false; return NULL; }
	}*/
	return _windowHandle;
}
bool Frame::GetWindowShown() { return bShown; }

void Frame::RunFuncArrayRack(int FRAME_func)
{
	for (int i = 0; i < ArrayNumberRack[FRAME_func]; i++){ (FuncArrayRack[FRAME_func][i])(); }
}

void Frame::_VirtualBeforeFunc()
{
	RunFuncArrayRack(FRAME_INIT_BEFORE);
}
void Frame::_VirtualAfterFunc()
{
	RunFuncArrayRack(FRAME_INIT_AFTER);
}
void Frame::_VirtualDisplayFunc()
{
	RunFuncArrayRack(FRAME_DISPLAY);

	if (0 == glutGet(GLUT_WINDOW_DOUBLEBUFFER)){/*단일 버퍼*/glFlush(); }
	else{ glutSwapBuffers(); }
}
void Frame::_VirtualReshapeFunc(int x, int y)
{
	_width = x; _height = y;
	RunFuncArrayRack(FRAME_RESHAPE);
}
void Frame::_VirtualMouseClickFunc(int button, int updown, int x, int y)
{
	UpdateMouseClick(button, updown, x, y);
	RunFuncArrayRack(FRAME_MOUSE_CLICK);
}
void Frame::_VirtualMouseMotionFunc(int x, int y)
{
	UpdateMouseDrag(x, y);
	RunFuncArrayRack(FRAME_MOUSE_MOTION);
}
void Frame::_VirtualMousePassiveFunc(int x, int y)
{
	UpdateMousePassiveDrag(x, y);
	RunFuncArrayRack(FRAME_MOUSE_PASSIVE);
}