#include "core.hxx"
#include "pathfinder.hxx"
#include "drawing.hxx"
#include <GL/freeglut.h>

static int window;
void initGL()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000); 

    window = glutCreateWindow("RSFF-VMAMZ");
}

void drawScene()
{
    Drawing dwg(window);

#define GL_TEST_MODEL
#ifdef GL_TEST_MODEL
    dwg.MoveTo(Point3D(0.f,     1.f,   5.f));
    dwg.LineTo(Point3D(0.f,     -1.f,  5.f));
    dwg.MoveTo(Point3D(1.f,     0.f,   5.f));
    dwg.LineTo(Point3D(-1.f,    0.f,   5.f));
    dwg.MoveTo(Point3D(0.f,     0.f,   4.f));
    dwg.LineTo(Point3D(0.f,    0.f,    6.f));

    dwg.MoveTo(Point3D(0, 0, 5));
    dwg.SetColor(Drawing::LIME);
    dwg.Cube(.1);
    dwg.Cube(2.f);
    dwg.MoveTo(Point3D(0, 2, 5));
    dwg.SetColor(Drawing::WHITE);
    dwg.Cube(2.f);
    dwg.MoveTo(Point3D(2, 2, 5));
    dwg.SetColor(Drawing::SALMON);
    dwg.Cube(2.f);
    dwg.MoveTo(Point3D(4, 2, 5));
    dwg.SetColor(Drawing::CYAN);
    dwg.Cube(2.f);
    dwg.MoveTo(Point3D(4, 0, 5));
    dwg.Cube(2.f);
#endif
}

void initRendering()
{
	//initModel();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// lighting
	//initLight();

	// blending
	//initBlending();

	glClearColor(.25f, .25f, 1.f, 1.f);
}

void initGL2()
{
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(drawScene);
	//glutKeyboardFunc(handleKeypress);
	//glutMouseFunc(handleMouse);
	//glutPassiveMotionFunc(doRotate);
	//glutMotionFunc(doRotate);
	//glutReshapeFunc(handleResize);
	//glutKeyboardUpFunc(handleKeyRelease);

	//glutTimerFunc(25, update, 0);
}

int main(int argc, char* argv[])
{
    Path p = Pathfinder::ComputePath();
    Point3D PP(0, 0, 0);

    glutInit(&argc, argv);
    initGL();
    initRendering();
    initGL2();

    glutMainLoop();

    return 0;
}
