#pragma once

#include <Prelude.h>

#include _CH_CAMERA_
#include _CH_VECTOR_
#include _CH_GLUT_

#define CAM_ADV_SMOOTH true// 부드러운 전환을 사용
#define CAM_ADV_FIXED false// 그냥 대입함.

#define _CAM_ADV_POS_EPSILON 0.01
#define _CAM_ADV_POS_CHANGE_RATIO 0.2

#define _CAM_ADV_ROT_EPSILON 0.01
#define _CAM_ADV_ROT_CHANGE_RATIO 0.2

#define _CAM_ADV_FOV_EPSILON 1//(10*_CAM_ADV_FOV_CHANGE_RATIO)
#define _CAM_ADV_FOV_CHANGE_RATIO 0.2

//#define _CAM_RAD2DEG 57.295779// 180/PI
//#define _CAM_DEG2RAD 0.017453 // PI/180


class AdvencedCamera : protected Camera
{
private:
	////////////////////////////////////////////////////////////////////////
	//카메라 변수들

	bool bSmooth; // CAM_ADV_SMOOTH의 경우 부드러운 화면 전환, 아닐 경우 Camera와 같이 즉시 대입. 
	bool bDesiredCamMode;//등각 투시인지 원근 투시인지 여부
	bool bDesiredOrigin;//카메라 자체 회전인지 타겟 주위로 도는지 여부

	double dDesiredFOV;		   //field of view(Vertical)*
	double dDesiredAspectRatio;   //가로세로비율 (가로/세로) *
	double dDesiredFocusDistance; //타겟과 카메라 본체 사이의 거리*
	double dDesiredFarClipDistance;//너무 멀어 카메라에 표시되지 않는 거리*
	double dDesiredNearClipDistance;//너무 가까워 카메라에 표시되지 않는 거리*

	Vector3 DesiredCamPos;//카메라 본체의 위치와 회전
	Vector3 DesiredCamRot;
	Vector3 DesiredTargetPos;//카메라가 포커싱하는 타겟 지점
	
	Vector3 DesiredOriginPosCam;
	Vector3 DesiredOriginPosTarget;

	bool bUpdateFOV;// True일 경우 목표값과 실제값이 달라 값 갱신이 필요한 경우.
	bool bUpdateAspectRatio;
	bool bUpdateFocusDistance;
	bool bUpdateClipDistance;
	bool bUpdateViewMode;
	double dViewModeFOV;
	double dViewModeFocusDistance;

	bool bUpdateCamPos;
	bool bUpdateCamRot;
	bool bUpdateOriginPos;
	bool bUpdateTargetPos;

	//void UpdateAdvencedCameraStatus(); // 부드러운 이동에 필요한 값 갱신을 해주는 함수.
	void UpdateFOV();
	void UpdateClipDistance();
	void UpdateViewMode();
	void UpdateCameraRotation();
	void UpdateFocusDistance();
	void UpdateOriginPosition();
	void UpdateCameraPosition();
	void UpdateTargetPosition();

	void StopFocusDistance();
	void StopCameraRotation();
	void StopCameraPosition();
	void StopTargetPosition();
	void StopOriginPosition();

	
public:
	////////////////////////////////////////////////////////////////////////
	//생성 소멸 연산자
	AdvencedCamera();

	////////////////////////////////////////////////////////////////////////
	//정보 출력 함수
	bool GetSmoothMode();//카메라 부드러운 이동 기능이 켜져있는지 반환
	bool GetViewMode();//등각인지 원근 투시인지 반환
	bool GetCameraOrigin();//카메라의 회전 원점이 타겟인지 카메라인지 반환

	Vector3 GetCameraPosition();//일정 시간 뒤 도착할 카메라 위치, 현재 초점 위치, 카메라 각도를 반환
	Vector3 GetCameraRotation();
	Vector3 GetTargetPosition();
	Vector3 GetDesiredCameraPosition();//일정 시간 뒤 도착할 카메라 위치, 현재 초점 위치, 카메라 각도를 반환
	Vector3 GetDesiredCameraRotation();
	Vector3 GetDesiredTargetPosition();
	Vector3 GetCameraX();
	Vector3 GetCameraY();
	Vector3 GetCameraZ();
	Vector3 GetMaxRotationLimit();//현재 카메라 제한 각도
	Vector3 GetMinRotationLimit();
	
	double GetFOV();//아래도 그와 동일, FOV, 가로/세로(비율),줌(초점과 카메라 간 거리),카메라의 표현 거리 반환
	double GetAspectRatio();
	double GetFocusDistance();
	double GetFarClipDistance();
	double GetNearClipDistance();
	////////////////////////////////////////////////////////////////////////
	//카메라 조정 함수

	void SetSmoothMode(bool SmoothMode);
	void SetCameraOrigin(bool Origin);
	void StopMovingCamera();//카메라를 현재 상태로 정지

	void AddCameraLocalPosition(Vector3 Position);//LocalTransition 함수의 경우 현재 카메라 기준의 로컬 좌표계로 앞뒤상하좌우로 움직임
	void AddCameraLocalPosition(float x, float y, float z);

	void AddCameraLocalRotation(Vector3 Rotation);
	void AddCameraLocalRotation(float x, float y, float z);

	void AddFocusDistance(double dDistance);

	void SetCameraRotation(Vector3 Rotation);
	void SetCameraRotation(float x, float y, float z);
	void AddCameraRotation(float x, float y, float z);
	
	void SetCameraRotationLimit(bool OnOff);
	void SetCameraRotationLimit(Vector3 min, Vector3 max);
	void SetCameraRotationLimit(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	void SetCameraPosition(Vector3 Position);//Transition 함수의 경우 카메라와 타겟의 위치를 재설정.
	void SetCameraPosition(float x, float y, float z);
	void SetTargetPosition(Vector3 Position);
	void SetTargetPosition(float x, float y, float z);
	void SetOriginPosition(Vector3 Position);
	void SetOriginPosition(float x, float y, float z);

	void SetFocusDistance(double dDistance);//실제로는 타겟과 카메라 사이의 거리를 조절


	void SetClipDistance(double dNear, double dFar);//카메라 기본 설정값을 조절
	void SetAspectRatio(double Ratio);
	void SetAspectRatio(double width, double height);
	void SetViewMode(bool bDefineType);
	void SetFOV(double dInputFOV);

	/////////////////////////////////////
	//상태 업데이트 함수

	void UpdateAdvencedCameraStatus(); // 부드러운 이동에 필요한 값 갱신을 해주는 함수.

	////////////////////////////////////////////////////////////////////////
	//그래픽 적용 함수

	/////////////////////////////////////////////////////////////////
	
	void SetupCamera();
	void SetupAllMatrix(bool ForcedUpdate = false);//카메라의 갱신과 모든 매트릭스의 셋업을 관할
	void SetupModelviewMatrix();//적용 후는 현재 매트릭스가 GL_MODELVIEW가 됨
	void SetupProjectionMatrix();
};


////////////////////////////////////////////////////////////////////////
//생성 소멸 연산자

AdvencedCamera::AdvencedCamera()
{
	SetCameraRotationLimit(false);

	bSmooth = true;
	bDesiredCamMode = GetViewMode();//등각 투시인지 원근 투시인지 여부
	bDesiredOrigin = GetCameraRotationOrigin();//카메라 자체 회전인지 타겟 주위로 도는지 여부

	dDesiredFOV = GetFOV();		   //field of view(Vertical)*
	dDesiredAspectRatio = GetAspectRatio();   //가로세로비율 (가로/세로) *
	dDesiredFocusDistance = GetFocusDistance(); //타겟과 카메라 본체 사이의 거리*
	dDesiredFarClipDistance = GetFarClipDistance();//너무 멀어 카메라에 표시되지 않는 거리*
	dDesiredNearClipDistance = GetNearClipDistance();//너무 가까워 카메라에 표시되지 않는 거리*

	DesiredCamPos = GetCameraPosition();//카메라 본체의 위치와 회전
	DesiredCamRot = GetCameraRotation();
	DesiredTargetPos = GetTargetPosition();//카메라가 포커싱하는 타겟 지점

	bUpdateFOV = false;// True일 경우 목표값과 실제값이 달라 값 갱신이 필요한 경우.
	bUpdateAspectRatio = false;
	bUpdateFocusDistance = false;
	bUpdateClipDistance = false;
	bUpdateViewMode = false;
	dViewModeFOV = 0; // View Mode의 자연스러운 전환을 위해 사용될 변수
	dViewModeFocusDistance = 0;

	bUpdateCamPos = false;
	bUpdateCamRot = false;
	bUpdateOriginPos = false;
	bUpdateTargetPos = false;

}

////////////////////////////////////////////////////////////////////////
//정보 출력 함수

bool AdvencedCamera::GetSmoothMode() { return bSmooth; };
bool AdvencedCamera::GetViewMode() { return bDesiredCamMode; }//등각인지 원근 투시인지 반환
bool AdvencedCamera::GetCameraOrigin() { return bDesiredOrigin; }//카메라의 회전 원점이 타겟인지 카메라인지 반환

Vector3 AdvencedCamera::GetCameraPosition() { return static_cast<Camera*>(this)->GetCameraPosition(); };//카메라 위치, 현재 초점 위치, 카메라 각도를 반환
Vector3 AdvencedCamera::GetCameraRotation() { return static_cast<Camera*>(this)->GetCameraRotation(); };
Vector3 AdvencedCamera::GetTargetPosition() { return static_cast<Camera*>(this)->GetTargetPosition(); };
/*
Vector3 AdvencedCamera::GetDesiredCameraPosition()
{
if (bUpdateCamPos){return DesiredCamPos;}
return GetCameraPosition();
};//카메라 위치, 현재 초점 위치, 카메라 각도를 반환
Vector3 AdvencedCamera::GetDesiredCameraRotation()
{
if (bUpdateCamRot){ return DesiredCamRot; }
return GetCameraRotation();
};
Vector3 AdvencedCamera::GetDesiredTargetPosition()
{
if (bUpdateTargetPos){ return DesiredTargetPos; }
return GetTargetPosition();
};*/
Vector3 AdvencedCamera::GetCameraX(){ return static_cast<Camera*>(this)->GetCameraX(); };
Vector3 AdvencedCamera::GetCameraY(){ return static_cast<Camera*>(this)->GetCameraY(); };
Vector3 AdvencedCamera::GetCameraZ(){ return static_cast<Camera*>(this)->GetCameraZ(); };
Vector3 AdvencedCamera::GetMaxRotationLimit() { return static_cast<Camera*>(this)->GetMaxRotationLimit(); };//현재 카메라 제한 각도
Vector3 AdvencedCamera::GetMinRotationLimit() { return static_cast<Camera*>(this)->GetMinRotationLimit(); };

double AdvencedCamera::GetFOV() { return static_cast<Camera*>(this)->GetFOV(); };//아래도 그와 동일, FOV, 가로/세로(비율),줌(초점과 카메라 간 거리),카메라의 표현 거리 반환
double AdvencedCamera::GetAspectRatio() { return static_cast<Camera*>(this)->GetAspectRatio(); };
double AdvencedCamera::GetFocusDistance() { return static_cast<Camera*>(this)->GetFocusDistance(); };
double AdvencedCamera::GetFarClipDistance() { return static_cast<Camera*>(this)->GetFarClipDistance(); };
double AdvencedCamera::GetNearClipDistance() { return static_cast<Camera*>(this)->GetNearClipDistance(); };

///////////////////////////////////////////////////////////////////
//움직임 정지 함수
void AdvencedCamera::StopMovingCamera()
{
	StopFocusDistance();

	StopCameraRotation();
	StopOriginPosition();

	StopCameraPosition();
	StopTargetPosition();
}
void AdvencedCamera::StopFocusDistance()
{
	bUpdateFocusDistance = false;
	dDesiredFocusDistance = GetFocusDistance();
}
void AdvencedCamera::StopCameraRotation()
{
	bUpdateCamRot = false;
	DesiredCamRot = GetCameraRotation();
}
void AdvencedCamera::StopCameraPosition()
{
	bUpdateCamPos = false;
	DesiredCamPos = GetCameraPosition();
}
void AdvencedCamera::StopTargetPosition()
{
	bUpdateTargetPos = false;
	DesiredTargetPos = GetTargetPosition();
}
void AdvencedCamera::StopOriginPosition()
{
	bUpdateFocusDistance = false;
	DesiredOriginPosCam = GetCameraPosition();
	DesiredOriginPosTarget = GetTargetPosition();
}

////////////////////////////////////////////////////////////////////////
//카메라 조정 함수
void AdvencedCamera::SetSmoothMode(bool SmoothMode){ bSmooth = SmoothMode; }
void AdvencedCamera::SetCameraOrigin(bool Origin){ bDesiredOrigin = Origin; }


void AdvencedCamera::AddCameraLocalPosition(Vector3 Position)
{
	StopMovingCamera();
	static_cast<Camera*>(this)->AddCameraLocalPosition(Position);
	DesiredCamPos = GetCameraPosition();
}//LocalTransition 함수의 경우 현재 카메라 기준의 로컬 좌표계로 앞뒤상하좌우로 움직임
void AdvencedCamera::AddCameraLocalPosition(float x, float y, float z){ AddCameraLocalPosition(Vector3(x, y, z)); }

void AdvencedCamera::AddCameraLocalRotation(Vector3 Rotation)
{
	StopMovingCamera();
	static_cast<Camera*>(this)->AddCameraLocalRotation(bDesiredOrigin, Rotation);
	DesiredCamRot = GetCameraRotation();
}
void AdvencedCamera::AddCameraLocalRotation(float x, float y, float z){ AddCameraLocalRotation(Vector3(x, y, z)); }

void AdvencedCamera::AddFocusDistance(double dDistance)
{
	StopMovingCamera();
	static_cast<Camera*>(this)->SetFocusDistance(bDesiredOrigin);
	/*if (bSmooth == CAM_ADV_SMOOTH)
	{
	bUpdateFocusDistance = true;
	dDesiredFocusDistance += dDistance;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{*/
	bUpdateFocusDistance = false;
	static_cast<Camera*>(this)->AddFocusDistance(dDistance);
	dDesiredFocusDistance = GetFocusDistance();
	//}
}


void AdvencedCamera::SetCameraRotation(Vector3 Rotation_input)
{
	static_cast<Camera*>(this)->SetCameraRotationOrigin(bDesiredOrigin);
	Vector3 Rotation = Rotation_input;
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateCamRot = true;
		//DesiredCamRot = Rotation;

		/////
		//아래 코드는 Rotation 할당 시 최대한 적은 거리로 목표에 근접하게 움직일 수 있는 DesiredCamRot을 계산
		Vector3 CurrentRot = GetCameraRotation();
		Vector3 DeltaRot = (Rotation - CurrentRot);
		Rotation.x -= (((int)(DeltaRot.x) / 360) * 360);
		Rotation.y -= ((int)(DeltaRot.y) / 360) * 360;
		Rotation.z -= ((int)(DeltaRot.z) / 360) * 360;
		if (DeltaRot.x > 0) { Rotation.x -= 360; }
		if (DeltaRot.y > 0) { Rotation.y -= 360; }
		if (DeltaRot.z > 0) { Rotation.z -= 360; }/// Rotation ------Current Rotation ------ Rotation2
		Vector3 Rotation2 = Rotation + 360;
		if (GetCameraRotationLimit() == true)
		{
			Vector3 LimitMin = GetMinRotationLimit();
			Vector3 LimitMax = GetMaxRotationLimit();
			bool minLimit;
			bool maxLimit;
			/////////////////////////////////////////////////////////////
			minLimit = LimitMin.x < Rotation.x;
			maxLimit = LimitMax.x > Rotation2.x;
			if (minLimit == maxLimit)
			{
				if (minLimit)
				{//minLimit < rotation  < currntrot < rotation2 < MaxLimit
					if (CurrentRot.x - Rotation.x < 180){ DesiredCamRot.x = Rotation.x; }
					else{ DesiredCamRot.x = Rotation2.x; }
				}
				else
				{//rotation < minLimit  < currntrot < MaxLimit < rotation2
					if (LimitMin.x - Rotation.x < Rotation2.x - LimitMax.x)	{ DesiredCamRot.x = LimitMin.x; }
					else{ DesiredCamRot.x = LimitMax.x; }
				}
			}
			else
			{//제한을 넘지 않은 값으로 할당
				if (minLimit){ DesiredCamRot.x = Rotation.x; }
				else{ DesiredCamRot.x = Rotation2.x; }
			}
			//////////////////////////////////////////////////////////////
			//y와 x에 대해 위와 동일하게 실행
			minLimit = LimitMin.y < Rotation.y;
			maxLimit = LimitMax.y > Rotation2.y;
			if (minLimit == maxLimit)
			{
				if (minLimit)
				{//minLimit < rotation  < currntrot < rotation2 < MaxLimit
					if (CurrentRot.y - Rotation.y < 180){ DesiredCamRot.y = Rotation.y; }
					else{ DesiredCamRot.y = Rotation2.y; }
				}
				else
				{//rotation < minLimit  < currntrot < MaxLimit < rotation2
					if (LimitMin.y - Rotation.y < Rotation2.y - LimitMax.y)	{ DesiredCamRot.y = LimitMin.y; }
					else{ DesiredCamRot.y = LimitMax.y; }
				}
			}
			else
			{//제한을 넘지 않은 값으로 할당
				if (minLimit){ DesiredCamRot.y = Rotation.y; }
				else{ DesiredCamRot.y = Rotation2.y; }
			}

			minLimit = LimitMin.z < Rotation.z;
			maxLimit = LimitMax.z > Rotation2.z;
			if (minLimit == maxLimit)
			{
				if (minLimit)
				{//minLimit < rotation  < currntrot < rotation2 < MaxLimit
					if (CurrentRot.z - Rotation.z < 180){ DesiredCamRot.z = Rotation.z; }
					else{ DesiredCamRot.z = Rotation2.z; }
				}
				else
				{//rotation < minLimit  < currntrot < MaxLimit < rotation2
					if (LimitMin.z - Rotation.z < Rotation2.z - LimitMax.z)	{ DesiredCamRot.z = LimitMin.z; }
					else{ DesiredCamRot.z = LimitMax.z; }
				}
			}
			else
			{//제한을 넘지 않은 값으로 할당
				if (minLimit){ DesiredCamRot.z = Rotation.z; }
				else{ DesiredCamRot.z = Rotation2.z; }
			}

		}
		else
		{
			if (CurrentRot.x - Rotation.x < 180){ DesiredCamRot.x = Rotation.x; }
			else{ DesiredCamRot.x = Rotation2.x; }

			if (CurrentRot.y - Rotation.y < 180){ DesiredCamRot.y = Rotation.y; }
			else{ DesiredCamRot.y = Rotation2.y; }

			if (CurrentRot.z - Rotation.z < 180){ DesiredCamRot.z = Rotation.z; }
			else{ DesiredCamRot.z = Rotation2.z; }
		}
		int temp2pi;
		temp2pi = ((int)(DesiredCamRot.x) / 360);
		if (DesiredCamRot.x >= 360) {
			static_cast<Camera*>(this)->AddCameraRotation(-360 * temp2pi, 0, 0);
			DesiredCamRot.x -= 360 * temp2pi;
		}
		else if (DesiredCamRot.x <= -360)
		{
			static_cast<Camera*>(this)->AddCameraRotation(-360 * temp2pi, 0, 0);
			DesiredCamRot.x -= 360 * temp2pi;
		}
		temp2pi = ((int)(DesiredCamRot.y) / 360);
		if (DesiredCamRot.y >= 360) {
			static_cast<Camera*>(this)->AddCameraRotation(0, -360 * temp2pi, 0);
			DesiredCamRot.y -= 360 * temp2pi;
		}
		else if (DesiredCamRot.y <= -360)
		{
			static_cast<Camera*>(this)->AddCameraRotation(0, -360 * temp2pi, 0);
			DesiredCamRot.y -= 360 * temp2pi;
		}
		temp2pi = ((int)(DesiredCamRot.z) / 360);
		if (DesiredCamRot.z >= 360) {
			static_cast<Camera*>(this)->AddCameraRotation(0, 0, -360 * temp2pi);
			DesiredCamRot.z -= 360 * temp2pi;
		}
		else if (DesiredCamRot.z <= -360)
		{
			static_cast<Camera*>(this)->AddCameraRotation(0, 0, -360 * temp2pi);
			DesiredCamRot.z -= 360 * temp2pi;
		}

	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateCamRot = false;
		static_cast<Camera*>(this)->SetCameraRotation(Rotation);
		DesiredCamRot = Rotation;
	}

	StopCameraPosition();
	StopTargetPosition();
};
void AdvencedCamera::SetCameraRotation(float x, float y, float z){ SetCameraRotation(Vector3(x, y, z)); };
void AdvencedCamera::AddCameraRotation(float x, float y, float z)
{
	static_cast<Camera*>(this)->SetCameraRotationOrigin(bDesiredOrigin);
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateCamRot = true;
		SetCameraRotation(DesiredCamRot + Vector3(x, y, z));
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateCamRot = false;
		static_cast<Camera*>(this)->AddCameraRotation(x, y, z);
		DesiredCamRot = GetCameraRotation();
	}
	StopCameraPosition();
	StopTargetPosition();
}




void AdvencedCamera::SetCameraRotationLimit(bool OnOff)
{
	static_cast<Camera*>(this)->SetCameraRotationLimit(OnOff);
	SetCameraRotationLimit(GetMinRotationLimit(), GetMaxRotationLimit());
	if (bUpdateCamRot) { SetCameraRotation(DesiredCamRot); }
}
void AdvencedCamera::SetCameraRotationLimit(Vector3 min, Vector3 max)
{
	Vector3 DeltaRot = max - min;
	//if (DeltaRot.x > 360){ max.x -= (((int)(DeltaRot.x)) / 360) * 360; }
	//if (DeltaRot.y > 360){ max.y -= (((int)(DeltaRot.y)) / 360) * 360; }
	//if (DeltaRot.z > 360){ max.z -= (((int)(DeltaRot.z)) / 360) * 360; }
	//DeltaRot = max - min;
	if (max.x > 360){ max.x -= (((int)(max.x)) / 360) * 360; }
	if (max.y > 360){ max.y -= (((int)(max.y)) / 360) * 360; }
	if (max.z > 360){ max.z -= (((int)(max.z)) / 360) * 360; }
	if (max.x < 0){ max.x += 360; }
	if (max.y < 0){ max.y += 360; }
	if (max.z < 0){ max.z += 360; }
	min = max - DeltaRot;
	if (DeltaRot.x >= 360) { max.x = 720; min.x = -720; }
	if (DeltaRot.y >= 360) { max.y = 720; min.y = -720; }
	if (DeltaRot.z >= 360) { max.z = 720; min.z = -720; }
	//위 과정으로 min~max는 -360~360 사이 범위. 이제 각도만 잘 재할당해주면 자연스럽게 각도 제한 가능
	//카메라 각도를 가능한 한 제한 범위 내로 넣으려 시도하고, 안되면 그나마 이동 거리가 짧은 쪽으로 할당
	Vector3 NewCameraRotation = GetCameraRotation();
	//Vector3 DeltaRot; = max - min; 재활용. 임시값을 저장하는 용도로 아래에서 사용할 생각
	if (NewCameraRotation.x < 0){ NewCameraRotation.x += 360; }
	if (NewCameraRotation.y < 0){ NewCameraRotation.y += 360; }
	if (NewCameraRotation.z < 0){ NewCameraRotation.z += 360; }

	if (NewCameraRotation.x > max.x)
	{
		DeltaRot.x = NewCameraRotation.x - max.x;
		NewCameraRotation.x -= 360;
		if (NewCameraRotation.x < min.x)
		{
			if (DeltaRot.x < min.x - NewCameraRotation.x){ NewCameraRotation.x = max.x; }
			else{ NewCameraRotation.x = min.x; }
		}
	}
	if (NewCameraRotation.x < min.x)
	{
		DeltaRot.x = min.x - NewCameraRotation.x;
		NewCameraRotation.x += 360;
		if (DeltaRot.x > NewCameraRotation.x - max.x){ NewCameraRotation.x = max.x; }
		else{ NewCameraRotation.x = min.x; }
	}

	if (NewCameraRotation.y > max.y)
	{
		DeltaRot.y = NewCameraRotation.y - max.y;
		NewCameraRotation.y -= 360;
		if (NewCameraRotation.y < min.y)
		{
			if (DeltaRot.y < min.y - NewCameraRotation.y){ NewCameraRotation.y = max.y; }
			else{ NewCameraRotation.y = min.y; }
		}
	}
	if (NewCameraRotation.y < min.y)
	{
		DeltaRot.y = min.y - NewCameraRotation.y;
		NewCameraRotation.y += 360;
		if (DeltaRot.y > NewCameraRotation.y - max.y){ NewCameraRotation.y = max.y; }
		else{ NewCameraRotation.y = min.y; }
	}

	if (NewCameraRotation.z > max.z)
	{
		DeltaRot.z = NewCameraRotation.z - max.z;
		NewCameraRotation.z -= 360;
		if (NewCameraRotation.z < min.z)
		{
			if (DeltaRot.z < min.z - NewCameraRotation.z){ NewCameraRotation.z = max.z; }
			else{ NewCameraRotation.z = min.z; }
		}
	}
	if (NewCameraRotation.z < min.z)
	{
		DeltaRot.z = min.z - NewCameraRotation.z;
		NewCameraRotation.z += 360;
		if (DeltaRot.z > NewCameraRotation.z - max.z){ NewCameraRotation.z = max.z; }
		else{ NewCameraRotation.z = min.z; }
	}

	static_cast<Camera*>(this)->SetCameraRotation(NewCameraRotation);
	static_cast<Camera*>(this)->SetCameraRotationLimit(min, max);
}
void AdvencedCamera::SetCameraRotationLimit(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	SetCameraRotationLimit(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
}

void AdvencedCamera::SetCameraPosition(Vector3 Position)
{
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateCamPos = true;
		DesiredCamPos = Position;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateCamPos = false;
		static_cast<Camera*>(this)->SetCameraPosition(Position);
		DesiredCamPos = Position;
	}
	StopFocusDistance();
	StopCameraRotation();
	StopOriginPosition();
}//Transition 함수의 경우 카메라와 타겟의 위치를 재설정.
void AdvencedCamera::SetTargetPosition(Vector3 Position)
{
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateTargetPos = true;
		DesiredTargetPos = Position;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateTargetPos = false;
		static_cast<Camera*>(this)->SetTargetPosition(Position);
		DesiredTargetPos = Position;
	}
	StopFocusDistance();
	StopCameraRotation();
	StopOriginPosition();
}
void AdvencedCamera::SetCameraPosition(float x, float y, float z) { SetCameraPosition(Vector3(x, y, z)); }
void AdvencedCamera::SetTargetPosition(float x, float y, float z) { SetTargetPosition(Vector3(x, y, z)); }

void AdvencedCamera::SetOriginPosition(Vector3 Position)
{//현재 Z축 회전으로 인해 자연스러운 이동이 안되는 문제점이 있음

	Vector3 BetweenPos = GetTargetPosition() - GetCameraPosition();
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateOriginPos = true;
		if (bDesiredOrigin == CAM_CAMERA)
		{
			DesiredOriginPosCam = Position;
			DesiredOriginPosTarget = Position + BetweenPos;
		}
		else if (bDesiredOrigin == CAM_TARGET)
		{
			DesiredOriginPosCam = Position - BetweenPos;
			DesiredOriginPosTarget = Position;
		}
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateTargetPos = false;
		if (bDesiredOrigin == CAM_TARGET)
		{
			SetTargetPosition(Position);
			SetCameraPosition(Position - BetweenPos);
		}
		else if (bDesiredOrigin == CAM_CAMERA)
		{
			SetTargetPosition(Position + BetweenPos);
			SetCameraPosition(Position);
		}
	}
	StopCameraPosition();
	StopTargetPosition();
}
void AdvencedCamera::SetOriginPosition(float x, float y, float z){ SetOriginPosition(Vector3(x, y, z)); }


void AdvencedCamera::SetFocusDistance(double dDistance)
{
	static_cast<Camera*>(this)->SetFocusDistance(bDesiredOrigin);
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateFocusDistance = true;
		dDesiredFocusDistance = dDistance;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateFocusDistance = false;
		dDesiredFocusDistance = dDistance;
		static_cast<Camera*>(this)->SetFocusDistance(dDistance);
	}
	StopCameraPosition();
	StopTargetPosition();
}//타겟과 카메라 사이의 거리를 조절


void AdvencedCamera::SetClipDistance(double dNear, double dFar)
{
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateClipDistance = true;
		dDesiredNearClipDistance = dNear;
		dDesiredFarClipDistance = dFar;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateClipDistance = false;
		static_cast<Camera*>(this)->SetClipDistance(dNear, dFar);
		dDesiredNearClipDistance = dNear;
		dDesiredFarClipDistance = dFar;
	}
}//카메라 기본 설정값을 조절
void AdvencedCamera::SetAspectRatio(double Ratio)
{
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateAspectRatio = true;
		dDesiredAspectRatio = Ratio;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateAspectRatio = false;
		static_cast<Camera*>(this)->SetAspectRatio(Ratio);
		dDesiredAspectRatio = Ratio;
	}
}
void AdvencedCamera::SetAspectRatio(double width, double height){ SetAspectRatio(width / height); }
void AdvencedCamera::SetViewMode(bool bDefineType)
{
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateViewMode = true;
		bDesiredCamMode = bDefineType;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateViewMode = false;
		bDesiredCamMode = bDefineType;
		static_cast<Camera*>(this)->SetViewMode(bDefineType);
	}
}
void AdvencedCamera::SetFOV(double dInputFOV)
{
	if (bSmooth == CAM_ADV_SMOOTH)
	{
		bUpdateFOV = true;
		dDesiredFOV = dInputFOV;
	}
	else if (bSmooth == CAM_ADV_FIXED)
	{
		bUpdateFOV = false;
		dDesiredFOV = dInputFOV;
		static_cast<Camera*>(this)->SetFOV(dInputFOV);
	}
}

void AdvencedCamera::UpdateFOV()
{
	if (bUpdateFOV)
	{
		if (abs(GetFOV() - dDesiredFOV) < _CAM_ADV_FOV_EPSILON)
		{
			static_cast<Camera*>(this)->SetFOV(dDesiredFOV);
			bUpdateFOV = false;
		}
		else{ static_cast<Camera*>(this)->SetFOV(_CAM_ADV_FOV_CHANGE_RATIO*dDesiredFOV + (1 - _CAM_ADV_FOV_CHANGE_RATIO)*GetFOV()); }
	}
}
void AdvencedCamera::UpdateClipDistance()
{
	if (bUpdateClipDistance)
	{
		if (
			abs(GetNearClipDistance() - dDesiredNearClipDistance)
			+ abs(GetFarClipDistance() - dDesiredFarClipDistance) < _CAM_ADV_POS_EPSILON * 2
			)
		{
			static_cast<Camera*>(this)->SetClipDistance(dDesiredNearClipDistance, dDesiredFarClipDistance);
			bUpdateClipDistance = false;
		}
		else
		{
			static_cast<Camera*>(this)->SetClipDistance
				(
				(_CAM_ADV_POS_CHANGE_RATIO*dDesiredNearClipDistance + (1 - _CAM_ADV_POS_CHANGE_RATIO)*GetNearClipDistance()),
				(_CAM_ADV_POS_CHANGE_RATIO*dDesiredFarClipDistance + (1 - _CAM_ADV_POS_CHANGE_RATIO)*GetFarClipDistance())
				);
		}
	}
}
void AdvencedCamera::UpdateViewMode()
{
	if (bUpdateViewMode)
	{
		//Iso -> Per Per -> Per
		if (bDesiredCamMode == CAM_PERSPECTIVE)
		{
			if (static_cast<Camera*>(this)->GetViewMode() == CAM_ISOMETRIC)
			{
				dViewModeFOV = GetFOV();
				dViewModeFOV *= (1 - _CAM_ADV_FOV_CHANGE_RATIO);
				static_cast<Camera*>(this)->SetViewMode(CAM_PERSPECTIVE);
			}
			if (dViewModeFOV < _CAM_ADV_FOV_EPSILON)
			{
				dViewModeFOV = 0;
				bUpdateViewMode = false;
			}
			else
			{
				dViewModeFOV *= (1 - _CAM_ADV_FOV_CHANGE_RATIO);
				//dViewModeFocusDistance *  tan((GetFOV()-dViewModeFOV) = dDesiredFocusDistance * (tan(dDesiredFOV / 2 * _CAM_DEG2RAD)여야 타겟 크기 변화 없이 view 변경 가능
			}
		}
		else//Per -> Iso
		{
			if (static_cast<Camera*>(this)->GetViewMode() == CAM_ISOMETRIC)
			{
				bUpdateViewMode = false;
			}
			else if (abs(GetFOV() - dViewModeFOV) < _CAM_ADV_FOV_EPSILON)
			{
				dViewModeFOV = 0;
				bUpdateViewMode = false;
				static_cast<Camera*>(this)->SetViewMode(CAM_ISOMETRIC);
			}
			else
			{
				//커지는게 맞음
				//Fov = dFov - dViewmodeFov
				dViewModeFOV = GetFOV() * _CAM_ADV_FOV_CHANGE_RATIO + dViewModeFOV * (1 - _CAM_ADV_FOV_CHANGE_RATIO);
			}
		}

		if (bUpdateViewMode == true)
		{
			dViewModeFocusDistance = dDesiredFocusDistance * (tan(dDesiredFOV / 2 * _DEG2RAD) / tan((GetFOV() - dViewModeFOV) / 2 * _DEG2RAD) - 1);
		}
		else
		{
			dViewModeFocusDistance = 0;
		}
		SetUpdateModelview(true);
		SetUpdateProjection(true);
	}
}
void AdvencedCamera::UpdateCameraRotation()
{
	if (bUpdateCamRot) {
		if (abs2(DesiredCamRot - GetCameraRotation()) < _CAM_ADV_ROT_EPSILON)
		{
			static_cast<Camera*>(this)->SetCameraRotation(DesiredCamRot);
			bUpdateCamRot = false;
		}
		else
		{
			static_cast<Camera*>(this)->SetCameraRotation(GetCameraRotation()*(1 - _CAM_ADV_ROT_CHANGE_RATIO) + DesiredCamRot * _CAM_ADV_ROT_CHANGE_RATIO);
		}
	}
	else
	{
		DesiredCamRot = GetCameraRotation();
	}
}
void AdvencedCamera::UpdateFocusDistance(){
	if (bUpdateFocusDistance)
	{
		if (abs(GetFocusDistance() - dDesiredFocusDistance) < _CAM_ADV_POS_EPSILON)
		{
			static_cast<Camera*>(this)->SetFocusDistance(dDesiredFocusDistance);
			bUpdateFocusDistance = false;
		}
		else
		{
			static_cast<Camera*>(this)->SetFocusDistance(_CAM_ADV_POS_CHANGE_RATIO*dDesiredFocusDistance + (1 - _CAM_ADV_POS_CHANGE_RATIO)*GetFocusDistance());
		}
	}
}
void AdvencedCamera::UpdateOriginPosition()
{
	if (bUpdateOriginPos)
	{
		Vector3 CurrentPos;
		Vector3 DesiredPos;

		if (bDesiredOrigin == CAM_CAMERA)
		{
			CurrentPos = GetCameraPosition();
			DesiredPos = DesiredOriginPosCam;
			DesiredOriginPosTarget = DesiredOriginPosCam - GetFocusDistance()*GetCameraZ();
		}
		else
		{
			CurrentPos = GetTargetPosition();
			DesiredPos = DesiredOriginPosTarget;
			DesiredOriginPosCam = DesiredOriginPosTarget + GetFocusDistance()*GetCameraZ();
		}

		if (abs2(DesiredPos - CurrentPos) < _CAM_ADV_POS_EPSILON)
		{
			static_cast<Camera*>(this)->SetAllPosition(bDesiredOrigin, DesiredPos);
			bUpdateOriginPos = false;
		}
		else
		{
			static_cast<Camera*>(this)->SetAllPosition(bDesiredOrigin, _CAM_ADV_POS_CHANGE_RATIO*DesiredPos + (1 - _CAM_ADV_POS_CHANGE_RATIO)* CurrentPos);
		}
	}
}
void AdvencedCamera::UpdateCameraPosition()
{
	if (bUpdateCamPos) {
		if (abs2(DesiredCamPos - GetCameraPosition()) < _CAM_ADV_POS_EPSILON)
		{

			static_cast<Camera*>(this)->SetCameraPosition(DesiredCamPos);
			bUpdateCamPos = false;
			bUpdateOriginPos = false;
		}
		else
		{
			static_cast<Camera*>(this)->SetCameraPosition(GetCameraPosition()*(1 - _CAM_ADV_POS_CHANGE_RATIO) + DesiredCamPos * _CAM_ADV_POS_CHANGE_RATIO);
		}
	}
}
void AdvencedCamera::UpdateTargetPosition()
{
	if (bUpdateTargetPos)
	{
		if (abs2(DesiredTargetPos - GetTargetPosition()) < _CAM_ADV_POS_EPSILON)
		{
			static_cast<Camera*>(this)->SetTargetPosition(DesiredTargetPos);
			bUpdateTargetPos = false;
			bUpdateOriginPos = false;
		}
		else
		{
			static_cast<Camera*>(this)->SetTargetPosition(GetTargetPosition()*(1 - _CAM_ADV_POS_CHANGE_RATIO) + DesiredTargetPos * _CAM_ADV_POS_CHANGE_RATIO);
		}
	}
}
void AdvencedCamera::UpdateAdvencedCameraStatus()
{
	UpdateFOV();
	UpdateClipDistance();
	UpdateViewMode();
	UpdateCameraRotation();
	UpdateFocusDistance();
	UpdateOriginPosition();
	UpdateCameraPosition();
	UpdateTargetPosition();
}

void AdvencedCamera::SetupCamera()
{
	UpdateAdvencedCameraStatus();
	SetupProjectionMatrix();
	SetupModelviewMatrix();
}

void AdvencedCamera::SetupAllMatrix(bool ForcedUpdate)
{
	if (ForcedUpdate)
	{
		SetUpdateModelview(true);
		SetUpdateProjection(true);
	}
	SetupProjectionMatrix();
	SetupModelviewMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void AdvencedCamera::SetupModelviewMatrix()
{
	if (GetUpdateModelview() == true)
	{
		Vector3 Pos = GetCameraPosition();
		Vector3 Rot = GetCameraRotation();
		SetUpdateModelview(false);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0, 0, -dViewModeFocusDistance);
		glRotatef(-Rot.z, 0.0, 0.0, 1.0);
		glRotatef(-Rot.x, 1.0, 0.0, 0.0);
		glRotatef(-Rot.y, 0.0, 1.0, 0.0);
		glTranslatef(-Pos.x, -Pos.y, -Pos.z);

	}
}
void AdvencedCamera::SetupProjectionMatrix()
{
	if (GetUpdateProjection() == true)
	{
		SetUpdateProjection(false);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (static_cast<Camera*>(this)->GetViewMode() == CAM_ISOMETRIC)
		{
			float tempSeeRange = GetFocusDistance()*tan(_DEG2RAD*GetFOV() / 2);
			glOrtho(-GetAspectRatio()*tempSeeRange, GetAspectRatio()*tempSeeRange, -tempSeeRange, tempSeeRange, GetNearClipDistance(), GetFarClipDistance());

		}
		else if (static_cast<Camera*>(this)->GetViewMode() == CAM_PERSPECTIVE)
		{
			gluPerspective(GetFOV() - dViewModeFOV, GetAspectRatio(), GetNearClipDistance() + dViewModeFocusDistance, GetFarClipDistance() + dViewModeFocusDistance);
		}
	}
	//	glMatrixMode(GL_MODELVIEW);
}