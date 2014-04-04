#ifndef DRAWING_HXX
#define DRAWING_HXX

#include "core.hxx"

class Drawing {
public:
    enum Color {
        CYAN = 0x0000FF,
        SALMON = 0xFF8080,
        YELLOW = 0xFFFF00,
        LIME = 0x00FF00,
        WHITE = 0xFFFFFF
    } color_;
    Point3D currentPoint_;
public:
    Drawing();
    void SetColor(Color);
    void MoveTo(Point3D);
    void LineTo(Point3D);
    void Cube(float);
};

#endif
