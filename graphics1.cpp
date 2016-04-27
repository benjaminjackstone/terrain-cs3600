
#include <stdlib.h>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
using namespace std;
#include "graphics.h"
#include "rat.h"
#include "glut.h"


// Global Variables (Only what you need!)
double screen_x = 700;
double screen_y = 500;
double gRatX;
double gRatY;
double gRatZ;
double gWorldBuffer = .5;
double gWaterHeight = -7.5;
bool gLeftButtonDown = false;
bool gMiddleButtonDown = false;
bool gRightButtonDown = false;
bool wavecahnge = false;
double gTilt = 0;
double wave = 0;
const int M = 100;
const int N = 100;

double zHeight[M + 1][N + 1];

RAT_MODE gCurrentView = PERSPECTIVE_VIEW;

Rat gRat;

//support for multiple machines
double GetDeltaTime() {
	static clock_t start_time = clock();
	static int current_frame = 0;
	clock_t current_time = clock();
	current_frame += 1;
	double total_time = double(current_time - start_time) / CLOCKS_PER_SEC;
	if (total_time == 0)
		total_time = .00001;
	double frames_per_second = (double)current_frame / total_time;
	double DT = 1.0 / frames_per_second;
	return DT;
}

double function(double x, double y) {

	double z = .4*sin(x*.25) + .3*sin(y*.25 - .43);
	z += 2 * sin(.4*y);
	z += 1.5 * cos(.03*x);
	z += 2 * sin(.2*x)*cos(.3*y);
	z += 3 * sin(.11*x)*cos(.03*y);
	double amplitude = 3.0;
	z *= amplitude;
	return z;
}
// 
// Functions that draw basic primitives
//
void DrawCircle(double x1, double y1, double radius)
{
	glBegin(GL_POLYGON);
	for(int i=0; i<32; i++)
	{
		double theta = (double)i/32.0 * 2.0 * 3.1415926;
		double x = x1 + radius * cos(theta);
		double y = y1 + radius * sin(theta);
		glVertex2d(x, y);
	}
	glEnd();
}

void DrawRectangle(double x1, double y1, double x2, double y2)
{
	glBegin(GL_QUADS);
	glVertex2d(x1,y1);
	glVertex2d(x2,y1);
	glVertex2d(x2,y2);
	glVertex2d(x1,y2);
	glEnd();
}

void DrawLine(double x1, double y1, double x2, double y2)
{
	glBegin(GL_LINES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glEnd();
}

void DrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	glBegin(GL_TRIANGLES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glVertex2d(x3,y3);
	glEnd();
}

// Outputs a string of text at the specified location.
void DrawText(double x, double y, char *string)
{
	void *font = GLUT_BITMAP_9_BY_15;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
	
	int len, i;
	glRasterPos2d(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) 
	{
		glutBitmapCharacter(font, string[i]);
	}

    glDisable(GL_BLEND);
}


void Generate() {
	for (int i = 0; i <= M; i++)
	{
		for (int j = 0; j <= N; j++)
		{
			zHeight[i][j] = function(double(i), double(j));
		}
	}
}

//
// GLUT callback functions
//

// This callback function gets called by the Glut
// system whenever it decides things need to be redrawn.
void display(void)
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double height = function(gRat.GetX(), gRat.GetY());

	glLoadIdentity();
	if (gCurrentView == PERSPECTIVE_VIEW) {
		gluLookAt(M / 2, -N / 10 , M / 2, gRat.GetX(), gRat.GetY(), 0, 0, 0, 1);
	}
	if (gCurrentView == RAT_VIEW) {
		double dt = GetDeltaTime();
		double rad = gRat.GetRotation() / 180.0 * 3.1459;
		double dx = cos(rad) * gRat.GetSpeed() * dt ;
		double dy = sin(rad) * gRat.GetSpeed() * dt;
		double hover = 2.0;
		double terrainHeight = function(gRat.GetX() + dx, gRat.GetY() + dy);
		double waterHeight = gWaterHeight;
		double H = fmax(terrainHeight, waterHeight) + hover;
		double currentTerrainHeight = fmax(waterHeight, function(gRat.GetX(),gRat.GetY()));
		if (currentTerrainHeight + 1 >= waterHeight && terrainHeight - .5 < waterHeight) {
			if (gTilt <= 0) gTilt += .00006;
			else gTilt = 0;
		}
		if (currentTerrainHeight - 1 <= waterHeight && terrainHeight + .5 > waterHeight) {
			if (gTilt >= 0 && gTilt < .1) gTilt += .00006;
		}
		else {
			gTilt = (fmax(terrainHeight, waterHeight) - currentTerrainHeight);
		}
		double lookZ = H + gTilt;
		if (currentTerrainHeight == waterHeight) {
			gRat.SetSpeed(20);
			gluLookAt(gRat.GetX(), gRat.GetY(), H, gRat.GetX() + dx, gRat.GetY() + dy, lookZ, 0, 0, 1);
		}
		else {
			gRat.SetSpeed(10.4);
			gluLookAt(gRat.GetX(), gRat.GetY(), H, gRat.GetX() + dx, gRat.GetY() + dy, lookZ, 0, 0, 1);
		}
	}
	
	glBegin(GL_QUADS);
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			
			glColor3ub(.5,(int)((zHeight[i][j]+14)*8)%256,.5);
			glVertex3d(i, j, zHeight[i][j]);
			glVertex3d(i, j+1, zHeight[i][j+1]);
			glVertex3d(i+1, j+1, zHeight[i+1][j+1]);
			glVertex3d(i+1, j, zHeight[i+1][j]);
		}
	}
	glEnd();
	if (wave < -.1) {
		wavecahnge = !wavecahnge;
	}
	if (!wavecahnge) {
		gWaterHeight -= 0.0001;
		wave += 0.0001;
	}
	if (wavecahnge) {
		gWaterHeight += 0.0001;
		wave -= 0.0001;
	}
	if (wave > .1) {
		wavecahnge = !wavecahnge;
	}

	//Draw water
	glBegin(GL_QUADS);
	glColor4d(0, 0, 1, .5);
	glVertex3d(0, 0, gWaterHeight);
	glVertex3d(M*2, 0, gWaterHeight);
	glVertex3d(M*2, N*2, gWaterHeight);
	glVertex3d(0, N*2, gWaterHeight);
	glEnd();

	if (gLeftButtonDown == true) {
		gRat.TurnLeft(GetDeltaTime());
	}
	if (gRightButtonDown == true) {
		gRat.TurnRight(GetDeltaTime());
	}
	if (gMiddleButtonDown == true) {
		gRat.Walk(GetDeltaTime());
	}
	if (gCurrentView != RAT_VIEW) {
		glPushMatrix();
		if (height > gWaterHeight) {
			glTranslated(gRat.GetX(), gRat.GetY(), height + 2.0);
		}
		else {
			glTranslated(gRat.GetX(), gRat.GetY(), gWaterHeight+ 2.0);
		}
		glRotated(gRat.GetRotation(), 0, 0, 1);
		glScaled(0.7, 0.7, 1.0);
		gRat.Draw();
		if (height > gWaterHeight) {
			glTranslated(-gRat.GetX(), -gRat.GetY(), -height - 2.0);
		}
		else {
			glTranslated(-gRat.GetX(), -gRat.GetY(), -gWaterHeight - 2.0);
		}
		glPopMatrix();
	}

	glutSwapBuffers();
	glutPostRedisplay();
}


void SetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double fov = 60;
	double aspect = float(screen_y) / screen_x;
	double near = .1;
	double far = (M + N * 5.0);
	gluPerspective(fov, aspect, near, far);
	glMatrixMode(GL_MODELVIEW);
}

void SetOrthographicProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-gWorldBuffer, M + gWorldBuffer, -gWorldBuffer, N + gWorldBuffer);
	glMatrixMode(GL_MODELVIEW);
}


// This callback function gets called by the Glut
// system whenever a key is pressed.
void keyboard(unsigned char c, int x, int y)
{
	switch (c) 
	{
		case 27: // escape character means to quit the program
			exit(0);
			break;
		case 't':
			gCurrentView = TOP_VIEW;
			SetOrthographicProjection();
			break;
		case 'p':
			gCurrentView = PERSPECTIVE_VIEW;
			SetPerspectiveProjection();
			break;
		case 'r':
			gCurrentView = RAT_VIEW;
			SetPerspectiveProjection();
			break;
		case 'u':
			gWaterHeight += 1.0;
			break;
		case 'd':
			gWaterHeight -= 1.0;
			break;
		default:
			return; // if we don't care, return without glutPostRedisplay()
	}

	glutPostRedisplay();
}

void handleSpecialKeypress(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		gLeftButtonDown = true;
		break;
	case GLUT_KEY_RIGHT:
		gRightButtonDown = true;
		break;
	case GLUT_KEY_UP:
		gMiddleButtonDown = true;
		break;
	}
}

void handleSpecialKeyReleased(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		gLeftButtonDown = false;
		break;
	case GLUT_KEY_RIGHT:
		gRightButtonDown = false;
		break;
	case GLUT_KEY_UP:
		gMiddleButtonDown = false;
		break;
	}
}


// This callback function gets called by the Glut
// system whenever the window is resized by the user.
void reshape(int w, int h)
{
	// Reset our global variables to the new width and height.
	screen_x = w;
	screen_y = h;

	// Set the pixel resolution of the final picture (Screen coordinates).
	glViewport(0, 0, w, h);

	// Set the projection mode to 2D orthographic, and set the world coordinates:
	if (gCurrentView == TOP_VIEW) {
		SetOrthographicProjection();
	} else {
		SetPerspectiveProjection();
	}
}

void motion(int x, int y) {
	//not really needed
	y = screen_y - y;
	double ratio = (double)x / screen_x;
	double wx = -gWorldBuffer + ratio * (M + 1.0);
	ratio = (double)y / screen_y;
	double wy = -gWorldBuffer + ratio * (N + 1.0);
	glutPostRedisplay();

}

// This callback function gets called by the Glut
// system whenever any mouse button goes up or down.
void mouse(int mouse_button, int state, int x, int y)
{
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{	
		gLeftButtonDown = true;
	}
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
	{
		gLeftButtonDown = false;
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) 
	{
		gMiddleButtonDown = true;
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) 
	{
		gMiddleButtonDown = false;
	}
	if (mouse_button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		gRightButtonDown = true;
	}
	if (mouse_button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		gRightButtonDown = false;
	}
	glutPostRedisplay();
}

// Your initialization code goes here.
void InitializeMyStuff()
{
	Generate();
	gRatX = M / 2;
	gRatY = N / 2;
	gRat.SetX(gRatX);
	gRat.SetY(gRatY);
	gRatZ = function(gRat.GetX(), gRat.GetY()) +.05; //makes the mouse float above the mountains
	gRat.SetZ(gRatZ);
}


int main(int argc, char **argv)
{
	srand(time(0));
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_x, screen_y);
	glutInitWindowPosition(50, 50);

	int fullscreen = 0;
	if (fullscreen) 
	{
		glutGameModeString("800x600:32");
		glutEnterGameMode();
	} 
	else 
	{
		glutCreateWindow("Terrain");
	}

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutSpecialFunc(handleSpecialKeypress);
	glutSpecialUpFunc(handleSpecialKeyReleased);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor3d(0,0,0); // forground color
	glClearColor(1, 1, 1, 0); // background color
	InitializeMyStuff();

	glutMainLoop();

	return 0;
}
