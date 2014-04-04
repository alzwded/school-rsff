#include "drawing.hxx"

Drawing::Drawing()
    : currentPoint_(0.f, 0.f, 0.f)
    , color_(WHITE)
{
}

void Drawing::SetColor(Color c)
{
    color_ = c;
}

void Drawing::MoveTo(Point3D p)
{
    currentPoint_ = p;
}

void Drawing::LineTo(Point3D p)
{
}

void Drawing::Cube(float l)
{
}
