#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <ctype.h>
#include <conio.h>
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
const int num_textures = 2;
static GLuint texName[num_textures];

// tga image loader code.
#include "tga.h"
gliGenericImage *readTgaImage(char *filename)
{
	FILE *file;
	gliGenericImage *image;
	file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Error: could not open \"%s\"\n", filename);
		return NULL;
	}
	image = gliReadTGA(file, filename);
	fclose(file);
	if (image == NULL) {
		printf("Error: could not decode file format of \"%s\"\n", filename);
		return NULL;
	}
	return image;
}


// Generic image loader code.
gliGenericImage *readImage(char *filename)
{
	size_t size = strlen(filename);
	if (toupper(filename[size - 3]) == 'T' && toupper(filename[size - 2]) == 'G' && toupper(filename[size - 1]) == 'A')
	{
		gliGenericImage * result = readTgaImage(filename);
		if (!result)
		{
			cerr << "Error opening " << filename << endl;
			_getch();
			exit(1);
		}
		return result;
	}
	else
	{
		cerr << "Unknown Filetype!\n";
		_getch();
		exit(1);
	}
}
void SetBorder(gliGenericImage * image)
{
	// set a border color.
	unsigned int border_r = 50;
	unsigned int border_g = 50;
	unsigned int border_b = 255;
	int x, y;
	y = 0;
	for (x = 0; x<image->width; x++)
	{
		image->pixels[3 * (y*image->width + x) + 0] = border_r;
		image->pixels[3 * (y*image->width + x) + 1] = border_g;
		image->pixels[3 * (y*image->width + x) + 2] = border_b;
	}
	y = 1;
	for (x = 0; x<image->width; x++)
	{
		image->pixels[3 * (y*image->width + x) + 0] = border_r;
		image->pixels[3 * (y*image->width + x) + 1] = border_g;
		image->pixels[3 * (y*image->width + x) + 2] = border_b;
	}
	y = image->height - 1;
	for (x = 0; x<image->width; x++)
	{
		image->pixels[3 * (y*image->width + x) + 0] = border_r;
		image->pixels[3 * (y*image->width + x) + 1] = border_g;
		image->pixels[3 * (y*image->width + x) + 2] = border_b;
	}
	y = image->height - 2;
	for (x = 0; x<image->width; x++)
	{
		image->pixels[3 * (y*image->width + x) + 0] = border_r;
		image->pixels[3 * (y*image->width + x) + 1] = border_g;
		image->pixels[3 * (y*image->width + x) + 2] = border_b;
	}

	x = 0;
	for (y = 0; y<image->height; y++)
	{
		image->pixels[3 * (y*image->width + x) + 0] = border_r;
		image->pixels[3 * (y*image->width + x) + 1] = border_g;
		image->pixels[3 * (y*image->width + x) + 2] = border_b;
	}
	x = 1;
	for (y = 0; y<image->height; y++)
	{
		image->pixels[3 * (y*image->width + x) + 0] = border_r;
		image->pixels[3 * (y*image->width + x) + 1] = border_g;
		image->pixels[3 * (y*image->width + x) + 2] = border_b;
	}
	x = image->width - 1;
	for (y = 0; y<image->height; y++)
	{
		int index = 3 * (y*image->width + x);
		image->pixels[index + 0] = border_r;
		image->pixels[index + 1] = border_g;
		image->pixels[index + 2] = border_b;
	}
	x = image->width - 2;
	for (y = 0; y<image->height; y++)
	{
		int index = 3 * (y*image->width + x);
		image->pixels[index + 0] = border_r;
		image->pixels[index + 1] = border_g;
		image->pixels[index + 2] = border_b;
	}
}

// Return true if h is a perfect power of 2 (up to 4096)
bool PowerOf2(int h)
{
	if (h != 2 && h != 4 && h != 8 && h != 16 && h != 32 && h != 64 && h != 128 &&
		h != 256 && h != 512 && h != 1024 && h != 2048 && h != 4096)
		return false;
	else
		return true;
}
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

	double z = 0;
	double zScaling = 0.9;
	z += 2 * sin(.4*y);
	z += 1.5 * cos(.3*x);
	z += 4 * sin(.2*x)*cos(.3*y);
	z += 6 * sin(.11*x)*cos(.03*y);
	z *= zScaling;
	return z;
}
// 
// Functions that draw basic primitives
//
void DrawCircle(double x1, double y1, double radius)
{
	glBegin(GL_POLYGON);
	for (int i = 0; i<32; i++)
	{
		double theta = (double)i / 32.0 * 2.0 * 3.1415926;
		double x = x1 + radius * cos(theta);
		double y = y1 + radius * sin(theta);
		glVertex2d(x, y);
	}
	glEnd();
}

void DrawRectangle(double x1, double y1, double x2, double y2)
{
	glBegin(GL_QUADS);
	glVertex2d(x1, y1);
	glVertex2d(x2, y1);
	glVertex2d(x2, y2);
	glVertex2d(x1, y2);
	glEnd();
}

void DrawLine(double x1, double y1, double x2, double y2)
{
	glBegin(GL_LINES);
	glVertex2d(x1, y1);
	glVertex2d(x2, y2);
	glEnd();
}

void DrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	glBegin(GL_TRIANGLES);
	glVertex2d(x1, y1);
	glVertex2d(x2, y2);
	glVertex2d(x3, y3);
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
	len = (int)strlen(string);
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
		gluLookAt(M / 2, -N / 2, M / 2, gRat.GetX(), gRat.GetY(), 0, 0, 0, 1);
	}
	if (gCurrentView == RAT_VIEW) {
		double dt = GetDeltaTime();
		double rad = gRat.GetRotation() / 180.0 * 3.1459;
		double dx = cos(rad) * gRat.GetSpeed() * dt;
		double dy = sin(rad) * gRat.GetSpeed() * dt;
		double hover = 2.0;
		double terrainHeight = function(gRat.GetX() + dx, gRat.GetY() + dy);
		double waterHeight = gWaterHeight;
		double H = fmax(terrainHeight, waterHeight) + hover;
		double currentTerrainHeight = fmax(waterHeight, function(gRat.GetX(), gRat.GetY()));
		if (currentTerrainHeight + 1 >= waterHeight && terrainHeight - .5 < waterHeight) {
			if (gTilt <= 0) gTilt += .006;
			else gTilt = 0;
		}
		if (currentTerrainHeight - 1 <= waterHeight && terrainHeight + .5 > waterHeight) {
			if (gTilt >= 0 && gTilt < .1) gTilt += .006;
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
	if (gCurrentView == RAT_VIEW) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texName[1]);
		static double h = 0.0;
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0, 0, 20);
		glTexCoord2d(1, 0); glVertex3d(0, N, 20);
		glTexCoord2d(1, 1); glVertex3d(M, N, 20);
		glTexCoord2d(0, 1); glVertex3d(M, 0, 20);
		glEnd();
		h += .00000007;
		glDisable(GL_TEXTURE_2D);

	}
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			/*
			glColor3ub(.5,(int)((zHeight[i][j]+14)*8)%256,.5);
			glVertex3d(i, j, zHeight[i][j]);
			glVertex3d(i, j+1, zHeight[i][j+1]);
			glVertex3d(i+1, j+1, zHeight[i+1][j+1]);
			glVertex3d(i+1, j, zHeight[i+1][j]);
			*/
			glColor4d(0, 0, 1, 1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texName[0]);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3d(i, j, zHeight[i][j]);
			glTexCoord2d(1, 0); glVertex3d(i, j + 1, zHeight[i][j + 1]);
			glTexCoord2d(1, 1); glVertex3d(i + 1, j + 1, zHeight[i + 1][j + 1]);
			glTexCoord2d(0, 1); glVertex3d(i + 1, j, zHeight[i + 1][j]);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
	}
	if (wave < -.1) {
		wavecahnge = !wavecahnge;
	}
	if (!wavecahnge) {
		gWaterHeight -= 0.001;
		wave += 0.001;
	}
	if (wavecahnge) {
		gWaterHeight += 0.001;
		wave -= 0.001;
	}
	if (wave > .1) {
		wavecahnge = !wavecahnge;
	}

	//Draw water
	glBegin(GL_QUADS);
	glColor4d(0, 0, 1, .5);
	glVertex3d(0, 0, gWaterHeight);
	glVertex3d(M, 0, gWaterHeight);
	glVertex3d(M, N, gWaterHeight);
	glVertex3d(0, N, gWaterHeight);
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
			glTranslated(gRat.GetX(), gRat.GetY(), gWaterHeight + 2.0);
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
	gluOrtho2D(0, M, 0, N);
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
	}
	else {
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
	gliGenericImage *image[num_textures];
	int n = 0;
	image[n++] = readImage("good_grass.tga");
	image[n++] = readImage("sky.tga");
	if (n != num_textures)
	{
		printf("Error: Wrong number of textures\n");
		_getch();
		exit(1);;
	}

	glGenTextures(num_textures, texName);

	for (int i = 0; i<num_textures; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texName[i]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		int repeats = false;
		int needs_border = false; // Needed if clamping and not filling the whole polygon.
		if (i == 0 || i == 1) repeats = true;
		if (i == 2) needs_border = true;
		if (repeats)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		if (needs_border) {
			// set a border.
			SetBorder(image[i]);
		}

		bool mipmaps = false;
		if (!PowerOf2(image[i]->height) || !PowerOf2(image[i]->width))
		{
			// WARNING: Images that do not have width and height as 
			// powers of 2 MUST use mipmaps.
			mipmaps = true;
		}

		if (mipmaps)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, image[i]->components,
				image[i]->width, image[i]->height,
				image[i]->format, GL_UNSIGNED_BYTE, image[i]->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				//GL_LINEAR_MIPMAP_LINEAR);
				GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				//GL_LINEAR);
				GL_NEAREST);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, image[i]->components,
				image[i]->width, image[i]->height, 0,
				image[i]->format, GL_UNSIGNED_BYTE, image[i]->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	Generate();
	gRatX = M / 2;
	gRatY = N / 2;
	gRat.SetX(gRatX);
	gRat.SetY(gRatY);
	gRatZ = function(gRat.GetX(), gRat.GetY()) + .05; //makes the mouse float above the mountains
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
	glColor3d(0, 0, 0); // forground color
	glClearColor(1, 1, 1, 0); // background color
	InitializeMyStuff();

	glutMainLoop();

	return 0;
}
