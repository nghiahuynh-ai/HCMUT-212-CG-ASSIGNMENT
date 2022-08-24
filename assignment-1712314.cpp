#include "stdafx.h"
#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <iostream>
using namespace std;

#define PI 3.1415926

struct Color
{
	float red, green, blue;
	Color() { red = green = blue = 0; }
	Color(int r, int b, int g) {
		red = r / 256.0;
		green = g / 256.0;
		blue = b / 256.0;
	}
};

#define numOfColor 16
Color colorList[numOfColor] = { Color(255,0,0), Color(0,255,0), Color(0,0,255), Color(0,68,255),
Color(255,128,0), Color(255,255,0), Color(255,0,255), Color(0,255,255),
Color(76,76,76), Color(128,128,128),Color(230,230,230),Color(255,128,128),
Color(128,255,128),Color(128,128,255),Color(0,0,0),Color(255,255,255) };

struct Point
{
	float x, y, z;
	Point() { x = y = z = 0; }
	Point(float a, float b, float c) {
		x = a;
		y = b;
		z = c;
	}
};

class Vector3
{
public:
	float	x, y, z;
	Vector3() { x = y = z = 0; }
	Vector3(float dx, float dy, float dz) { x = dx; y = dy; z = dz; }
	Vector3(Vector3& v) { x = v.x; y = v.y; z = v.z; }
	void set(float dx, float dy, float dz) { x = dx; y = dy; z = dz; }
	void set(Vector3& v) { x = v.x; y = v.y; z = v.z; }
	void flip() { x = -x; y = -y; z = -z; }
	Vector3 cross(Vector3 b) {
		Vector3 c(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
		return c;
	}
	float dot(Vector3 b) { return x * b.x + y * b.y + z * b.z; }
	void normalize() {
		float length = sqrt(x * x + y * y + z * z);
		x = x / length;
		y = y / length;
		z = z / length;
	}
};

class VertexID
{
public:
	int Index;
};

class Face
{
public:
	int nVerts;
	VertexID* vert;
	Vector3	facenorm;
	Face() {
		nVerts = 0;
		vert = NULL;
	}
	~Face() {
		if (vert != NULL) {
			delete[] vert;
			vert = NULL;
		}
		nVerts = 0;
	}
};


class Mesh
{
public:
	int numVerts;
	Point* pt;
	int numFaces;
	Face* face;
	Color color;

	Mesh() {
		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
	}
	~Mesh() {
		if (pt != NULL) delete[] pt;
		if (face != NULL) delete[] face;
		numVerts = 0;
		numFaces = 0;
	}
	void DrawWireFrame() {
		glColor3f(0, 0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int f = 0; f < numFaces; f++) {
			glBegin(GL_POLYGON);
			for (int v = 0; v < face[f].nVerts; v++) {
				int idx = face[f].vert[v].Index;
				glVertex3f(pt[idx].x, pt[idx].y, pt[idx].z);
			}
			glEnd();
		}
	}
	void DrawColor() {
		glColor3f(color.red, color.blue, color.green);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (int f = 0; f < numFaces; f++) {
			glBegin(GL_POLYGON);
			for (int v = 0; v < face[f].nVerts; v++) {
				int idx = face[f].vert[v].Index;
				glVertex3f(pt[idx].x, pt[idx].y, pt[idx].z);
			}
			glEnd();
		}
	}
	void CalculateFacesNorm() {
		for (int f = 0; f < numFaces; f++) {
			for (int v = 0; v < face[f].nVerts; v++)
			{
				int i = face[f].vert[v].Index;
				int next_i = face[f].vert[(v + 1) % face[f].nVerts].Index;
				face[f].facenorm.x += (pt[i].y - pt[next_i].y) * (pt[i].z + pt[next_i].z);
				face[f].facenorm.y += (pt[i].z - pt[next_i].z) * (pt[i].x + pt[next_i].x);
				face[f].facenorm.z += (pt[i].x - pt[next_i].x) * (pt[i].y + pt[next_i].y);
			}
			face[f].facenorm.normalize();
		}
	}
	void Draw() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (int f = 0; f < numFaces; f++) {
			glBegin(GL_POLYGON);
			for (int v = 0; v < face[f].nVerts; v++)
			{
				int	iv = face[f].vert[v].Index;
				glNormal3f(face[f].facenorm.x, face[f].facenorm.y, face[f].facenorm.z);
				glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
			}
			glEnd();

		}
	}

	void Cube(float	fSize, Color c)
	{
		color = c;
		int i;

		numVerts = 8;
		pt = new Point[numVerts];
		pt[0] = Point(-fSize, fSize, fSize);
		pt[1] = Point(fSize, fSize, fSize);
		pt[2] = Point(fSize, fSize, -fSize);
		pt[3] = Point(-fSize, fSize, -fSize);
		pt[4] = Point(-fSize, -fSize, fSize);
		pt[5] = Point(fSize, -fSize, fSize);
		pt[6] = Point(fSize, -fSize, -fSize);
		pt[7] = Point(-fSize, -fSize, -fSize);


		numFaces = 6;
		face = new Face[numFaces];

		//Left face
		face[0].nVerts = 4;
		face[0].vert = new VertexID[face[0].nVerts];
		face[0].vert[0].Index = 1;
		face[0].vert[1].Index = 5;
		face[0].vert[2].Index = 6;
		face[0].vert[3].Index = 2;

		//Right face
		face[1].nVerts = 4;
		face[1].vert = new VertexID[face[1].nVerts];
		face[1].vert[0].Index = 0;
		face[1].vert[1].Index = 3;
		face[1].vert[2].Index = 7;
		face[1].vert[3].Index = 4;

		//top face
		face[2].nVerts = 4;
		face[2].vert = new VertexID[face[2].nVerts];
		face[2].vert[0].Index = 0;
		face[2].vert[1].Index = 1;
		face[2].vert[2].Index = 2;
		face[2].vert[3].Index = 3;

		//bottom face
		face[3].nVerts = 4;
		face[3].vert = new VertexID[face[3].nVerts];
		face[3].vert[0].Index = 7;
		face[3].vert[1].Index = 6;
		face[3].vert[2].Index = 5;
		face[3].vert[3].Index = 4;

		//near face
		face[4].nVerts = 4;
		face[4].vert = new VertexID[face[4].nVerts];
		face[4].vert[0].Index = 4;
		face[4].vert[1].Index = 5;
		face[4].vert[2].Index = 1;
		face[4].vert[3].Index = 0;

		//Far face
		face[5].nVerts = 4;
		face[5].vert = new VertexID[face[5].nVerts];
		face[5].vert[0].Index = 3;
		face[5].vert[1].Index = 2;
		face[5].vert[2].Index = 6;
		face[5].vert[3].Index = 7;

		CalculateFacesNorm();
	}

	void Cylinder(float fHeight, float fRadius, Color c) {
		color = c;
		int nSegment = 40;
		numVerts = 2 * nSegment;
		pt = new Point[numVerts];

		int		i;
		int		idx;
		float	fAngle = 2 * PI / nSegment;
		float	x, y, z;

		for (i = 0; i < nSegment; i++) {
			pt[i] = Point(fRadius * cos(fAngle * i), fHeight, fRadius * sin(fAngle * i));
			pt[i + nSegment] = Point(fRadius * cos(fAngle * i), 0, fRadius * sin(fAngle * i));
		}

		numFaces = nSegment + 2;
		face = new Face[numFaces];

		idx = 0;
		face[idx].nVerts = numVerts / 2;
		face[idx].vert = new VertexID[face[idx].nVerts];
		for (int i = 0; i < nSegment; i++) {
			face[idx].vert[i].Index = i;
		}

		idx++;
		face[idx].nVerts = numVerts / 2;
		face[idx].vert = new VertexID[face[idx].nVerts];
		for (int i = nSegment; i < numVerts; i++) {
			face[idx].vert[i - nSegment].Index = i;
		}

		idx++;
		for (i = 0; i < nSegment; i++) {
			face[idx].nVerts = 4;
			face[idx].vert = new VertexID[face[idx].nVerts];

			face[idx].vert[0].Index = i;
			if (i < nSegment - 2)
				face[idx].vert[1].Index = i + 2;
			else
				face[idx].vert[1].Index = 1;
			face[idx].vert[2].Index = face[idx].vert[1].Index + nSegment;
			face[idx].vert[3].Index = face[idx].vert[0].Index + nSegment;

			idx++;
		}
		CalculateFacesNorm();
	}

	void Shape1(float fHeight, float fRadius, float padding, Color c) {
		color = c;
		int nSegment = 100, nSegmentPerQuarter = 24;
		numVerts = ((nSegmentPerQuarter + 1) * 8 + 8) + (nSegment * 2);
		pt = new Point[numVerts];

		//upper points
		float p = padding / 2, m = sqrt(fRadius * fRadius - p * p), init_angle = asin(padding / (2 * fRadius));
		int sign[4][2] = { { 1,1 },{ -1,1 },{ -1,-1 },{ 1,-1 } };
		for (int i = 0; i < 4; i++) {
			pt[i * (numVerts - nSegment * 2) / 4] = Point(p * sign[i][0], fHeight, p * sign[i][1]);
			pt[i * (numVerts - nSegment * 2) / 4 + 1] = Point(p * sign[i][0], 0, p * sign[i][1]);
			float start = i * PI / 2 + init_angle;
			float end = start + PI / 2 - 2 * init_angle;
			float increment = (end - start) / nSegmentPerQuarter;
			for (int j = 2; j < (numVerts - nSegment * 2) / 4; j += 2) {
				float angle = start + (j / 2 - 1) * increment;
				pt[i * (numVerts - nSegment * 2) / 4 + j] = Point(fRadius * cos(angle), fHeight, fRadius * sin(angle));
				pt[i * (numVerts - nSegment * 2) / 4 + j + 1] = Point(fRadius * cos(angle), 0, fRadius * sin(angle));
			}
		}
		//lower points
		float fAngle = 2 * PI / nSegment;
		int begin = (nSegmentPerQuarter + 1) * 8 + 8;
		for (int i = begin; i < numVerts; i += 2) {
			pt[i] = Point(fRadius * cos(fAngle * (i - begin) / 2), 0, fRadius * sin(fAngle * (i - begin) / 2));
			pt[i + 1] = Point(fRadius * cos(fAngle * (i - begin) / 2), -fHeight, fRadius * sin(fAngle * (i - begin) / 2));
		}

		numFaces = (nSegmentPerQuarter * 4 + 12) + (nSegment + 2);
		face = new Face[numFaces];

		//upper faces
		int idx = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < (numVerts - nSegment * 2) / 4; j += 2) {
				if (j == (numVerts - nSegment * 2) / 4 - 2) {
					face[idx].nVerts = 4;
					face[idx].vert = new VertexID[4];
					face[idx].vert[0].Index = i * (numVerts - nSegment * 2) / 4 + j;
					face[idx].vert[1].Index = i * (numVerts - nSegment * 2) / 4 + j + 1;
					face[idx].vert[2].Index = i * (numVerts - nSegment * 2) / 4 + 1;
					face[idx].vert[3].Index = i * (numVerts - nSegment * 2) / 4;
				}
				else {
					face[idx].nVerts = 4;
					face[idx].vert = new VertexID[4];
					face[idx].vert[0].Index = i * (numVerts - nSegment * 2) / 4 + j;
					face[idx].vert[1].Index = i * (numVerts - nSegment * 2) / 4 + j + 1;
					face[idx].vert[2].Index = i * (numVerts - nSegment * 2) / 4 + j + 3;
					face[idx].vert[3].Index = i * (numVerts - nSegment * 2) / 4 + j + 2;
				}
				idx++;
			}
		}
		for (int i = 0; i < 4; i++) {
			face[idx].nVerts = nSegmentPerQuarter + 2;
			face[idx].vert = new VertexID[nSegmentPerQuarter + 2];
			for (int j = 0; j < nSegmentPerQuarter + 2; j++) {
				face[idx].vert[j].Index = i * (numVerts - nSegment * 2) / 4 + j * 2;
			}
			idx++;
		}
		//lower faces
		for (int i = begin; i < numVerts; i += 2) {
			if (i == numVerts - 2) {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[4];
				face[idx].vert[0].Index = i;
				face[idx].vert[1].Index = i + 1;
				face[idx].vert[2].Index = begin + 1;
				face[idx].vert[3].Index = begin;
			}
			else {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[4];
				face[idx].vert[0].Index = i;
				face[idx].vert[1].Index = i + 1;
				face[idx].vert[2].Index = i + 3;
				face[idx].vert[3].Index = i + 2;
			}
			idx++;
		}
		face[idx].nVerts = nSegment;
		face[idx].vert = new VertexID[nSegment];
		for (int i = begin; i < numVerts; i += 2) {
			face[idx].vert[(i - begin) / 2].Index = i;
		}
		idx++;
		face[idx].nVerts = nSegment;
		face[idx].vert = new VertexID[nSegment];
		for (int i = begin + 1; i < numVerts; i += 2) {
			face[idx].vert[(i - begin - 1) / 2].Index = i;
		}
		CalculateFacesNorm();
	}

	void Shape2(float llength, float slength, float lwidth, float swidth, float height, Color c) {
		color = c;
		float x1 = -llength / 2, x2 = -slength / 2, x3 = slength / 2, x4 = llength / 2;
		float z1 = -lwidth / 2, z2 = -swidth / 2, z3 = swidth / 2, z4 = lwidth / 2;
		int nSegment = 20;
		numVerts = 16 + (nSegment - 2) * 2;
		pt = new Point[numVerts];
		Point p[16] = { Point(x4,height,z3),Point(x4,0,z3),Point(x3,height,z4),Point(x3,0,z4),
			Point(x2,height,z4),Point(x2,0,z4),Point(x1,height,z3),Point(x1,0,z3),
			Point(x1,height,z2),Point(x1,0,z2),Point(x2,height,z1),Point(x2,0,z1),
			Point(x3,height,z1),Point(x3,0,z1),Point(x4,height,z2),Point(x4,0,z2) };
		for (int i = 0; i < 16; i++) {
			pt[i] = p[i];
		}
		Point n11 = Point(p[0].x - p[2].x, height, p[0].z - p[2].z);
		Point n12 = Point(p[12].x - p[14].x, height, p[12].z - p[14].z);
		float m11 = n11.x * p[0].x + n11.z * p[0].z;
		float m12 = n12.x * p[14].x + n12.z * p[14].z;
		float xo = (n11.z * m12 - n12.z * m11) / (n11.z * n12.x - n11.x * n12.z);
		float yo = (m11 - n11.x * xo) / n11.z;
		Point o1 = Point(xo, height, yo);
		Point o2 = Point(-xo, height, yo);
		float r = sqrt(pow(o1.x - p[0].x, 2) + pow(o1.z - p[0].z, 2));
		float begin = -PI / 2 + acos(p[0].z / r);
		float angle = (PI - 2 * acos(p[0].z / r)) / (nSegment / 2);
		int idx = 16;
		for (int i = 1; i < nSegment / 2; i++) {
			float x = r * cos(begin + i * angle), z = r * sin(begin + i * angle);
			pt[idx] = Point(x + o1.x, height, z + o1.z);
			pt[idx + 1] = Point(x + o1.x, 0, z + o1.z);
			pt[idx + (nSegment / 2 - 1) * 2] = Point(-x + o2.x, height, z + o2.z);
			pt[idx + (nSegment / 2 - 1) * 2 + 1] = Point(-x + o2.x, 0, z + +o2.z);
			idx += 2;
		}

		numFaces = 8 + nSegment;
		face = new Face[numFaces];

		idx = 0;
		for (int i = 0; i < 16; i += 2) {
			if (i != 6 && i != 14) {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[4];
				face[idx].vert[0].Index = i;
				face[idx].vert[1].Index = i + 1;
				face[idx].vert[2].Index = i + 3;
				face[idx].vert[3].Index = i + 2;
				idx++;
			}

		}

		for (int i = 16; i < numVerts; i += 2) {
			if (i == numVerts - 2) break;
			else if (i == 16 + nSegment - 4) continue;
			else {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[4];
				face[idx].vert[0].Index = i;
				face[idx].vert[1].Index = i + 1;
				face[idx].vert[2].Index = i + 3;
				face[idx].vert[3].Index = i + 2;
				idx++;
			}
		}

		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[4];
		face[idx].vert[0].Index = 14;
		face[idx].vert[1].Index = 15;
		face[idx].vert[2].Index = 17;
		face[idx].vert[3].Index = 16;
		idx++;
		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[4];
		face[idx].vert[0].Index = 0;
		face[idx].vert[1].Index = 1;
		face[idx].vert[2].Index = 16 + (nSegment / 2 - 2) * 2 + 1;
		face[idx].vert[3].Index = 16 + (nSegment / 2 - 2) * 2;
		idx++;
		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[4];
		face[idx].vert[0].Index = 8;
		face[idx].vert[1].Index = 9;
		face[idx].vert[2].Index = 16 + (nSegment / 2 - 2) * 2 + 3;
		face[idx].vert[3].Index = 16 + (nSegment / 2 - 2) * 2 + 2;
		idx++;
		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[4];
		face[idx].vert[0].Index = 6;
		face[idx].vert[1].Index = 7;
		face[idx].vert[2].Index = numVerts - 1;
		face[idx].vert[3].Index = numVerts - 2;


		idx++;
		face[idx].nVerts = 8 + (nSegment / 2 - 1) * 2;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].Index = 8;
		face[idx].vert[1].Index = 10;
		face[idx].vert[2].Index = 12;
		face[idx].vert[3].Index = 14;
		int m = 4;
		for (int i = 16; i < 16 + nSegment - 2; i += 2) {
			face[idx].vert[m++].Index = i;
		}
		face[idx].vert[m++].Index = 0;
		face[idx].vert[m++].Index = 2;
		face[idx].vert[m++].Index = 4;
		face[idx].vert[m++].Index = 6;
		for (int i = numVerts - 2; i >= 16 + nSegment - 2; i -= 2) {
			face[idx].vert[m++].Index = i;
		}

		idx++;
		face[idx].nVerts = 8 + (nSegment / 2 - 1) * 2;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].Index = 9;
		face[idx].vert[1].Index = 11;
		face[idx].vert[2].Index = 13;
		face[idx].vert[3].Index = 15;
		m = 4;
		for (int i = 17; i < 16 + nSegment - 2; i += 2) {
			face[idx].vert[m++].Index = i;
		}
		face[idx].vert[m++].Index = 1;
		face[idx].vert[m++].Index = 3;
		face[idx].vert[m++].Index = 5;
		face[idx].vert[m++].Index = 7;
		for (int i = numVerts - 1; i >= 16 + nSegment - 2; i -= 2) {
			face[idx].vert[m++].Index = i;
		}
		CalculateFacesNorm();
	}

	void Shape3(float length, float width, float height, Color c) {
		color = c;
		int nSegment = 20;
		numVerts = 24 + (nSegment - 1) * 2;
		pt = new Point[numVerts];

		float xi = length, yi = height, zi = width, r = width / 2;
		Point p[12] = { Point(xi,yi,zi / 2), Point(xi,0,zi / 2),Point(0,yi,zi / 2),Point(0,0,zi / 2),
			Point(r * cos(5 * PI / 6), yi, r * sin(5 * PI / 6)),
			Point(r * cos(5 * PI / 6), 0, r * sin(5 * PI / 6)),
			Point(r * cos(7 * PI / 6), yi, r * sin(7 * PI / 6)),
			Point(r * cos(7 * PI / 6), 0, r * sin(7 * PI / 6)),
			Point(0,yi,-zi / 2),Point(0,0,-zi / 2),Point(xi,yi,-zi / 2),Point(xi,0,-zi / 2) };
		for (int i = 0; i < numVerts; i++) {
			pt[i] = p[i];
		}
		float angle = PI / nSegment;
		for (int i = 1; i < (nSegment - 1) * 2; i += 2) {
			pt[11 + i] = Point(r * cos(-PI / 2 + (i / 2 + 1) * angle) + xi, yi, r * sin(-PI / 2 + (i / 2 + 1) * angle));
			pt[12 + i] = Point(r * cos(-PI / 2 + (i / 2 + 1) * angle) + xi, 0, r * sin(-PI / 2 + (i / 2 + 1) * angle));
		}
		for (int i = 0; i < 12; i += 2) {
			pt[12 + (nSegment - 1) * 2 + i] = Point(r * cos(-PI / 6 + i / 2 * PI / 3), yi, r * sin(-PI / 6 + i / 2 * PI / 3));
			pt[12 + (nSegment - 1) * 2 + i + 1] = Point(r * cos(-PI / 6 + i / 2 * PI / 3), 3 * yi, r * sin(-PI / 6 + i / 2 * PI / 3));
		}

		numFaces = 14 + nSegment;
		face = new Face[numFaces];
		int idx = 0;
		for (int i = 0; i < 12 + (nSegment - 1) * 2; i += 2) {
			if (i == 12 + (nSegment - 1) * 2 - 2) {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[face[idx].nVerts];
				face[idx].vert[0].Index = i;
				face[idx].vert[1].Index = i + 1;
				face[idx].vert[2].Index = 1;
				face[idx].vert[3].Index = 0;
			}
			else {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[face[idx].nVerts];
				face[idx].vert[0].Index = i;
				face[idx].vert[1].Index = i + 1;
				face[idx].vert[2].Index = i + 3;
				face[idx].vert[3].Index = i + 2;
			}
			idx++;
		}
		for (int i = 0; i < 12; i += 2) {
			if (i == 10) {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[face[idx].nVerts];
				face[idx].vert[0].Index = 12 + (nSegment - 1) * 2 + i;
				face[idx].vert[1].Index = 12 + (nSegment - 1) * 2 + i + 1;
				face[idx].vert[2].Index = 12 + (nSegment - 1) * 2 + 1;
				face[idx].vert[3].Index = 12 + (nSegment - 1) * 2;
			}
			else {
				face[idx].nVerts = 4;
				face[idx].vert = new VertexID[face[idx].nVerts];
				face[idx].vert[0].Index = 12 + (nSegment - 1) * 2 + i;
				face[idx].vert[1].Index = 12 + (nSegment - 1) * 2 + i + 1;
				face[idx].vert[2].Index = 12 + (nSegment - 1) * 2 + i + 3;
				face[idx].vert[3].Index = 12 + (nSegment - 1) * 2 + i + 2;
			}
			idx++;
		}
		face[idx].nVerts = 6 + nSegment - 1;
		face[idx].vert = new VertexID[face[idx].nVerts];
		for (int i = 0; i < 12 + (nSegment - 1) * 2; i += 2) {
			face[idx].vert[i / 2].Index = i;
		}
		idx++;
		face[idx].nVerts = 6 + nSegment - 1;
		face[idx].vert = new VertexID[face[idx].nVerts];
		for (int i = 1; i < 12 + (nSegment - 1) * 2; i += 2) {
			face[idx].vert[i / 2].Index = i;
		}
		idx++;
		face[idx].nVerts = 6;
		face[idx].vert = new VertexID[face[idx].nVerts];
		for (int i = 0; i < 12; i += 2) {
			face[idx].vert[i / 2].Index = 12 + (nSegment - 1) * 2 + 1 + i;
		}
		CalculateFacesNorm();
	}

	void Floor() {

	}
};

//object
Mesh cube1, cube2, cylinder1, cylinder2, cylinder3, myMesh1, myMesh2, myMesh3;
//object angle
float angleX = 0, angleY = 0;
//shape 1
float shape1Radius = 1.25, shape1Height = 0.2, shape1Padding = 0.2;
//cube
float cubeEdge = shape1Height / 2, maxVal = 1;
float cube1angle = 0.0, cube2angle = PI / 2, shape3angle = 0;
//shape 2
float shape2Llength = 1, shape2Slength = 0.6, shape2Lwidth = 0.25, shape2Swidth = 0.2, shape2Height = 0.125;
float shape2angle = 0.0;
//shape 3
float shape3Height = 0.1, shape3Length = maxVal / 2, shape3Width = shape1Padding;
//cylinder
float cylinderHeight = 0.02, cylinderrRadius = 0.08, cylinderAngle = 0;
//camera
float camera_dis, camera_height, camera_angle;
//mode
bool drawMode = 1, viewMode = 1;
//view port
float viewPortWidth = 1000.0, viewPortheight = 500.0;
//color
GLfloat red[] = { 1.0f, 0.0f, 0.0f,1.0f };
GLfloat green[] = { 0.0f, 1.0f, 0.0f,1.0f };
GLfloat blue[] = { 0.0f, 0.0f, 1.0f,1.0f };
GLfloat golden[] = { 1.0f, 0.5f, 0.25f };
GLfloat black[] = { 0.0f, 0.0f, 0.0f,1.0f };
GLfloat white[] = { 1.0f, 1.0f, 1.0f,1.0f };
GLfloat yellow[] = { 1.0f, 1.0f, 0.0f,1.0f };
GLfloat purple[] = { 1.0f, 0.0f, 1.0f,1.0f };
GLfloat almostpink[] = { 1.0f, 0.0f, 0.5f, 1.0f };
GLfloat gray[] = { 0.5f, 0.5f, 0.5f,1.0f };


void createObjects() {
	cube1.Cube(cubeEdge, Color(0, 255, 0));
	cube2.Cube(cubeEdge, Color(0, 255, 0));
	cylinder1.Cylinder(cylinderHeight, cylinderrRadius, Color(255, 0, 0));
	cylinder2.Cylinder(cylinderHeight, cylinderrRadius, Color(255, 0, 0));
	cylinder3.Cylinder(cylinderHeight, cylinderrRadius, Color(255, 0, 0));
	myMesh1.Shape1(shape1Height, shape1Radius, shape1Padding, Color(255, 0, 0));
	myMesh2.Shape2(shape2Llength, shape2Slength, shape2Lwidth, shape2Swidth, shape2Height, Color(128, 128, 128));
	myMesh3.Shape3(shape3Length, shape3Width, shape3Height, Color(0, 0, 255));
}

void setupMaterial(float ambient[], float diffuse[], float specular[], float shiness) {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiness);
}


void draw() {
	if (drawMode) {
		setupMaterial(red, red, red, 100);
		myMesh1.Draw();

		glPushMatrix();
		glTranslatef(maxVal * cos(cube1angle), cubeEdge, 0);
		setupMaterial(green, green, green, 100);
		cube1.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, cubeEdge, maxVal * cos(cube2angle));
		setupMaterial(green, green, green, 100);
		cube2.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(maxVal * cos(cube1angle) / 2, shape1Height, maxVal * cos(cube2angle) / 2);
		glRotatef(shape2angle, 0, 1, 0);
		setupMaterial(gray, gray, gray, 100);
		myMesh2.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, shape3Height + cubeEdge + shape2Height + cylinderHeight, 0);
		glRotatef(shape3angle, 0, 1, 0);
		setupMaterial(blue, blue, blue, 100);
		myMesh3.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.5 * cos(cylinderAngle), 6 * cylinderHeight + cubeEdge + shape2Height + shape3Height, -0.5 * sin(cylinderAngle));
		setupMaterial(red, red, red, 100);
		cylinder1.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(cos(cylinderAngle), cubeEdge + shape2Height + shape3Height, 0);
		setupMaterial(red, red, red, 100);
		cylinder2.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, cubeEdge + shape2Height + shape3Height, -sin(cylinderAngle));
		setupMaterial(red, red, red, 100);
		cylinder3.Draw();
		glPopMatrix();
	}
	else {
		setupMaterial(black, black, black, 100);
		myMesh1.DrawWireFrame();

		glPushMatrix();
		glTranslatef(maxVal * cos(cube1angle), cubeEdge, 0);
		cube1.DrawWireFrame();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, cubeEdge, maxVal * cos(cube2angle));
		cube2.DrawWireFrame();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(maxVal * cos(cube1angle) / 2, shape1Height, maxVal * cos(cube2angle) / 2);
		glRotatef(shape2angle, 0, 1, 0);
		myMesh2.DrawWireFrame();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, shape3Height + cubeEdge + shape2Height + cylinderHeight, 0);
		glRotatef(shape3angle, 0, 1, 0);
		myMesh3.DrawWireFrame();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.5 * cos(cylinderAngle), 6 * cylinderHeight + cubeEdge + shape2Height + shape3Height, -0.5 * sin(cylinderAngle));
		cylinder1.DrawWireFrame();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(cos(cylinderAngle), cubeEdge + shape2Height + shape3Height, 0);
		cylinder2.DrawWireFrame();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, cubeEdge + shape2Height + shape3Height, -sin(cylinderAngle));
		cylinder3.DrawWireFrame();
		glPopMatrix();
	}
}

void view(bool viewMode) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (viewMode) {
		gluLookAt(camera_dis * cos(camera_angle), camera_height, camera_dis * sin(camera_angle), 0, 0, 0, 0, 1, 0);
	}
	else {
		gluLookAt(0, 5.0, 0, 0, 0, 0, 0, 0, 1);
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view(viewMode);

	glRotatef(angleX, 1, 0, 0);
	glRotatef(angleY, 0, 1, 0);

	glViewport(0, 0, viewPortWidth, viewPortheight);

	draw();

	glFlush();
}

void keyBoardFunc(unsigned char key, int x, int y) {
	switch (key)
	{
	case '1':
		angleY += 5;
		break;
	case '2':
		angleY -= 5;
		break;
	case '3':
		angleX += 5;
		break;
	case '4':
		angleX -= 5;
		break;
	case '5':
		cube1angle += 2 * PI / 100;
		cube2angle += 2 * PI / 100;
		shape2angle -= 3.6;
		shape3angle += 3.6;
		cylinderAngle += 2 * PI / 100;
		break;
	case '6':
		cube1angle -= 2 * PI / 100;
		cube2angle -= 2 * PI / 100;
		shape2angle += 3.6;
		shape3angle -= 3.6;
		cylinderAngle -= 2 * PI / 100;
		break;
	case 'w':
	case 'W':
		drawMode = !drawMode;
		break;
	case 'v':
	case 'V':
		viewMode = !viewMode;
		break;
	case '+':
		camera_dis += 0.1;
		break;
	case '-':
		camera_dis -= 0.1;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void mySpecialKey(int theKey, int x, int y) {
	if (theKey == GLUT_KEY_DOWN)
		camera_height -= 0.1;
	if (theKey == GLUT_KEY_UP)
		camera_height += 0.1;
	if (theKey == GLUT_KEY_LEFT)
		camera_angle -= 0.1;
	if (theKey == GLUT_KEY_RIGHT)
		camera_angle += 0.1;
	glutPostRedisplay();
}

void init() {
	//camera setup
	camera_angle = PI / 3;
	camera_height = 5.0;
	camera_dis = 5.0;

	GLfloat	lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1 };
	GLfloat	lightSpecular[] = { 0.5f, 0.5f, 0.5f, 1 };
	GLfloat	lightAmbient1[] = { 0.2f, 0.2f, 0.2f, 1 };
	GLfloat	lightAmbient2[] = { 0.2f, 0.2f, 0.2f, 1 };
	GLfloat light_position1[] = { 50.0f, 50.0f, 50.0f, 1 };
	GLfloat light_position2[] = { 0.0f, -100.0f, 0.0f, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, viewPortWidth / viewPortheight, 2.0f, 1000.0f);
}

void description() {
	cout << "1, 2	: Xoay ban quay xung quanh truc Y cuc bo" << endl;
	cout << "3, 4	: Xoay ban quay xung quanh truc X cuc bo" << endl;
	cout << "5, 6	: Xoay tay quay" << endl;
	cout << "W, w : Chuyen doi qua lai giua che do khung day va to mau" << endl;
	cout << "V, v : Chuyen doi qua lai giua che do 2D va 3D" << endl;
	cout << "+	: Tang khoang cach camera" << endl;
	cout << "-	: Tang khoang cach camera" << endl;
	cout << "up arrow  : Tang chieu cao camera" << endl;
	cout << "down arrow: Giam chieu cao camera" << endl;
	cout << "<-		   : Quay camera theo chieu kim dong ho" << endl;
	cout << "->		   : Quay camera nguoc chieu kim dong ho" << endl;
}

int  main(int argc, char** argv)
{
	description();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 500);
	glutCreateWindow("Huynh Nguyen Hieu Nghia - 1712314");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyBoardFunc);
	glutSpecialFunc(mySpecialKey);
	createObjects();
	init();
	glutMainLoop();
}
