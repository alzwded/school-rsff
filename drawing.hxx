#ifndef DRAWING_HXX
#define DRAWING_HXX

#include "core.hxx"
#include <string>

class Drawing {
public:
    enum Color {
        CYAN = 0x00FFFF,
        SALMON = 0xFF8080,
        YELLOW = 0xFFFF00,
        LIME = 0x00FF00,
        WHITE = 0xFFFFFF
    } color_;
    Point3D currentPoint_;
    int window_;
    float textScale;

public:
    Drawing(int window);
    ~Drawing();
    void SetColor(Color);
    void MoveTo(Point3D);
    void LineTo(Point3D);
    void Cube(float);
    void MoveTo(Point2D);
    void LineTo(Point2D);
    void SetTextScale(float);
    void Text(std::string const&);

    static void SetVelocity(float dx, float dy, float dz);
    static void SetRotationalVelocity(float rx, float ry);

    static void SetOnMouseDown(
            void (*f)(int x, int y, int button));
    static void SetOnMouseUp(
            void (*f)(int x, int y, int button));
    static void SetOnMouseMove(
            void (*f)(int dx, int dy));
    static void SetOnKeyDown(
            void (*f)(char key));
    static void SetOnKeyUp(
            void (*f)(char key));

    static void Init(int* argc, char* argv[]);
    static void Loop(
            void (*update)(),
            void (*draw)(Drawing&));
};

#endif
