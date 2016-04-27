#pragma once

void DrawLine(double x1, double y1, double x2, double y2);
void DrawCircle(double x1, double y1, double radius);
void DrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3);
double function(double x, double y);

enum RAT_MODE { TOP_VIEW, PERSPECTIVE_VIEW, RAT_VIEW };
