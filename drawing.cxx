#include "drawing.hxx"
#include <GL/freeglut.h>
#include <cmath>

#define SETFUCKINGCOLOR(R, G, B) glColor3f( (R) / 255.f , (G) / 255.f , (B) / 255.f )

Drawing::Drawing(int window)
    : currentPoint_(0.f, 0.f, 0.f)
    , color_(YELLOW)
    , window_(window)
    , rx(30.f), ry(-15.f), px(0.f), py(-0.f)
    //, rx(30.f), ry(0.f), px(0.f), py(-0.f)
{
    // setup
	glClearColor(.0, .0, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);

    // rotate around center point
    float x, y, z;
#define COS(U) cosf( U / 180.f * 3.14159 )
#define SIN(U) sinf( U / 180.f * 3.14159 )
    x = +5.f * SIN(-rx) * COS(-ry);
    y = +5.f * SIN(-ry);
    z = +5.f * COS(-rx) * COS(-ry);
    gluLookAt(
            -px + x, y, py + z,
            -px, 0.f, py,
            0, 1, 0);

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glEnable(GL_DEPTH_TEST);

    SETFUCKINGCOLOR((color_ >> 16) % 256, (color_ >> 8) % 256, color_ % 256);
}

Drawing::~Drawing()
{
	glPopMatrix();
	glutSwapBuffers(); 
}

void Drawing::SetColor(Color c)
{
    color_ = c;
    SETFUCKINGCOLOR((color_ >> 16) % 256, (color_ >> 8) % 256, color_ % 256);
}

void Drawing::MoveTo(Point3D p)
{
    currentPoint_ = p;
}

void Drawing::LineTo(Point3D p)
{
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex3f(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glVertex3f(p.x, p.y, -p.z);
    glEnd();
}

void Drawing::Cube(float l)
{
    glPushMatrix();
    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glLineWidth(2);
    glutWireCube(l);
    glPopMatrix();
}
