#pragma once

#include <boost/shared_ptr.hpp>

#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>
#include <cg/io/point.h>

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
    public:
        vertex(bool is_inf)
            : is_inf(is_inf)
        {}

        vertex(point_2t<Scalar> p)
            : is_inf(false), p(p)
        {}

        EP<Scalar> incedent_edge;
        bool is_inf;
        point_2t<Scalar> p;
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
        cell() {
            vps.push_back(VP<Scalar>(new vertex<Scalar>(true)));
        }

        void add(point_2t<Scalar> p) {
            if (contains(p)) {
                std::cerr << "contains: " << p << std::endl;
                return;
            }
            VP<Scalar> a(VP<Scalar>(new vertex<Scalar>(p)));
            vps.push_back(a);
        }

    private:
        bool contains(point_2t<Scalar> const & p) {
            if (std::any_of(vps.begin(), vps.end(),
                    [&p](VP<Scalar> vp) { return vp->p == p; })
               )
            {
                  return true;
            }
            return false;
        }

        std::vector<FP<Scalar> > fps;
        std::vector<VP<Scalar> > vps;
    };

    template<class Scalar>
    class delaunay_triangulation {
    public:
        delaunay_triangulation()
        {}

        void add_point(point_2t<Scalar> point) {
            std::cerr << "add_point: " << point << std::endl;
            d_cell.add(point);
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
