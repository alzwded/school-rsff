#include <cstdio>
#include <fstream>
#include <algorithm>
#include "core.hxx"
#include "pathfinder.hxx"
#include "drawing.hxx"
#include "parser.hxx"

static Beam::vector beams;
static Sensor::vector sensors;
static AnimationData ad;
#define AROUND(X, Y, RX, RY, T) (X > RX - T && X < RX + T && Y > RY - T && Y < RY + T)

bool toggle = false;

struct BeamDrawer {
    Drawing& dwg;
    BeamDrawer(Drawing& drawing)
        : dwg(drawing)
    {}
    void operator()(Beam const& o) const
    {
        dwg.MoveTo(o.firstPoint);
        dwg.LineTo(o.secondPoint);
    }
};

struct SensorDrawer
{
    Drawing& dwg;
    AnimationData const& ad;
    SensorDrawer(Drawing& drawing, AnimationData const& animationData)
        : dwg(drawing)
        , ad(animationData)
    {}
    void operator()(Sensor const& o) const
    {
        if(o.selected || ad.Sensors().find(&o) != ad.Sensors().end()) {
            dwg.SetColor(Drawing::LIME);
        } else {
            switch(o.type) {
            case Sensor::CENTRAL:
                dwg.SetColor(Drawing::SALMON);
                break;
            case Sensor::ROUTER:
                dwg.SetColor(Drawing::CYAN);
                break;
            case Sensor::SENSOR:
                dwg.SetColor(Drawing::YELLOW);
                break;
            }
        }
        dwg.MoveTo(o.location);
        dwg.Cube(0.1f);
    }
};

static void drawScene(Drawing& dwg)
{
    dwg.SetColor(Drawing::WHITE);
    std::for_each(beams.begin(), beams.end(), BeamDrawer(dwg));

    std::for_each(sensors.begin(), sensors.end(), SensorDrawer(dwg, ad));

    std::for_each(ad.Beams().begin(), ad.Beams().end(), BeamDrawer(dwg));
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
    dwg.MoveTo(Point2D(500, 0));
    dwg.LineTo(Point2D(500, 1000));
    dwg.MoveTo(Point2D(0, 500));
    dwg.LineTo(Point2D(1000, 500));

    dwg.MoveTo(Point2D(50, 30));
    dwg.SetTextScale(8);
    dwg.Text("Hello!");

    if(toggle) dwg.SetColor(Drawing::LIME);
    else dwg.SetColor(Drawing::WHITE);
    dwg.MoveTo(Point2D(770, 800));
    dwg.SetTextScale(3);
    dwg.Text("Click");
    dwg.MoveTo(Point2D(770, 770));
    dwg.LineTo(Point2D(770, 830));
    dwg.LineTo(Point2D(830, 830));
    dwg.LineTo(Point2D(830, 770));
    dwg.LineTo(Point2D(770, 770));
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
        if(!AROUND(x, y, 800, 800, 30)) {
            dragging = true;
        }
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
        if(AROUND(x, y, 800, 800, 30)) {
            toggle = !toggle;
        } else {
            dragging = false;
        }
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
        vx = (float)-dx / 100.f * 8.f;
        vz = (float)dy / 100.f * 8.f;
    }
    if(panningUp) {
        vy = (float)-dy / 100.f * 8.f;
    }

    Drawing::SetVelocity(vx, vy, vz);
}

int main(int argc, char* argv[])
{
    std::fstream f;
    if(argc != 2) {
        printf("usage: %s inputFile\n", argv[0]);
        return 1;
    } else {
        f.open(argv[1], std::ios::in);

        if(!f.good()) {
            printf("usage: %s inputFile\n", argv[0]);
            printf("Failed to open %s\n", argv[1]);
            return 2;
        }
    }

    Path p = Pathfinder::ComputePath();
    Point3D PP(0, 0, 0);

    Drawing::Init(&argc, argv);
    Drawing::SetOnMouseDown(onmousedown);
    Drawing::SetOnMouseUp(onmouseup);
    Drawing::SetOnMouseMove(onmousemove);

    Parser::Parse(f, beams, sensors);

    Drawing::Loop(NULL, drawScene);

    return 0;
}
