#include <cstdio>
#include <fstream>
#include <algorithm>
#include <ctime>
#include "core.hxx"
#include "pathfinder.hxx"
#include "drawing.hxx"
#include "parser.hxx"

static bool dragging = false;
static bool panningUp = false;
static bool panning = false;
static int lastX = 0, lastY = 0;

static Beam::vector beams;
static Sensor::vector sensors;
static Button::vector buttons;
static AnimationData ad;

static Path path;
static int animFrame = 0;
static bool animating = false;
static bool showRange = false;

static Sensor::vector::iterator iSensor = sensors.end();

static bool clickEnabled = false;

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
        if(o.selected
                || ad.Sensors().find(&o) != ad.Sensors().end()
                || iSensor != sensors.end() && &o == &(*iSensor)
        ) {
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
        if(showRange 
                && o.type != Sensor::CENTRAL
                && ad.EmittingSensors().find(&o) != ad.EmittingSensors().end()) {
            dwg.Sphere(o.range);
        }
    }
};

struct ButtonDrawer
{
    Drawing& dwg;
    ButtonDrawer(Drawing& drawing)
        : dwg(drawing)
    {}

    void operator()(Button const& o)
    {
        if(o.highlighted) {
            dwg.SetColor(Drawing::LIME);
        } else {
            dwg.SetColor(Drawing::WHITE);
        }

        Point2D p2(o.extent.x, o.location.y);
        Point2D p4(o.location.x, o.extent.y);

        dwg.MoveTo(o.location);
        dwg.LineTo(p2);
        dwg.LineTo(o.extent);
        dwg.LineTo(p4);
        dwg.LineTo(o.location);

        Point2D textLocation(o.location.x + 3, o.extent.y - 3);
        dwg.MoveTo(textLocation);
        dwg.SetTextScale(5);
        dwg.Text(o.text);
    }
};

static void drawScene(Drawing& dwg)
{
    dwg.SetColor(Drawing::WHITE);
    std::for_each(beams.begin(), beams.end(), BeamDrawer(dwg));

    std::for_each(sensors.begin(), sensors.end(), SensorDrawer(dwg, ad));

    dwg.SetColor(Drawing::LIME);
    std::for_each(ad.Beams().begin(), ad.Beams().end(), BeamDrawer(dwg));

    ad.Clear();

    // Overlay
    dwg.SetColor(Drawing::WHITE);
    dwg.MoveTo(Point2D(10, 950));
    dwg.SetTextScale(5);
    dwg.Text("Selecteaza nodul de start:");
    std::for_each(buttons.begin(), buttons.end(), ButtonDrawer(dwg));
}

static void onmousedown(int x, int y, int btn)
{
    for(Button::vector::iterator i = buttons.begin();
            i != buttons.end(); ++i)
    {
        Button& btn = *i;
        if(!dragging && !panning && !panningUp
                && btn.location.x < x && btn.location.y < y
                && btn.extent.x > x && btn.extent.y > y)
        {
            clickEnabled = true;
            return;
        }
    }

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
    for(Button::vector::iterator i = buttons.begin();
            clickEnabled && i != buttons.end();
            ++i)
    {
        Button& btn = *i;
        if(!dragging && !panning && !panningUp
                && btn.location.x < x && btn.location.y < y
                && btn.extent.x > x && btn.extent.y > y)
        {
            if(btn.clicked) btn.clicked();
        }
    }

    if(clickEnabled) {
        clickEnabled = false;
        return;
    }

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
        vx = (float)-dx / 100.f * 8.f;
        vz = (float)dy / 100.f * 8.f;
    }
    if(panningUp) {
        vy = (float)-dy / 100.f * 8.f;
        vx = (float)-dx / 100.f * 8.f;
    }

    Drawing::SetVelocity(vx, vy, vz);

    for(Button::vector::iterator i = buttons.begin();
            i != buttons.end(); ++i)
    {
        Button& btn = *i;
        if(!dragging && !panning && !panningUp
                && btn.location.x < x && btn.location.y < y
                && btn.extent.x > x && btn.extent.y > y)
        {
            btn.highlighted = true;
        } else {
            btn.highlighted = false;
        }
    }
}

static void PrecClicked()
{
    if(iSensor == sensors.begin()) {
        iSensor = sensors.end();
    } else {
        --iSensor;
    }
    if(iSensor == sensors.end()) {
        printf("no sensor selected\n");
    } else {
        std::cout << "current sensor: " << iSensor - sensors.begin() + 1 << std::endl;
    }
    path.clear();
    if(animating) printf("stopping animation\n");
    animating = false;
}

static void UrmClicked()
{
    if(iSensor == sensors.end()) {
        iSensor = sensors.begin();
    } else {
        ++iSensor;
    }
    if(iSensor == sensors.end()) {
        printf("no sensor selected\n");
    } else {
        std::cout << "current sensor: " << iSensor - sensors.begin() + 1 << std::endl;
    }
    path.clear();
    if(animating) printf("stopping animation\n");
    animating = false;
}

static void sensordeselect(Sensor& s)
{
    s.selected = false;
}

static void AnimClicked()
{
    animating = !animating;
    if(path.size() == 0 && iSensor == sensors.end()) {
        printf("no sensor selected. Doing nothing\n");
        return;
    }
    if(animating) printf("starting animation\n");
    else printf("pausing animation\n");
    if(path.size() == 0) {
        printf("computing path...\n");
        clock_t ticks = clock();
        Pathfinder::SetStartingSensor(*iSensor);
        path = Pathfinder::ComputePath(sensors);
        ticks = clock() - ticks;
        std::cout << "path computed (" << ((float)ticks / CLOCKS_PER_SEC) << " seconds), number of frames: " << path.size() << std::endl;
        animFrame = 0;
    }
    if(animating && iSensor != sensors.end()) {
        std::for_each(sensors.begin(), sensors.end(), sensordeselect);
        iSensor = sensors.end();
    }
}

static void updateScene()
{
#define ACTUAL_FRAMES_PER_ANIMATION_FRAME 30
    static size_t frameCounter = 0;
    if(path.size() == 0) {
        frameCounter = 0;
        return;
    }

    if(animating && ++frameCounter % ACTUAL_FRAMES_PER_ANIMATION_FRAME == 0) {
        frameCounter = 0;
        animFrame = (animFrame + 1) % path.size();
    }

    for(Path::Edges_t::iterator i = path[animFrame].begin();
            i != path[animFrame].end(); ++i)
    {
        ad << *i;
    }
}

static void RangeClicked()
{
    showRange = !showRange;
    if(showRange) printf("showing sensor range as a sphere\n");
    else printf("showing only connections\n");
}

static void addButtons()
{
    buttons.push_back(Button(
                Point2D(10, 970),
                Point2D(60, 990),
                "Prec",
                PrecClicked));
    buttons.push_back(Button(
                Point2D(110, 970),
                Point2D(160, 990),
                "Urm",
                UrmClicked));
    buttons.push_back(Button(
                Point2D(700, 970),
                Point2D(980, 990),
                "Porneste/Opreste animatia",
                AnimClicked));
    buttons.push_back(Button(
                Point2D(700, 945),
                Point2D(980, 965),
                "Show range",
                RangeClicked));
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

    addButtons();

    Drawing::Init(&argc, argv);
    Drawing::SetOnMouseDown(onmousedown);
    Drawing::SetOnMouseUp(onmouseup);
    Drawing::SetOnMouseMove(onmousemove);

    Parser::Parse(f, beams, sensors);
    iSensor = sensors.end();

    Drawing::Loop(updateScene, drawScene);

    return 0;
}
