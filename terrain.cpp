#include "terrain.h"
#include "graphics.h"
#include <cmath>
#include "glut.h"

Terrain::Terrain(){}


void Terrain::Generate() {
	for (int i = 0; i <= M; i++)
	{
		for (int j = 0; j <= N; j++)
		{
			zHeight[i][j] = function(double(i), double(j));
		}
	}
}

double Terrain::getZHeight(double x, double y) {
	return zHeight[int(x)][int(y)];
}

void Terrain::Draw() {
	glBegin(GL_QUADS);
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			glColor3b((i * 34532 + j * 13542551) % 256, 
				(i * 23535 + j * 1355571) % 256, 
				(i * 456542 + j * 1358888) % 256);
			/*glVertex3d(i, j, zHeight[i][j]);
			glVertex3d(i, j+1, zHeight[i][j+1]);
			glVertex3d(i+1, j+1, zHeight[i+1][j+1]);
			glVertex3d(i+1, j, zHeight[i+1][j]);*/
			glVertex3d(i, j, function(i, j));
			glVertex3d(i, j + 1, function(i, j + 1));
			glVertex3d(i + 1, j + 1, function(i + 1, j + 1));
			glVertex3d(i + 1, j, function(i + 1, j));
		}
	}
	glEnd();

	//Draw water
	double waterLevel = -7.5;
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 255);
	glVertex3d(0, 0, waterLevel);
	glVertex3d(M, 0, waterLevel);
	glVertex3d(M, N, waterLevel);
	glVertex3d(0, N, waterLevel);
	glEnd();
}