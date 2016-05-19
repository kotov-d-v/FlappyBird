//
// Created by Dmitry Kotov on 3/26/16.
//

#include "mathex.h"
#include <math.h>
#include <limits>

namespace math {

    static unsigned long int next_seed = 1;

    int rand(void)
    {
        next_seed = next_seed * 1103515245 + 12345;
        return (unsigned int)(next_seed/65536) % MRAND_MAX;
    }

    void srand(unsigned int seed)
    {
        next_seed = seed;
    }

    Point &Point::operator+=(const Point &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    Point &Point::operator-=(const Point &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    Point &Point::operator*=(float a)
    {
        x *= a;
        y *= a;
        return *this;
    }

    Point operator+(const Point &p1, const Point &p2)
    {
        Point x = p1;
        x += p2;
        return x;
    }

    Point operator-(const Point &p1, const Point &p2)
    {
        Point x = p1;
        x -= p2;
        return x;
    }

    float operator*(const Point &p1, const Point &p2)
    {
        return p1.x*p2.x + p1.y*p2.y;
    }

    Point operator*(const Point &p, float a)
    {
        Point x = p;
        x *= a;
        return x;
    }

    bool Circle::contains(const Point &pt0) const
    {
        Point dp = pt - pt0;
        return dp.x*dp.x + dp.y*dp.y < r*r;
    }

    bool Rect::contains(const Point &p) const
    {
        return p.x > x0 && p.x < x1 && p.y > y0 && p.y < y1;
    }

    Rect::Rect(const Circle &cr)
    {
        x0 = cr.pt.x - cr.r;
        y0 = cr.pt.y - cr.r;
        x1 = cr.pt.x + cr.r;
        y1 = cr.pt.y + cr.r;
    }

    Rect& Rect::add(const Rect &rc)
    {
        x0 = min(x0, rc.x0);
        y0 = min(y0, rc.y0);
        x1 = max(x1, rc.x1);
        y1 = max(y1, rc.y1);

        return *this;
    }

// offset by point
    Rect& Rect::move(float dx, float dy)
    {
        x0 += dx;
        y0 += dy;
        x1 += dx;
        y1 += dy;

        return *this;
    }

// move to point
    Rect& Rect::moveTo(float x, float y)
    {
        x1 += x - x0;
        y1 += y - y0;
        x0 = x;
        y0 = y;

        return *this;
    }

    bool Rect::intersects(const Rect &rc) const
    {
        return x1 > rc.x0 && x0 < rc.x1 && y1 > rc.y0 && y0 < rc.y1;
    }

// solve equation: ax^2 + bx + c = 0
// stores roots in x (should be at least x[2])
// returns number of roots
    int solveEq(float a, float b, float c, float* x)
    {
        if(a == 0)
        {
            x[0] = -c/b;
            return 1;
        }

        float D = b*b - 4.f*a*c;
        if(D < 0) return 0;

        if(D == 0)
        {
            x[0] = -0.5f*b/a;
            return 1;
        }

        float sqrtD = sqrtf(D);

        x[0] = 0.5f*(sqrtD - b)/a;
        x[1] = -0.5f*(sqrtD + b)/a;

        return 2;
    }

// collision time for circle cr moving with velocity vel & point pt
    float collisionTime(const Circle &cr, const Vector2D& vel, const Point& pt)
    {
        const float infinity = std::numeric_limits<float>::max();

        float t[2];

        Vector2D d = cr.pt - pt;
        int N = solveEq(vel*vel, d*vel*2.f, d*d - cr.r*cr.r, t);

        if(N == 0)
            return infinity;

        if(N == 1)
            return t[0];

        float tmin = infinity;

        if(t[0] >= 0)
            tmin = t[0];

        if(t[1] >= 0)
            tmin = min(tmin, t[1]);

        return tmin;
    }

// collision time for circle cr moving with velocity vel & rectangle rc
    float collisionTime(const Circle &cr, const Vector2D& vel, const Rect &rc)
    {
        const float infinity = std::numeric_limits<float>::max();
        float t[6];

        t[0] = collisionTime(cr, vel, Point(rc.x0, rc.y0));
        t[1] = collisionTime(cr, vel, Point(rc.x0, rc.y1));
        t[2] = collisionTime(cr, vel, Point(rc.x1, rc.y1));
        t[3] = collisionTime(cr, vel, Point(rc.x1, rc.y0));

        math::Rect bb(cr); // bounding box of cr

// t[4] = intersection with x lines
        if(vel.x == 0)
        {
            t[4] = infinity;
        }
        else
        {
            if(vel.x > 0)
                t[4] = (rc.x0 - bb.x1)/vel.x; // this will be the common case
            else
                t[4] = (rc.x1 - bb.x0)/vel.x;

            float y = cr.pt.y + vel.y*t[4];

            if(y < rc.y0 || y > rc.y1)
                t[4] = infinity; // out of the segment
        }

// t[5] = intersection with y lines
        if(vel.y == 0)
        {
            t[5] = infinity;
        }
        else
        {
            if(vel.y > 0)
                t[5] = (rc.y0 - bb.y1)/vel.y; // this will be the common case
            else
                t[5] = (rc.y1 - bb.y0)/vel.y;

            float x = cr.pt.x + vel.x*t[5];

            if(x < rc.x0 || x > rc.x1)
                t[5] = infinity; // out of the segment
        }

        float tmin = infinity;
        for(int i = 0; i < 6; ++i)
        {
            if(t[i] < 0) continue;

            tmin = min(tmin, t[i]);
        }

        return tmin;
    }

}
