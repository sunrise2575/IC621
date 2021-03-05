#include <Prelude.h>
#include _CH_VECTOR_
#include _CH_MATRIX_
#include _CH_UTILITY_

#define _RAD2DEG 57.295779// 180/PI
#define _DEG2RAD 0.017453 // PI/180

/*
 * 물체의 이동과 회전, 그리고 관련되는 함수들에 대한 클래스.
 */

class Transition
{
private:

	Matrix2 LocalCoord{ 3, 3 };

	void UpdateCorrectDegree();//기본 각도를 -360~360 사이로 제한.
	

protected:

	Vector3 Origin;
	Vector3 Position;
	Vector3 Rotation;
	Vector3 Scale{1};//only Local

public:
	Transition();

	Vector3 GetLocalX();
	Vector3 GetLocalY();
	Vector3 GetLocalZ();

	Vector3 GetOrigin();
	Vector3 GetPosition();
	Vector3 GetRotation();
	Vector3 GetScale();//GET 함수들

	void SetOrigin(Vector3 newPos);
	void SetPosition(Vector3 newPos);
	void SetRotation(Vector3 newRot);
	void SetScale(Vector3 newSize);//SET 함수들

	void AddOrigin(Vector3 movePos, bool isWorld = false);
	void AddPosition(Vector3 movePos, bool isWorld = true);
	void AddRotation(Vector3 moveRot, bool isWorld = true);//ADD 함수들
	void AddScale(Vector3 addSize);//SET 함수들
};
Transition::Transition(){LocalCoord = Util::rotationMatrix(Rotation*_DEG2RAD);}

Vector3 Transition::GetLocalX() { return Vector3(LocalCoord[0][0], LocalCoord[1][0], LocalCoord[2][0]); }
Vector3 Transition::GetLocalY() { return Vector3(LocalCoord[0][1], LocalCoord[1][1], LocalCoord[2][1]); }
Vector3 Transition::GetLocalZ() { return Vector3(LocalCoord[0][2], LocalCoord[1][2], LocalCoord[2][2]); }

Vector3 Transition::GetOrigin() { return Origin; }
Vector3 Transition::GetPosition() { return Position; }
Vector3 Transition::GetRotation() { return Rotation; }
Vector3 Transition::GetScale() { return Scale; } //GET 함수들

void Transition::SetOrigin(Vector3 newPos)
{
	Position += newPos - Origin;
	Origin = newPos;
}
void Transition::SetPosition(Vector3 newPos)
{
	Position = newPos;
}
void Transition::SetRotation(Vector3 newRot)
{
	Rotation = newRot;
	UpdateCorrectDegree();
	LocalCoord = Util::rotationMatrix(Rotation*_DEG2RAD);
}
void Transition::SetScale(Vector3 newSize)
{
	Scale = newSize;
}//SET 함수들

void Transition::AddOrigin(Vector3 movePos, bool isWorld)
{
	if (!isWorld) { movePos = GetLocalX()*movePos.x + GetLocalY()*movePos.y + GetLocalZ()*movePos.z; }
	Position += movePos;
	Origin += movePos;
}
void Transition::AddPosition(Vector3 movePos, bool isWorld)
{
	if (!isWorld) { movePos = GetLocalX()*movePos.x + GetLocalY()*movePos.y + GetLocalZ()*movePos.z; }
	Position += movePos;
}

void Transition::AddRotation(Vector3 moveRot, bool isWorld)
{
	if (isWorld) { Rotation += moveRot; LocalCoord = Util::rotationMatrix(Rotation*_DEG2RAD); }
	else
	{
		double HorizontalLength;
		Vector3 NewRotation{ 0 };
		Matrix2 NewLocalCoord{ 3, 3 };

		moveRot *= _DEG2RAD;
		NewLocalCoord = LocalCoord*Util::rotationMatrix(moveRot);//Calc Local Coord After rotation;
		HorizontalLength = sqrt(pow(NewLocalCoord[0][2], 2) + pow(NewLocalCoord[2][2], 2));
		
		NewRotation.x = atan2(-NewLocalCoord[1][2],HorizontalLength);
		if (0 == HorizontalLength)
		{//x is 90 or -90 deg;
			if (-NewLocalCoord[1][2] < 0)
				NewRotation.y = atan2(-NewLocalCoord[0][1], -NewLocalCoord[2][1]);
			else
				NewRotation.y = atan2(NewLocalCoord[0][1], NewLocalCoord[2][1]);
		}//z is useless so not calc( Rotation Lock )
		NewRotation.y = atan2(-NewLocalCoord[0][2], -NewLocalCoord[2][2]);
		NewRotation.z = atan2(NewLocalCoord[1][0], NewLocalCoord[1][1]);
		
		LocalCoord = NewLocalCoord;
		Rotation = NewRotation*_RAD2DEG;

		
	}
	void UpdateCorrectDegree();
}
void Transition::AddScale(Vector3 addSize){ Scale += addSize; }

void Transition::UpdateCorrectDegree()
{
	Rotation.x -= ((int)Rotation.x / 360) * 360;
	Rotation.y -= ((int)Rotation.y / 360) * 360;
	Rotation.z -= ((int)Rotation.z / 360) * 360;
}
