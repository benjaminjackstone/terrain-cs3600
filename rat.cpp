#include "rat.h"
#include "graphics.h"
#include <math.h>
#include "glut.h"



Rat::Rat(){
	mSpeed = 10.4;
}

void Rat::Draw() {
	glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
	// bottom  // Red
	glColor3d(1, 1, 1);
	glVertex3f(1.0f, 0.0f, 0.0f);   // Green
	glVertex3f(-1.0f, -1.0f, 0.0f);  // Blue
	glVertex3f(-1.0f, 1.0f, 0.0f);
	
	// top     // Red
	glVertex3f(1.0f, 0.0f, 1.0f);    // Green
	glVertex3f(-1.0f, -1.0f, 1.0f);    // Blue
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();

	glBegin(GL_QUADS);
	// leftside    // red
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);    // blue
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);


	// rightside      // red
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);      // green
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	// back      // blue
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);    // green
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f,-1.0f, 0.0f);
	glEnd();
}

void Rat::TurnLeft(double DT) {
	mRotation += 200 * DT;
}

void Rat::TurnRight(double DT) {
	mRotation -= 200 * DT;
}

void Rat::Walk(double DT) {
	double radians = mRotation / 180 * 3.141592654;
	double dX = cos(radians) * mSpeed * DT;
	double dY = sin(radians) * mSpeed * DT;
	double newX = mX + dX;
	double newY = mY + dY;
		mX = newX;
		mY = newY;
}

void Rat::SetX(double newX) {
	mX = newX;
}

void Rat::SetY(double newY) {
	mY = newY;
}

void Rat::SetZ(double newZ) {
	mZ = newZ;
}

double Rat::GetX() {
	return mX;
}

double Rat::GetY() {
	return mY;
}

double Rat::GetZ() {
	return mZ;
}

double Rat::GetRotation() {
	return mRotation;
}
double Rat::GetSpeed() {
	return mSpeed;
}

