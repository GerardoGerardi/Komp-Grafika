#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

#include<vector>

using namespace std;
bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


#define M_PI 3.14159265358979323846
void DrawWall(double v1[], double v2[], bool, vector<double>);
void DrawTriangle(double v1[], double v2[], double v3[]);

double* CalculateNorm(double v1[], double v2[], double v3[])//������ ������� � �� �������������
{
	double a[] = { v1[0] - v2[0],v1[1] - v2[1],v1[2] - v2[2] };
	double b[] = { v3[0] - v2[0],v3[1] - v2[1],v3[2] - v2[2] };;
	double Result[] = { a[1] * b[2] - b[1] * a[2],(-1.0 * a[0] * b[2] + b[0] * a[2]),a[0] * b[1] - b[0] * a[1] }; //�������
	for (int i = 0; i < 3; i++)
		Result[i] = Result[i] / (sqrt(pow(Result[0], 2) + pow(Result[1], 2) + pow(Result[2], 2))); //������������
	return &Result[0];
}

void DrawPolygon(double v1[], double v2[], double v3[], double v4[], double v5[]) //�������
{
	glBegin(GL_POLYGON);
	glVertex3dv(v1);
	glVertex3dv(v2);
	glVertex3dv(v3);
	glVertex3dv(v4);
	glVertex3dv(v5);
	glEnd();
}

void DrawPartOfCuted(double v1[], double v2[], double v3[], double v1c[], double v2c[], double v3c[])
{
	glBegin(GL_TRIANGLES);
	glTexCoord2d(v2c[0], v2c[1]); glVertex3dv(v2);
	glTexCoord2d(v1c[0], v1c[1]); glVertex3dv(v1);
	glTexCoord2d(v3c[0], v3c[1]); glVertex3dv(v3);
	glEnd();
}

void DrawCuted(double v1[], double v2[], double v3[]) //���������
{ //double v1 v2 - �����
	double O[] = { 1.5, -0.16666666666666667, (v1[2] + v2[2]) / 2 }; //����� ����������
	double A[3];//��� ������ ���������� ���� �������������
	double C[3];//��� ������ ���������� ���� �������������
	double r = 8.3032791380541; //������ ����������
	int n = 10000; //���������� �������������,������� �� ����������
	double Ac[] = { 12.0 / 17, (5.0 / 16.0) };
	double Cc[2];
	double v3c[] = { (8.0 / 17), (12.0 / 16) };
	double  number = 0;
	double k = 1;
	
	for (int i = 0; i <= n; i++) {
		A[0] = r * cos((((2.0 * M_PI) / n) * i)) + O[0]; //������� ������� ����� � ���������� � ��� 1 �����
		A[1] = r * sin(((2.0 * (M_PI) / n) * i)) + O[1]; //������� ������� ����� � ���������� � ��� 1 �����
		A[2] = v1[2]; //������ ��� ����� ��� ����� ������ �� ����� ��������� (z)
		C[0] = r * cos((((2.0 * M_PI) / n) * (i + 1))) + O[0];//������� ������� ����� � ���������� � ��� 2 �����
		C[1] = r * sin(((2.0 * (M_PI) / n) * (i + 1))) + O[1];//������� ������� ����� � ���������� � ��� 2 �����
		C[2] = v1[2];//������ ��� ����� ��� ����� ������ �� ����� ��������� (z)
		if ((A[1] >= (-0.375 * A[0] + 8)) && ((C[1] >= (-0.375 * C[0] + 8))))
		{
			if (((A[2] + C[2]) / 2) != 0) 
			{
				number++;
			}
		}
	}
	for (int i = 0; i <= n; i++)
	{
		A[0] = r * cos(((( 2*M_PI) / n) * i)) + O[0]; //������� ������� ����� � ���������� � ��� 1 �����
		A[1] = r * sin((( 2*(M_PI) / n) * i)) + O[1]; //������� ������� ����� � ���������� � ��� 1 �����
		A[2] = v1[2]; //������ ��� ����� ��� ����� ������ �� ����� ��������� (z)
		C[0] = r * cos(((2*(M_PI) / n) * (i + 1))) + O[0];//������� ������� ����� � ���������� � ��� 2 �����
		C[1] = r * sin((( 2*(M_PI) / n) * (i + 1))) + O[1];//������� ������� ����� � ���������� � ��� 2 �����
		C[2] = v1[2];//������ ��� ����� ��� ����� ������ �� ����� ��������� (z)

		if ((A[1] >= (-0.375 * A[0] + 8)) && ((C[1] >= (-0.375 * C[0] + 8)))) 
		{
			if (((A[2] + C[2]) / 2) != 0) //�������� ������� �� �� ��� �����
			{
				vector<double> points = { (number-700 -i + 0.0) / number, (number-700-i + 1.0) / number };
				DrawWall(A, C, false, points);
				glNormal3d(CalculateNorm(A, C, v3)[0], CalculateNorm(A, C, v3)[1], CalculateNorm(A, C, v3)[2]);
			}
			else
			{
				glNormal3d(CalculateNorm(A, v3, C)[0], CalculateNorm(A, v3, C)[1], CalculateNorm(A, v3, C)[2]);
			}
			Ac[0] = (r * cos((((2 * M_PI) / n) * i)) + O[0] + 4) / 17.0;
			Ac[1] = (r * sin((((2 * M_PI) / n) * i)) + O[1]) / 16.0;
			Cc[0] = (r * cos((((2 * M_PI) / n) * (i + 1.0))) + O[0] + 4) / 17.0;
			Cc[1] = (r * sin((((2 * M_PI) / n) * (i + 1.0))) + O[1]) / 16.0;
			DrawPartOfCuted(A, C, v3, Ac, Cc, v3c);
		}
	}
}

void DrawTriangle(double v1[], double v2[], double v3[]) //�����������
{
	glBegin(GL_TRIANGLES);
	glColor3d(0, 0, 0);
	glVertex3dv(v1);
	glVertex3dv(v2);
	glVertex3dv(v3);
	glEnd();
}

void DrawSquare(double v1[], double v2[], double v3[], double v4[], bool draw = true, vector<double> V = { 0,0 }) //�������
{
	glBegin(GL_QUADS);
	glColor3d(0.228, 0.1488, 0.322);
	if (draw) {
		glTexCoord2d(1, 1);
		glVertex3dv(v1);
		glTexCoord2d(0, 1);
		glVertex3dv(v2);
		glTexCoord2d(0, 0);
		glVertex3dv(v3);
		glTexCoord2d(1, 0);
		glVertex3dv(v4);
	}
	else {
		glTexCoord2d(V[0], 1);
		glVertex3dv(v1);
		glTexCoord2d(V[1], 1);
		glVertex3dv(v2);
		glTexCoord2d(V[0], 0);
		glVertex3dv(v3);
		glTexCoord2d(V[1], 0);
		glVertex3dv(v4);
	}
	glEnd();
}

void DrawFigure(double Figure[8][3]) //���������
{ 
	double M[] = { 3,8,0 };
	glNormal3d(CalculateNorm(Figure[0], Figure[1], Figure[2] )[0], CalculateNorm(Figure[0], Figure[1], Figure[2])[1], CalculateNorm(Figure[0], Figure[1], Figure[2])[2]);
	glBegin(GL_TRIANGLES);
	glTexCoord2d((8.0 / 17), (12.0 / 16)); glVertex3dv(Figure[3]);
	glTexCoord2d((12.0 / 17), (11.0 / 16)); glVertex3dv(Figure[2]);
	glTexCoord2d(12.0 / 17, (5.0 / 16.0)); glVertex3dv(Figure[6]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glTexCoord2d((8.0 / 17), (12.0 / 16)); glVertex3dv(Figure[3]);
	glTexCoord2d((4.0 / 17), (8.0 / 16)); glVertex3dv(Figure[5]);
	glTexCoord2d((6.0 / 17), (1)); glVertex3dv(Figure[4]);
	glEnd();
	DrawCuted(Figure[6], Figure[5], Figure[3]); //�����
	glBegin(GL_POLYGON);
	glTexCoord2d(1, (15.0 / 16)); glVertex3dv(Figure[1]);
	glTexCoord2d((12.0 / 17), (11.0 / 16)); glVertex3dv(Figure[2]);
	glTexCoord2d(12.0 / 17, (5.0 / 16.0)); glVertex3dv(Figure[6]);
	glTexCoord2d(13.0 / 17, 0); glVertex3dv(Figure[0]);
	glEnd();
}

void DrawWall(double v1[], double v2[], bool draw = true, vector<double> V = { 0,0 }) //������ 1 �����
{
	double tmp1[3];
	double tmp2[3];
	tmp1[0] = v1[0];
	tmp1[1] = v1[1];
	tmp1[2] = v1[2] - 4;
	tmp2[0] = v2[0];
	tmp2[1] = v2[1];
	tmp2[2] = v2[2] - 4;
	glNormal3d(CalculateNorm(v1, tmp1, v2)[0], CalculateNorm(v1, tmp1, v2)[1], CalculateNorm(v1, tmp1, v2)[2]);
	if (draw) {
		DrawSquare(v1, v2, tmp2, tmp1);
	}
	else {
		DrawSquare(v1, v2, tmp2, tmp1, false, V);
	}
}

void DrawWalls(double Figure[7][3], int n) //����� ��� �������� ������
{
	for (int i = 0; i <= n; i++)
	{
		if (i == n)
			DrawWall(Figure[0], Figure[i]);
		else
			if (i != 5)
				DrawWall(Figure[i + 1],Figure[i]);

	}
}

void DrawUpperFigure(double Figure[7][3]) //����� ��������
{
	for (int i = 0; i < 7; i++)
		Figure[i][2] = Figure[i][2] + 4;
	glNormal3d(CalculateNorm(Figure[0], Figure[2], Figure[1])[0], CalculateNorm(Figure[0], Figure[2], Figure[1])[1], CalculateNorm(Figure[0], Figure[2], Figure[1])[2]);
	glBegin(GL_TRIANGLES);
	glTexCoord2d((8.0 / 17), (12.0 / 16)); glVertex3dv(Figure[3]);
	glTexCoord2d((12.0 / 17), (11.0 / 16)); glVertex3dv(Figure[2]);
	glTexCoord2d(12.0 / 17, (5.0 / 16.0)); glVertex3dv(Figure[6]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glTexCoord2d((8.0/ 17), (12.0 / 16)); glVertex3dv(Figure[3]);
	glTexCoord2d((4.0/ 17), (8.0 / 16)); glVertex3dv(Figure[5]);
	glTexCoord2d((6.0/17), (1)); glVertex3dv(Figure[4]);
	glEnd();
	DrawCuted(Figure[6], Figure[5], Figure[3]); //�����
	glBegin(GL_POLYGON);
	glTexCoord2d(1,(15.0/16)); glVertex3dv(Figure[1]);
	glTexCoord2d((12.0 / 17), (11.0 / 16)); glVertex3dv(Figure[2]);
	glTexCoord2d(12.0 / 17, (5.0 / 16.0)); glVertex3dv(Figure[6]);
	glTexCoord2d(13.0/17,0); glVertex3dv(Figure[0]);
	glEnd();
}

void DrawPartOfCircle(double v1[], double v2[],double v3[],double v1c[],double v3c[])
{
	//double v2c[] = { 12.0 / 16.0,12.0 / 16.0 };
	int n = 1000;
	glBegin(GL_TRIANGLES);
	glTexCoord2d(v1c[0], v1c[1]); glVertex3dv(v1);
	glTexCoord2d(5.0 / 17.0, 12.0 / 16.0); glVertex3dv(v2);
	glTexCoord2d(v3c[0], v3c[1]); glVertex3dv(v3);
	glEnd();
}

void DrawHalfOfCircle(double v2[], double v3[], int m, double l) //���������� 
{ // 2 3 - �����
	double v1[3];
	double v4[3];
	double v2c[] = { 6.0/17, 1};
	double v4c[2];
	double r; //������
	double n = 1000;
	double k;//���� ������� �����
	if ((v2[0] - v3[0]) != 0)
	{
		k = (v2[1] - v3[1]) / (v2[0] - v3[0]); //�� 90 ��������
	}
	else
		k = atan(M_PI / 2.0); //90 ��������
	v1[0] = (v3[0] + v2[0]) / 2;
	v1[1] = (v3[1] + v2[1]) / 2;//��������
	v1[2] = (v3[2] + v2[2]) / 2;
	v1[0] = v1[0];
	r = sqrt(pow(v3[0] - v2[0], 2) + pow(v3[1] - v2[1], 2) + pow(v3[2] - v2[2], 2)) / 2;
	for (int i = 1; i <= n; i++)
	{
		v4[0] = r * cos((atan(k) + l * ((M_PI) / n) * i)) + v1[0];//x
		v4[1] = r * sin((atan(k) + l * ((M_PI) / n) * i)) + v1[1];//y
		v4[2] = m;//������
		glNormal3d(CalculateNorm(v2, v1, v4)[0], CalculateNorm(v2, v1, v4)[1], CalculateNorm(v2, v1, v4)[2]);
		//DrawTriangle(v2, v1, v4);
		v4c[0] = (r * cos((atan(k) + l * ((M_PI) / n) * i)) + (v1[0])+4)/17.0;//x
		v4c[1] = (r * sin((atan(k) + l * ((M_PI) / n) * i)) + v1[1])/16.0;//y
		DrawPartOfCircle(v2,v1,v4, v4c,v2c);
		v2c[0] = v4c[0];
		v2c[1] = v4c[1];
		vector<double> point = { (i + 0.0) / n, (i + 1.0) / n };
		DrawWall(v4, v2, false, point);
		v1[2] = v1[2] - m;
		v2[2] = v2[2] - m;
		v4[2] = v4[2] - m;
		glNormal3d(CalculateNorm(v2, v4, v1)[0], CalculateNorm(v2, v4, v1)[1], CalculateNorm(v2, v4, v1)[2]);
		DrawPartOfCircle(v2, v1, v4, v4c, v2c);
		
		v1[2] = v1[2] + m;
		v2[2] = v2[2] + m;
		v4[2] = v4[2] + m;
		//������������ ������ ����� � ����� ������ ��� ������ ����� ����� ������������ ������ ����� ������ ����������
		v2[0] = v4[0];
		v2[1] = v4[1];
		v2[2] = v4[2];
	}
}
//////////////////////////////////////



void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);



	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	///////////////////////////////////
	
	////////////////////
	double Figure[7][3] = { {9,0,0},{13,15,0},{8,11,0},{4,12,0},{2,16,0},{0,8,0},{8,5,0} };
	double M[] = { 3,8,0 };
	glBindTexture(GL_TEXTURE_2D, texId);
	DrawFigure(Figure);
	DrawUpperFigure(Figure);
	DrawWalls(Figure, 6);
	DrawHalfOfCircle(Figure[4], Figure[5], 4, 1);//����������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}