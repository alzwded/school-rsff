#include "drawing.hxx"
#include <GL/freeglut.h>

Drawing::Drawing(int window)
    : currentPoint_(0.f, 0.f, 0.f)
    , color_(YELLOW)
    , window_(window)
    , rx(30.f), ry(-15.f), px(0.f), py(0.f)
    //, rx(0.f), ry(0.f), px(0.f), py(0.f)
{
    // setup
	glClearColor(.0, .0, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //                                 w/h
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-10.0, 10.0, -10.0, 10.0, -1.0, -20.0);
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
    //gluPerspective(90.f, 1.f, -1.0, -20.0);

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glEnable(GL_DEPTH_TEST);

    // rotate
	glRotatef(-ry, 1.0f, 0.0f, 0.0f); // yCamera
	glRotatef(-rx, 0.0f, 1.0f, 0.0f); //Roteste camera
	glTranslatef(-px - 5.f, -5.0f, py);

    glLineWidth(3.0);
#define SETFUCKINGCOLOR(R, G, B) glColor3f( (R) / 255.f , (G) / 255.f , (B) / 255.f )
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
    glBegin(GL_LINES);
    glLineWidth(1.0);
    glVertex3f(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glVertex3f(p.x, p.y, -p.z);
    glEnd();
}

void Drawing::Cube(float l)
{
    glPushMatrix();
    glLineWidth(0.5);
    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glutWireCube(l);
    glPopMatrix();
}
