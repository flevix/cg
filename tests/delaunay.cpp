#include <gtest/gtest.h>
#include <cg/triangulation/delaunay.h>
#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>
#include "random_utils.h"
#include <cmath>

bool check(std::vector<cg::triangle_2> & ts)
{
    for (cg::triangle_2 tr : ts) {
        for (cg::triangle_2 p_tr : ts) {
            for (int i = 0; i < 3; i++) {
                cg::point_2 p = p_tr[i];
                if (tr[0] != p && tr[1] != p &&
                     tr[2] != p && cg::in_circle(tr[0], tr[1], tr[2], p)) {
                    return false;
                }
            }
        }
    }
    return true;
}

TEST(delaunay, test1) {
    cg::delaunay_triangulation<double> triangulation;
    std::vector<cg::point_2> pts = uniform_points(10);
    for (size_t i = 0; i < pts.size(); i++) {
        cg::point_2 p(pts[i].x, pts[i].x);
        triangulation.add_point(p);
        std::vector<cg::triangle_2> trs = triangulation.get_triangulation();
        EXPECT_TRUE(check(trs));
    }
}


TEST(delaunay, test2) {
    cg::delaunay_triangulation<double> triangulation;
    std::vector<cg::point_2> pts = uniform_points(100);
    double y = 100;
    for (size_t i = 0; i < pts.size(); i++) {
        cg::point_2 p(pts[i].x, y);
        triangulation.add_point(p);
        std::vector<cg::triangle_2> trs = triangulation.get_triangulation();
        EXPECT_TRUE(check(trs));
    }
}

TEST(delaunay, test3) {
    cg::delaunay_triangulation<double> triangulation;
    std::vector<cg::point_2> pts = uniform_points(100);
    double x = 100;
    for (size_t i = 0; i < pts.size(); i++) {
        cg::point_2 p(x, pts[i].y);
        triangulation.add_point(p);
        std::vector<cg::triangle_2> trs = triangulation.get_triangulation();
        EXPECT_TRUE(check(trs));
    }
}

TEST(delaunay, test4) {
    const double PI = 3.14159265358979323846;
    const int MAX_ANGLE = 360;
    cg::point_2f center(0, 0);
    double radius = 100;
    cg::point_2f current(center.x + radius, center.y);
    cg::delaunay_triangulation<double> triangulation;

    std::vector<cg::point_2> pts = uniform_points(100);
    for (size_t i = 0; i < pts.size(); i++)
    {
        double alpha = pts[i].x * PI / (MAX_ANGLE / 2);
        std::cerr << alpha << std::endl;
        cg::point_2f next(center.x + radius * cos(alpha), center.y + radius * sin(alpha));
        triangulation.add_point(next);
        current = next;

        std::vector<cg::triangle_2> trs = triangulation.get_triangulation();
        EXPECT_TRUE(check(trs));
    }
}
