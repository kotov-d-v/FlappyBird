//
// Created by Dmitry Kotov on 3/26/16.
//

#ifndef FLAPPYBIRD_MATHEX_H
#define FLAPPYBIRD_MATHEX_H

#define MRAND_MAX 32767

namespace math {

    int rand(void);

    void srand(unsigned int seed);

    template<typename T> T min(T x, T y) {return x < y ? x : y;}

    template<typename T> T max(T x, T y) {return x > y ? x : y;}

    struct Point
    {
        float x;
        float y;
        Point() : x(0), y(0) {}
        Point(float ax, float ay) : x(ax), y(ay) {}

        Point& operator+=(const Point& p);
        Point& operator-=(const Point& p);
        Point& operator*=(float a);

        friend Point operator+(const Point& p1, const Point& p2);
        friend Point operator-(const Point& p1, const Point& p2);
        friend float operator*(const Point& p1, const Point& p2);
        friend Point operator*(const Point& p, float a);
    };

    using Vector2D = Point;

    struct Circle
    {
        Point pt;
        float r;

        Circle() : r(0) {}

        Circle(const Point& point, float radius) : pt(point), r(radius) {}

        Circle(float x, float y, float radius) : pt(x, y), r(radius) {}

        bool contains(const Point& p) const;
    };

    struct Rect
    {
        float x0, x1, y0, y1;

        Rect() : x0(0), y0(0), x1(0), y1(0) {}

        Rect(const Circle&);

        Rect& add(const Rect&);

        // offset by point
        Rect& move(float dx, float dy);

        // move to point
        Rect& moveTo(float x, float y);

        Rect(float ax0, float ay0, float ax1, float ay1) : x0(ax0), y0(ay0), x1(ax1), y1(ay1) {}

        bool contains(const Point& p) const;

        bool intersects(const Rect&) const;

        float width() const {return x1 - x0;}

        float height() const {return y1 - y0;}

    };

    struct OrientedRect
    {
        Point origin;
        Vector2D side1;
        Vector2D side2;
    };

    float collisionTime(const Circle&, const Vector2D&, const Rect&);
};


#endif //FLAPPYBIRD_MATHEX_H
