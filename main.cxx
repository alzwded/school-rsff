#include "core.hxx"
#include "pathfinder.hxx"
#include "drawing.hxx"

static void drawScene(Drawing& dwg)
{
#define GL_TEST_MODEL
#ifdef GL_TEST_MODEL
    dwg.MoveTo(Point3D(0.f,     1.f,   0.f));
    dwg.LineTo(Point3D(0.f,     -1.f,  0.f));
    dwg.MoveTo(Point3D(1.f,     0.f,   0.f));
    dwg.LineTo(Point3D(-1.f,    0.f,   0.f));
    dwg.MoveTo(Point3D(0.f,     0.f,   -1.f));
    dwg.LineTo(Point3D(0.f,    0.f,     1.f));

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

    dwg.SetColor(Drawing::WHITE);
    dwg.MoveTo(Point2D(50, 0));
    dwg.LineTo(Point2D(50, 100));
    dwg.MoveTo(Point2D(0, 50));
    dwg.LineTo(Point2D(100, 50));

    dwg.MoveTo(Point2D(10, 10));
    dwg.SetTextScale(8);
    dwg.Text("Hello!");
#endif
}

static bool dragging = false;
static bool panningUp = false;
static bool panning = false;
static int lastX = 0, lastY = 0;

static void onmousedown(int x, int y, int btn)
{
    lastX = x;
    lastY = y;
    if(btn == 1) {
        dragging = true;
    }
    if(btn == 2) {
        panningUp = true;
    }
    if(btn == 3) {
        panning = true;
    }
}

static void onmouseup(int x, int y, int btn)
{
    if(btn == 1) {
        dragging = false;
    }
    if(btn == 2) {
        panningUp = false;
    }
    if(btn == 3) {
        panning = false;
    }
}

static void onmousemove(int x, int y)
{
    int dx = x - lastX;
    int dy = y - lastY;
    lastX = x;
    lastY = y;
    if(dragging) {
        Drawing::SetRotationalVelocity(
                (float)dx / 100.f * 180.f,
                (float)-dy / 100.f * 180.f);
    }
    float vx = 0.f, vy = 0.f, vz = 0.f;
    if(panning) {
        vx = (float)-dx / 100.f * 4.f;
        vy = (float)-dy / 100.f * 4.f;
    }
    if(panningUp) {
        vz = (float)dy / 100.f * 4.f;
    }

    Drawing::SetVelocity(vx, vy, vz);
}

int main(int argc, char* argv[])
{
    Path p = Pathfinder::ComputePath();
    Point3D PP(0, 0, 0);

    Drawing::Init(&argc, argv);
    Drawing::SetOnMouseDown(onmousedown);
    Drawing::SetOnMouseUp(onmouseup);
    Drawing::SetOnMouseMove(onmousemove);

    Drawing::Loop(NULL, drawScene);

    return 0;
}
