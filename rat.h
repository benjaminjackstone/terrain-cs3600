#pragma once
class Rat {

public:
	Rat();
	void Draw();
	void TurnLeft(double DT);
	void TurnRight(double DT);
	void Walk(double DT);
	double GetRotation();
	void SetX(double newX);
	void SetY(double newY);
	void SetZ(double newZ);
	double GetX();
	double GetY();
	double GetZ();
	double GetSpeed();
	void SetSpeed(double s) { mSpeed = s; };

private:
	double mRotation;
	double mX;
	double mY;
	double mZ;
	double mSpeed;
};