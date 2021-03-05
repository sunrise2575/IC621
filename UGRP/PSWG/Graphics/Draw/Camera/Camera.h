/*//////////////////////////////

목적 : 그래픽 출력 시 시점 조작을 간편하고, 더욱 읽기 좋은 방향으로 할 수 있도록 함

담당 : 201411093 유진우

160801 카메라 클래스 완성 후 분화. 카메라 클래스 단독으로 버그 테스트 시작
160805 카메라 클래스 완성 후 디버깅 완료. 확장인 AdvenceCamera 제작 시작.
*///////////////////////////////

#pragma once

#include <Prelude.h>

#include _CH_VECTOR_
#include _CH_GLUT_

#define CAM_ISOMETRIC true
#define CAM_PERSPECTIVE false
#define CAM_CAMERA true
#define CAM_TARGET false

#define _RAD2DEG 57.295779// 180/PI
#define _DEG2RAD 0.017453 // PI/180


class Camera
{
	
private:

	////////////////////////////////////////////////////////////////////////
	//카메라 변수들

	double dFOV;		   //field of view(Vertical)*
	double dAspectRatio;   //가로세로비율 (가로/세로) *
	double dFocusDistance; //타겟과 카메라 본체 사이의 거리*

	double dFarClipDistance;//너무 멀어 카메라에 표시되지 않는 거리*
	double dNearClipDistance;//너무 가까워 카메라에 표시되지 않는 거리*

	bool bCamMode;//등각 투시인지 원근 투시인지 여부
	bool bRotationOrigin;//카메라 자체 회전인지 타겟 주위로 도는지 여부
	bool bFixedPoint;//FocusDistance 조절 시 고정되는 것이 타겟인지 카메라인지 여부. TARGET의 경우 카메라가 전진/후진함
	bool bRotationLimit;//카메라 회전 제한 여부 (true 면 제한)

	Vector3 MaxRotationLimit;//카메라 회전 제한 시 이 Max값이나 min 값보다 각도가 클 경우 
	Vector3 MinRotationLimit;//Max나 Min 값을 각도에 대입해버림.

	Vector3 X;//카메라 기준에서의 상대 x,y,z 방향 좌표
	Vector3 Y;
	Vector3 Z;

	Vector3 CamPos;//카메라 본체의 위치와 회전
	Vector3 CamRot;
	Vector3 TargetPos;//카메라가 포커싱하는 타겟 지점

	void UpdateCameraDegree();//카메라 중심에 타겟이 위치하도록 각도를 업데이트.
	void UpdateCorrectDegree();//카메라 회전 제한 및 기본 각도를 -360~360 사이로 제한.
	void UpdateCameraLocalCoord();//카메라 로컬 좌표계 x,y,z 업데이트

	////////////////////////////////////////////////////////////////////////
	//최적화 변수들

	bool bUpdateModelview;
	bool bUpdateProjection;

protected:
	////////////////////////////////////////////////////////////////////////
	//정보 출력 함수
	bool GetUpdateModelview();
	bool GetUpdateProjection();
	
	void SetUpdateModelview(bool Modelview);
	void SetUpdateProjection(bool Projection);

public:
	////////////////////////////////////////////////////////////////////////
	//생성 소멸 연산자
	Camera();
	//생성자 오버로딩 없이 생성 후 직접 초기화하는 방향으로 잡음.
	//만약 생성자에서 해결을 하려면 너무 오버로딩이 많고, 그리 편하지도 않을 것 같음.
	//따라 기본 세팅만 생성자에서 해두고, 커스텀은 생성 이후에 직접 하는 것으로 갈피를 잡았음.


	//~Camera(); 현재는 기본 소멸자 사용

	////////////////////////////////////////////////////////////////////////
	//정보 출력 함수
	bool GetViewMode();//등각인지 원근 투시인지 반환
	bool GetCameraRotationOrigin();//카메라의 회전 원점이 타겟인지 카메라인지 반환
	bool GetCameraFixedPoint();//포커스 거리 조절 시 움직이지 않는 원점
	bool GetCameraRotationLimit();//현재 회전 제한이 걸려있는지 여부	반환

	Vector3 GetCameraX();
	Vector3 GetCameraY();
	Vector3 GetCameraZ();

	Vector3 GetCameraPosition();//카메라 위치, 현재 초점 위치, 카메라 각도를 반환
	Vector3 GetCameraRotation();
	Vector3 GetTargetPosition();

	Vector3 GetMaxRotationLimit();//현재 카메라 제한 각도
	Vector3 GetMinRotationLimit();

	double GetFOV();//아래도 그와 동일, FOV, 가로/세로(비율),줌(초점과 카메라 간 거리),카메라의 표현 거리 반환
	double GetAspectRatio();
	double GetFocusDistance();
	double GetFarClipDistance();
	double GetNearClipDistance();


	////////////////////////////////////////////////////////////////////////
	//카메라 조정 함수

	void SetCameraRotationOrigin(bool Origin);
	void SetCameraRotation(bool Origin, float x, float y, float z);//카메라 회전을 조정하는 함수
	void AddCameraRotation(bool Origin, float x, float y, float z);
	void SetCameraRotation(bool Origin, Vector3 Rotation);
	void SetCameraRotation(float x, float y, float z);
	void AddCameraRotation(float x, float y, float z);
	void SetCameraRotation(Vector3 Rotation);
	
	void AddCameraLocalRotation(bool Origin, float x, float y, float z);
	void AddCameraLocalRotation(bool Origin, Vector3 Rotation);
	void AddCameraLocalRotation(Vector3 Rotation);
	void AddCameraLocalRotation(float x, float y, float z);
	
	void SetCameraRotationLimit(bool OnOff);
	void SetCameraRotationLimit(Vector3 min, Vector3 max);
	void SetCameraRotationLimit(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	void SetCameraRotationLimit(bool OnOff, Vector3 min, Vector3 max);
	void SetCameraRotationLimit(bool OnOff, float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	void SetCameraPosition(Vector3 Position);//Transition 함수의 경우 카메라와 타겟의 위치를 재설정.
	void SetTargetPosition(Vector3 Position);
	void SetAllPosition(bool Origin, Vector3 Position);//Origin 을 Position으로 이동시키되, 타겟과 카메라 모두 이동
	void AddCameraLocalPosition(Vector3 Position);//LocalTransition 함수의 경우 현재 카메라 기준의 로컬 좌표계로 앞뒤상하좌우로 움직임
	void SetCameraPosition(float x, float y, float z);
	void SetTargetPosition(float x, float y, float z);
	void SetAllPosition(bool Origin, float x, float y, float z);
	void AddCameraLocalPosition(float x, float y, float z);

	void SetFocusDistance(bool FixedPoint);
	void SetFocusDistance(bool FixedPoint, double dDistance);//타겟과 카메라 사이의 거리를 조절
	void AddFocusDistance(bool FixedPoint, double dDistance);
	void SetFocusDistance(int dDistance);
	void SetFocusDistance(double dDistance);//실제로는 타겟과 카메라 사이의 거리를 조절
	void AddFocusDistance(double dDistance);

	void SetClipDistance(double dNear, double dFar);//카메라 기본 설정값을 조절
	void SetAspectRatio(double Ratio);
	void SetAspectRatio(double width, double height);
	void SetViewMode(bool bDefineType);
	void SetFOV(double dInputFOV);
	
	


	  

	////////////////////////////////////////////////////////////////////////
	//그래픽 적용 함수
	void SetupAllMatrix(bool ForcedUpdate = false);
	void SetupModelviewMatrix();//적용 후는 현재 매트릭스가 GL_MODELVIEW가 됨
	void SetupProjectionMatrix();//적용 후는 현재 매트릭스가 GL_PROJECTION이 됨

};

void Camera::UpdateCameraDegree()
{
	CamRot.y = _RAD2DEG*atan2(CamPos.x - TargetPos.x, CamPos.z - TargetPos.z);
	CamRot.x = _RAD2DEG*atan2(TargetPos.y - CamPos.y, sqrt(pow(CamPos.x - TargetPos.x, 2) + pow(CamPos.z - TargetPos.z, 2)));
}
void Camera::UpdateCorrectDegree() {

	if (bRotationLimit)
	{
		if (MaxRotationLimit.x<CamRot.x)
		{
			CamRot.x = MaxRotationLimit.x;
		}
		else if (MinRotationLimit.x>CamRot.x)
		{
			CamRot.x = MinRotationLimit.x;
		}
		if (MaxRotationLimit.y<CamRot.y)
		{
			CamRot.y = MaxRotationLimit.y;
		}
		else if (MinRotationLimit.y>CamRot.y)
		{
			CamRot.y = MinRotationLimit.y;
		}
		if (MaxRotationLimit.z<CamRot.z)
		{
			CamRot.z = MaxRotationLimit.z;
		}
		else if (MinRotationLimit.z>CamRot.z)
		{
			CamRot.z = MinRotationLimit.z;
		}
	}

	CamRot.x -= ((int)CamRot.x / 360) * 360;
	CamRot.y -= ((int)CamRot.y / 360) * 360;
	CamRot.z -= ((int)CamRot.z / 360) * 360;
	// -360~360 사이로 각도를 만들어줌 ( 넘어가면 0으로 돌려줌 )

}
void Camera::UpdateCameraLocalCoord()
{
	Vector3 RadCamRot = CamRot * _DEG2RAD;
	//아래는 회전변환의 곱 ( Z X Y ) 행렬

	X = Vector3(
		+cos(RadCamRot.y)*cos(RadCamRot.z) + sin(RadCamRot.x)*sin(RadCamRot.y)*sin(RadCamRot.z),
		+cos(RadCamRot.x)*sin(RadCamRot.z),
		-sin(RadCamRot.y)*cos(RadCamRot.z) + sin(RadCamRot.x)*cos(RadCamRot.y)*sin(RadCamRot.z)
		);
	Y = Vector3(
		-cos(RadCamRot.y)*sin(RadCamRot.z) + sin(RadCamRot.x)*sin(RadCamRot.y)*cos(RadCamRot.z),
		+cos(RadCamRot.x)*cos(RadCamRot.z),
		+sin(RadCamRot.y)*sin(RadCamRot.z) + sin(RadCamRot.x)*cos(RadCamRot.y)*cos(RadCamRot.z)
		);
	Z = Vector3(
		+cos(RadCamRot.x)*sin(RadCamRot.y),
		-sin(RadCamRot.x),
		+cos(RadCamRot.x)*cos(RadCamRot.y)
		);

}
/////////////////////////////////////////////////////////
bool Camera::GetUpdateModelview(){ return bUpdateModelview; }
bool Camera::GetUpdateProjection(){ return bUpdateProjection; }
void Camera::SetUpdateModelview(bool Modelview){ bUpdateModelview = Modelview; }
void Camera::SetUpdateProjection(bool Projection){ bUpdateProjection = Projection; }
/////////////////////////////////////////////////////////
//생성, 소멸, 복사, 오퍼레이터

Camera::Camera()
{
	dFOV = 45;		   //field of view(Vertical)*
	dAspectRatio = 1;   //가로세로비율 (가로/세로) *
	dFocusDistance = 1; //타겟과 카메라 본체 사이의 거리*

	dFarClipDistance = 1000;//너무 멀어 카메라에 표시되지 않는 거리*
	dNearClipDistance = 1;//너무 가까워 카메라에 표시되지 않는 거리*

	bCamMode = CAM_PERSPECTIVE;//등각 투시인지 원근 투시인지 여부
	bRotationOrigin = CAM_TARGET;//카메라 자체 회전인지 타겟 주위로 도는지 여부
	bFixedPoint = CAM_TARGET;//FocusDistance 조절 시 고정되는 것이 타겟인지 카메라인지 여부. TARGET의 경우 카메라가 전진/후진함
	bRotationLimit = true;//카메라 회전 제한 여부 (true 면 제한)

	MaxRotationLimit = Vector3(90, 720, 0);//카메라 회전 제한 시 이 Max값이나 min 값보다 각도가 클 경우 
	MinRotationLimit = Vector3(-90, -720, 0);;//Max나 Min 값을 각도에 대입해버림.

	X = Vector3(1, 0, 0);//카메라 기준에서의 상대 x,y,z 방향 좌표
	Y = Vector3(0, 1, 0);
	Z = Vector3(0, 0, 1);

	CamPos = Vector3(0, 0, 10);//카메라 본체의 위치와 회전
	CamRot = Vector3(0, 0, 0);;
	TargetPos = Vector3(0, 0, 0);;//카메라가 포커싱하는 타겟 지점

	bUpdateModelview = true;
	bUpdateProjection = true;
}

/////////////////////////////////////////////////////////
//정보 출력 함수

double Camera::GetFOV() { return dFOV; }
double Camera::GetAspectRatio() { return dAspectRatio; }
double Camera::GetFocusDistance() { return dFocusDistance; }
double Camera::GetFarClipDistance() { return dFarClipDistance; }
double Camera::GetNearClipDistance() { return dNearClipDistance; }

bool Camera::GetViewMode() { return bCamMode; } //등각인지 원근 투시인지 반환
bool Camera::GetCameraRotationOrigin() { return bRotationOrigin; }
bool Camera::GetCameraFixedPoint() { return bFixedPoint; }
bool Camera::GetCameraRotationLimit() { return bRotationLimit; } //현재 회전 제한이 걸려있는지 여부	반환

Vector3 Camera::GetMaxRotationLimit() { return MaxRotationLimit; } //현재 카메라 제한 각도
Vector3 Camera::GetMinRotationLimit() { return MinRotationLimit; }

Vector3 Camera::GetCameraX() { return X; }
Vector3 Camera::GetCameraY() { return Y; }
Vector3 Camera::GetCameraZ() { return Z; }

Vector3 Camera::GetCameraPosition() { return CamPos; }
Vector3 Camera::GetCameraRotation() { return CamRot; }

Vector3 Camera::GetTargetPosition() { return TargetPos; }

/////////////////////////////////////////////////////////
void Camera::SetCameraRotationOrigin(bool Origin)
{
	bRotationOrigin = Origin;
}
void Camera::SetCameraRotation(bool Origin, Vector3 Rotation)
{
	bUpdateModelview = true;
	CamRot = Rotation;
	//회전값을 대입한다.

	UpdateCorrectDegree();
	UpdateCameraLocalCoord();

	if (Origin == CAM_CAMERA) // CAM_CAMERA
	{
		TargetPos = CamPos - (Z * dFocusDistance);
	}
	else if (Origin == CAM_TARGET) // CAM_TARGET
	{
		CamPos = TargetPos + (Z * dFocusDistance);
	}
}
void Camera::SetCameraRotation(bool Origin, float x, float y, float z)
{
	SetCameraRotation(Origin, Vector3(x, y, z));
}//CAM_CAMERA 또는 CAM_TARGET 을 이용해 회전 원점을 지정하고, x, y, z로 회전 각도를 지정(DEG)
void Camera::AddCameraRotation(bool Origin, float x, float y, float z)
{
	SetCameraRotation(Origin, CamRot + Vector3(x, y, z));
}

void Camera::SetCameraRotation(float x, float y, float z)
{
	SetCameraRotation(bRotationOrigin, Vector3(x, y, z));
}
void Camera::AddCameraRotation(float x, float y, float z)
{
	SetCameraRotation(bRotationOrigin, CamRot + Vector3(x, y, z));
}
void Camera::SetCameraRotation(Vector3 Rotation)
{
	SetCameraRotation(bRotationOrigin, Rotation);
}

void Camera::AddCameraLocalRotation(bool Origin, float x, float y, float z)
{
	AddCameraLocalRotation(Origin, Vector3(x, y, z));
}
void Camera::AddCameraLocalRotation(bool Origin, Vector3 Rotation)
{
	bUpdateModelview = true;
	//연산 구조
	//1. 현재의 로컬 좌표계 Camera(X,Y,Z)에 Vector3의 Rotation값을 가진 회전 행렬 Local(X,Y,Z)를 곱함
	//   New = Camera * Local
	//2. New 행렬에서 x축과 y축에 대한 오일러 각도를 구함.
	//3. z축 각도값 구하는 것은 다소 에러가 있어, 아래와 같이 구현
	//4. New 행렬에 2에서 구한 x, y축에 대한 오일러 각도의 회전 행렬의 역행렬 Local(X,Y,Z)을 곱함
	//	 ForZ = Local * New
	//5. ForZ 행렬에서 z축에 대한 오일러 각도를 구함 
	Vector3* X1; //행렬 연산 하드코딩이 귀찮아 만든 포인터 변수.
	Vector3* Y1;
	Vector3* Z1;
	//Vector3* X2 = &LocalX;
	Vector3* Y2;
	Vector3* Z2;

	Vector3 RadLocalRot;//여기 할당된 각도의
	Vector3 LocalX;//회전 변환 행렬 | LocalX , LocalY, LocalZ | (열벡터의 행)
	Vector3 LocalY;
	Vector3 LocalZ;

	float NewZRotation = 0;
	float NewYRotation = 0;
	float NewXRotation = 0;

	RadLocalRot = Rotation * _DEG2RAD;
	/*Vector3 LocalX = Vector3(
	+cos(RadLocalRot.y)*cos(RadLocalRot.z) + sin(RadLocalRot.x)*sin(RadLocalRot.y)*sin(RadLocalRot.z),
	+cos(RadLocalRot.x)*sin(RadLocalRot.z),
	-sin(RadLocalRot.y)*cos(RadLocalRot.z) + sin(RadLocalRot.x)*cos(RadLocalRot.y)*sin(RadLocalRot.z)
	);*/
	LocalY = Vector3(
		-cos(RadLocalRot.y)*sin(RadLocalRot.z) + sin(RadLocalRot.x)*sin(RadLocalRot.y)*cos(RadLocalRot.z),
		+cos(RadLocalRot.x)*cos(RadLocalRot.z),
		+sin(RadLocalRot.y)*sin(RadLocalRot.z) + sin(RadLocalRot.x)*cos(RadLocalRot.y)*cos(RadLocalRot.z)
		);
	LocalZ = Vector3(
		+cos(RadLocalRot.x)*sin(RadLocalRot.y),
		-sin(RadLocalRot.x),
		+cos(RadLocalRot.x)*cos(RadLocalRot.y)
		);

	X1 = &X;
	Y1 = &Y;
	Z1 = &Z;
	//Vector3* X2 = &LocalX;
	Y2 = &LocalY;
	Z2 = &LocalZ;

	/*Vector3 NewX = Vector3(
	X1->x*X2->x + Y1->x*X2->y + Z1->x*X2->z,
	X1->y*X2->x + Y1->y*X2->y + Z1->y*X2->z,
	X1->z*X2->x + Y1->z*X2->y + Z1->z*X2->z
	);*/
	Vector3 NewY = Vector3(
		X1->x*Y2->x + Y1->x*Y2->y + Z1->x*Y2->z,
		X1->y*Y2->x + Y1->y*Y2->y + Z1->y*Y2->z,
		X1->z*Y2->x + Y1->z*Y2->y + Z1->z*Y2->z
		);
	Vector3 NewZ = Vector3(
		X1->x*Z2->x + Y1->x*Z2->y + Z1->x*Z2->z,
		X1->y*Z2->x + Y1->y*Z2->y + Z1->y*Z2->z,
		X1->z*Z2->x + Y1->z*Z2->y + Z1->z*Z2->z
		);


	if (sqrt(NewZ.x*NewZ.x + NewZ.z*NewZ.z) != 0)
	{
		NewYRotation = atan2(NewZ.x, NewZ.z);
	}
	NewXRotation = atan2(-NewZ.y, sqrt(pow(NewZ.x, 2) + pow(NewZ.z, 2)));
	//NewZRotation = atan2(-NewX.y, NewY.y);

	//Z축 각도를 구하기 위해 역행렬을 만들고 곱함

	RadLocalRot = Vector3(-NewXRotation, -NewYRotation, 0);

	LocalX = Vector3(
		+cos(RadLocalRot.y),
		+sin(RadLocalRot.x)*sin(RadLocalRot.y),
		0//-cos(RadLocalRot.x)*sin(RadLocalRot.y)
		);
	LocalY = Vector3(
		0,
		cos(RadLocalRot.x),
		0//sin(RadLocalRot.x)
		);
	LocalZ = Vector3(
		+sin(RadLocalRot.y),
		-sin(RadLocalRot.x)*cos(RadLocalRot.y),
		0//+cos(RadLocalRot.x)*cos(RadLocalRot.y)
		);

	X1 = &LocalX;
	Y1 = &LocalY;
	Z1 = &LocalZ;
	Y2 = &NewY;

	Vector3 forZY = Vector3(
		X1->x*Y2->x + Y1->x*Y2->y + Z1->x*Y2->z,
		X1->y*Y2->x + Y1->y*Y2->y + Z1->y*Y2->z,
		0//X1->z*Y2->x + Y1->z*Y2->y + Z1->z*Y2->z
		);


	//NewZRotation = CamR;
	NewZRotation = atan2(-forZY.x, forZY.y);


	SetCameraRotation(Origin, _RAD2DEG*NewXRotation, _RAD2DEG*NewYRotation, _RAD2DEG*NewZRotation);// _RAD2DEG*NewZRotation);
}
void Camera::AddCameraLocalRotation(Vector3 Rotation)
{
	AddCameraLocalRotation(bRotationOrigin, Rotation);
}
void Camera::AddCameraLocalRotation(float x, float y, float z)
{
	AddCameraLocalRotation(bRotationOrigin, Vector3(x, y, z));
}

void Camera::SetCameraRotationLimit(bool OnOff)
{
	bUpdateModelview = true;

	bRotationLimit = OnOff;
	SetCameraRotation(bRotationOrigin, CamRot);
}
void Camera::SetCameraRotationLimit(Vector3 min, Vector3 max)
{
	bUpdateModelview = true;

	MinRotationLimit = min;
	MaxRotationLimit = max;
	SetCameraRotation(bRotationOrigin, CamRot);
}
void Camera::SetCameraRotationLimit(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	SetCameraRotationLimit(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
}
void Camera::SetCameraRotationLimit(bool OnOff, Vector3 min, Vector3 max)
{
	SetCameraRotationLimit(OnOff);
	SetCameraRotationLimit(min, max);
}
void Camera::SetCameraRotationLimit(bool OnOff, float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	SetCameraRotationLimit(OnOff);
	SetCameraRotationLimit(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
}

void Camera::SetAllPosition(bool Origin, Vector3 Position)
{
	bUpdateModelview = true;
	Vector3 deltaPos;
	if (Origin == CAM_TARGET)
	{
		deltaPos = (Position - TargetPos);
		TargetPos = Position;
		CamPos += deltaPos;
	}
	else if (Origin == CAM_CAMERA)
	{
		deltaPos = (Position - CamPos);
		CamPos = Position;
		TargetPos += deltaPos;
	}
}
void Camera::SetAllPosition(bool Origin, float x, float y, float z){ SetAllPosition(Origin, Vector3(x, y, z)); }

void Camera::SetCameraPosition(Vector3 Position)
{
	bUpdateModelview = true;

	CamPos = Position;

	UpdateCameraDegree();//타겟으로 카메라 회전
	UpdateCorrectDegree();//회전 각도 검사
	UpdateCameraLocalCoord();//로컬 좌표계 업데이트

	dFocusDistance = abs(TargetPos - CamPos);

}
void Camera::SetTargetPosition(Vector3 Position)
{
	bUpdateModelview = true;

	TargetPos = Position;

	UpdateCameraDegree();//타겟으로 카메라 회전
	UpdateCorrectDegree();//회전 각도 검사
	UpdateCameraLocalCoord();//로컬 좌표계 업데이트

	dFocusDistance = abs(TargetPos - CamPos);
}
void Camera::AddCameraLocalPosition(Vector3 Position)
{
	bUpdateModelview = true;

	Vector3 move = X*Position.x + Y*Position.y + Z*Position.z;

	CamPos += move;
	TargetPos += move;
}
void Camera::SetCameraPosition(float x, float y, float z)
{
	SetCameraPosition(Vector3(x, y, z));
}
void Camera::SetTargetPosition(float x, float y, float z)
{
	SetTargetPosition(Vector3(x, y, z));
}
void Camera::AddCameraLocalPosition(float x, float y, float z)
{
	AddCameraLocalPosition(Vector3(x, y, z));
}

void Camera::SetFocusDistance(bool FixedPoint)
{
	bFixedPoint = FixedPoint;
}
void Camera::SetFocusDistance(bool FixedPoint, double dDistance)
{
	bUpdateModelview = true;
	bUpdateProjection = true;
	dFocusDistance = dDistance;
	if (FixedPoint == CAM_TARGET)
	{
		CamPos = TargetPos + Z*dFocusDistance;
	}
	else if (FixedPoint == CAM_CAMERA)
	{
		TargetPos = CamPos + Z*dFocusDistance;
	}
}//타겟과 카메라 사이의 거리를 조절
void Camera::AddFocusDistance(bool FixedPoint, double dDistance)
{
	bUpdateModelview = true;
	bUpdateProjection = true;
	dFocusDistance += dDistance;
	if (FixedPoint == CAM_TARGET)
	{
		CamPos = TargetPos + Z*dFocusDistance;
	}
	else if (FixedPoint == CAM_CAMERA)
	{
		TargetPos = CamPos + Z*dFocusDistance;
	}
}
void Camera::SetFocusDistance(int dDistance)
{
	SetFocusDistance(bFixedPoint, dDistance);
}
void Camera::SetFocusDistance(double dDistance)
{
	SetFocusDistance(bFixedPoint, dDistance);
}//실제로는 타겟과 카메라 사이의 거리를 조절
void Camera::AddFocusDistance(double dDistance)
{
	AddFocusDistance(bFixedPoint, dDistance);
}

void Camera::SetClipDistance(double dNear, double dFar)
{
	bUpdateProjection = true;

	dNearClipDistance = dNear;
	dFarClipDistance = dFar;
}
void Camera::SetAspectRatio(double Ratio)
{
	bUpdateProjection = true;

	dAspectRatio = Ratio;
}
void Camera::SetAspectRatio(double width, double height)
{
	bUpdateProjection = true;

	dAspectRatio = width / height;
}
void Camera::SetViewMode(bool bDefineType)
{
	bUpdateProjection = true;

	bCamMode = bDefineType;
}//CAM_ISOMETRIC_VIEW 또는 CAM_PERSPECTIVE_VIEW
void Camera::SetFOV(double dInputFOV)
{
	bUpdateProjection = true;

	dFOV = dInputFOV;
}

////////////////////////////////////////////////////////
//그래픽 적용 함수
void Camera::SetupAllMatrix(bool ForcedUpdate)
{
	if (ForcedUpdate)
	{
		bUpdateModelview = true;
		bUpdateProjection = true;
	}
	SetupProjectionMatrix();
	SetupModelviewMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void Camera::SetupModelviewMatrix()
{
	if (bUpdateModelview)
	{
		bUpdateModelview = false;
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-CamRot.z, 0.0, 0.0, 1.0);
		glRotatef(-CamRot.x, 1.0, 0.0, 0.0);
		glRotatef(-CamRot.y, 0.0, 1.0, 0.0);
		glTranslatef(-CamPos.x, -CamPos.y, -CamPos.z);
	}
}
void Camera::SetupProjectionMatrix()
{
	if (bUpdateProjection)
	{
		bUpdateProjection = false;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (bCamMode == CAM_ISOMETRIC)
		{
			float tempSeeRange = dFocusDistance*tan(_DEG2RAD*dFOV / 2);
			glOrtho(-dAspectRatio*tempSeeRange, dAspectRatio*tempSeeRange, -tempSeeRange, +tempSeeRange, dNearClipDistance, dFarClipDistance);
		}
		else if (bCamMode == CAM_PERSPECTIVE)
		{
			gluPerspective(dFOV, dAspectRatio, dNearClipDistance, dFarClipDistance);
		}
	}
	//	glMatrixMode(GL_MODELVIEW);
}
