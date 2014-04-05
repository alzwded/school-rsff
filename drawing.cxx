#include "drawing.hxx"
#include <GL/freeglut.h>
#include <cmath>

#define COS(U) cosf( U / 180.f * 3.14159 )
#define SIN(U) sinf( U / 180.f * 3.14159 )
#define SETFUCKINGCOLOR(R, G, B) glColor3f( (R) / 255.f , (G) / 255.f , (B) / 255.f )

static int window;
static void (*drawFunc_)(Drawing&) = NULL;
static void (*updateFunc_)() = NULL;

static void (*onmousedown)(int, int, int) = NULL;
static void (*onmouseup)(int, int, int) = NULL;
static void (*onmousemove)(int, int) = NULL;
static void (*onkeydown)(char) = NULL;
static void (*onkeyup)(char) = NULL;

static int windowW = 1000, windowH = 1000;

// , rx(30.f), ry(-15.f), px(0.f), py(-0.f)
//static float rx = 30.f, ry = -15.f, px = 0.f, py = 0.f, pz = 0.f;
static float rx = 0.f, ry = 0.f, px = 0.f, py = 0.f, pz = 0.f;
static float dx = 0.f, dy = 0.f, dz = 0.f, drx = 0.f, dry = 0.f;

static void draw_()
{
    Drawing dwg(window);
    if(drawFunc_) drawFunc_(dwg);
    glutSwapBuffers();
}

static void update_(int)
{
    if(updateFunc_) updateFunc_();

    if(fabs(drx) > 1.e-5) {
        rx = rx + drx;
        drx = 0.f;
    }
    if(fabs(dry) > 1.e-5) {
        if(dry > 0.f) {
            if(ry + dry < 90.f) {
                ry = ry + dry;
            }
        } else {
            if(ry + dry > -90.f) {
                ry = ry + dry;
            }
        }
        dry = 0.f;
    }
    if(fabs(dx) > 1.e-5) {
        px = px + dx;
        dx = 0.f;
    }
    if(fabs(dy) > 1.e-5) {
        py = py + dy;
        dy = 0.f;
    }
    if(fabs(dz) > 1.e-5) {
        pz = pz + dz;
        dz = 0.f;
    }

    glutPostRedisplay();
    glutTimerFunc(17, update_, 0);
}

void Drawing::SetVelocity(float _dx, float _dy, float _dz)
{
    float M = sqrtf(_dx * _dx + _dy * _dy + _dz * _dz);
    float theta = atan2f(_dz, _dx); // yaw
    float fi = atan2f(sqrtf(_dx * _dx + _dz * _dz), _dy); // pitch
    //fi = fi - ry / 180.f * 3.14159; // leave this alone
    theta = theta - rx / 180.f * 3.14159;
    float ax = M * sinf(fi) * cosf(theta);
    float az = M * sinf(fi) * sinf(theta);
    float ay = M * cosf(fi);
    dx = ax;
    dy = ay;
    dz = az;
}

void Drawing::SetOnMouseUp( void (*f)(int x, int y, int button))
{
    onmouseup = f;
}
void Drawing::SetOnMouseDown( void (*f)(int x, int y, int button))
{
    onmousedown = f;
}
void Drawing::SetOnMouseMove( void (*f)(int dx, int dy))
{
    onmousemove = f;
}
void Drawing::SetOnKeyDown( void (*f)(char key))
{
    onkeydown = f;
}
void Drawing::SetOnKeyUp( void (*f)(char key))
{
    onkeyup = f;
}

void Drawing::SetRotationalVelocity(float _rx, float _ry)
{
    drx = _rx;
    dry = _ry;
}

static void initGL()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000); 

    window = glutCreateWindow("RSFF-VMAMZ");
}

static void initRendering()
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

static void handleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); 

	glLoadIdentity(); 
	gluPerspective(60.0, (double)w / (double)h, 1.5, 20.0); 
    //glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);

	//wSize = std::make_pair(w, h);
    windowW = w;
    windowH = h;
}

static void handleKeyRelease(unsigned char key, int x, int y)
{
    if(onkeyup) onkeyup(key);
}

static void handleKeyPress(unsigned char key, int x, int y)
{
    if(onkeydown) onkeydown(key);
}

static void handleMouse(int button, int state, int X, int Y)
{
    int btn = 0;
    switch(button) {
    case GLUT_LEFT_BUTTON:
        btn = 1;
        break;
    case GLUT_MIDDLE_BUTTON:
        btn = 2;
        break;
    case GLUT_RIGHT_BUTTON:
        btn = 3;
        break;
    }

    int x = 0, y = 0;
    x = (int)((float)X / windowW * 1000.f);
    y = (int)((float)Y / windowH * 1000.f);

    switch(state) {
    case GLUT_UP:
        if(onmouseup) onmouseup(x, y, btn);
        break;
    case GLUT_DOWN:
        if(onmousedown) onmousedown(x, y, btn);
        break;
    }
}

static void handleMouseMotion(int X, int Y)
{
    int x = 0, y = 0;
    x = (int)((float)X / windowW * 1000.f);
    y = (int)((float)Y / windowH * 1000.f);
    if(onmousemove) onmousemove(x, y);
}

static void initGL2()
{
	//glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(handleKeyPress);
	glutMouseFunc(handleMouse);
	glutPassiveMotionFunc(handleMouseMotion);
	glutMotionFunc(handleMouseMotion);
	glutKeyboardUpFunc(handleKeyRelease);
	glutReshapeFunc(handleResize);

	//glutTimerFunc(25, update, 0);
}

void Drawing::Init(int* argc, char* argv[])
{
    glutInit(argc, argv);
    initGL();
    initRendering();
    initGL2();
}

void Drawing::Loop(
        void (*update)(),
        void (*draw)(Drawing&))
{
    drawFunc_ = draw;
    glutDisplayFunc(draw_);
    glutTimerFunc(17, update_, 0);
    glutMainLoop();
}

Drawing::Drawing(int window)
    : currentPoint_(0.f, 0.f, 0.f)
    , color_(YELLOW)
    , window_(window)
    , textScale(0.9f)
{
    // setup
	glClearColor(.0, .0, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	gluPerspective(60.0, (double)1000 / (double)1000, 1.5, 20.0); 

    // rotate around center point
    float x, y, z;
    x = +5.f * SIN(-rx) * COS(-ry);
    y = +5.f * SIN(-ry);
    z = +5.f * COS(-rx) * COS(-ry);
    gluLookAt(
            -px + x, py + y, pz + z,
            -px, py, pz,
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

void Drawing::MoveTo(Point2D p)
{
    currentPoint_.x = p.x;
    currentPoint_.y = p.y;
}

void Drawing::LineTo(Point3D p)
{
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex3f(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glVertex3f(p.x, p.y, -p.z);
    glEnd();
}

static void EnterOverlayContext()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100.0f, 100.0f, 0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

static void LeaveOverlayContext()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Drawing::LineTo(Point2D p)
{
    EnterOverlayContext();

    glLineWidth(3.0);

    glBegin(GL_LINES);
    glVertex2f(currentPoint_.x, currentPoint_.y);
    glVertex2f(p.x, p.y);
    glEnd();

    LeaveOverlayContext();
}

void Drawing::SetTextScale(float s)
{
    textScale = s / 100.f / 5.f;
}

void Drawing::Text(std::string const& s)
{
    EnterOverlayContext();

    glPushMatrix();
    glLineWidth(0.5);
    glTranslatef(currentPoint_.x, currentPoint_.y, 0.f);
	glScalef(textScale, -textScale, 0.0f);
	for(int i = 0; i < s.size(); ++i) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, s[i]);
	}
    glPopMatrix();

    LeaveOverlayContext();
}

void Drawing::Cube(float l)
{
    glPushMatrix();
    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glLineWidth(2);
    glutWireCube(l);
    glPopMatrix();
}
