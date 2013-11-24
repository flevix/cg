#pragma once

#include <boost/shared_ptr.hpp>

#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>

#include <iostream>

namespace cg
{
    template<class Scalar>
    class vertex;
    template<class Scalar>
    using VP = std::shared_ptr<vertex<Scalar> >;

    template<class Scalar>
    class edge;
    template<class Scalar>
    using EP = std::shared_ptr<edge<Scalar> >;

    template<class Scalar>
    class face;
    template<class Scalar>
    using FP = std::shared_ptr<face<Scalar> >;

    template<class Scalar>
    class vertex {
        vertex() {}
    };

    template<class Scalar>
    class edge {
        edge() {}
    };

    template<class Scalar>
    class face {
        face() {}
    };

    template<class Scalar>
    class cell {
    public:
        cell()
        {}

    private:
    };

    template<class Scalar>
    class delaunay_triangulation {
    public:
        delaunay_triangulation()
        {}

        void add_point(point_2t<Scalar> point) {
            std::cerr << "add_point" << std::endl;
        }

        std::vector<triangle_2t<Scalar> > get_triangulation() {
            std::cerr << "get_triangulation" << std::endl;
            std::vector<triangle_2t<Scalar> > t;
            return t;
        }

    private:
        cell<Scalar> d_cell;
        std::vector<point_2t<Scalar> > points;
        std::vector<triangle_2t<Scalar> > triangles;
    };
}
