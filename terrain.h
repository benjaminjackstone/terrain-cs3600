#pragma once
const int M = 100;
const int N = 100;


class Terrain {

public:
	Terrain();
	void Draw();
	void Generate();
	double getZHeight(double x, double y);


private:
	double zHeight[M+1][N+1];

};