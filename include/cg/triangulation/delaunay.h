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
    public:
        edge()
        {}
        edge(VP<Scalar> fst)
            : first_point(fst)
        {}

        VP<Scalar> first_point;
        EP<Scalar> twin;
        EP<Scalar> next;
        FP<Scalar> incedent_face;
    };

    template<class Scalar>
    class face {
    public:
        face()
            : is_inf(false)
        {}
        face(VP<Scalar> a, VP<Scalar> b, VP<Scalar> c) {
            EP<Scalar> eps[3] = {
                EP<Scalar>(new edge<Scalar>(a)),
                EP<Scalar>(new edge<Scalar>(b)),
                EP<Scalar>(new edge<Scalar>(c))
            };
            eps[0]->incedent_face = FP<Scalar>(this);
            eps[1]->incedent_face = FP<Scalar>(this);
            eps[2]->incedent_face = FP<Scalar>(this);

            eps[0]->next = eps[1];
            eps[1]->next = eps[2];
            eps[2]->next = eps[0];
            incedent_edge = eps[0];
        }

        bool contains_inf() {
            return incedent_edge->first_point->is_inf
                    || incedent_edge->next->first_point->is_inf
                    || incedent_edge->next->next->first_point->is_inf;
        }

        triangle_2t<Scalar> get_triangle() {
            return triangle_2t<Scalar>(
                        incedent_edge->first_point->p,
                        incedent_edge->next->first_point->p,
                        incedent_edge->next->next->first_point->p
                        );
        }

        EP<Scalar> incedent_edge;
        bool is_inf;
    };

    template<class Scalar>
    class cell {
    public:
        cell() {
            vps.push_back(VP<Scalar>(new vertex<Scalar>(true)));
        }

        bool add(point_2t<Scalar> p) {
            if (contains(p)) {
                std::cerr << "contains: " << p << std::endl;
                return false;
            }
            VP<Scalar> new_VP(VP<Scalar>(new vertex<Scalar>(p)));
            vps.push_back(new_VP);
            if (vps.size() < 4) {
                if (vps.size() == 3) {
                    init();
                }
                return true;
            }

            return true;
        }

        std::vector<triangle_2t<Scalar> > get_triangles() {
            std::vector<triangle_2t<Scalar> > trs;
            for (FP<Scalar> fp : fps) {
                if (fp->contains_inf()) {
                    continue;
                }
                trs.push_back(fp->get_triangle());
            }
            return trs;
        }

    private:
        void init() {
            FP<Scalar> face1(new face<Scalar>(vps[0], vps[2], vps[1]));
            FP<Scalar> face2(new face<Scalar>(vps[0], vps[1], vps[2]));
            EP<Scalar> inc_edg1 = face1->incedent_edge;
            EP<Scalar> inc_edg2 = face2->incedent_edge;
            set_twins(inc_edg1, inc_edg2);
            set_twins(inc_edg1->next, inc_edg2->next);
            set_twins(inc_edg1->next->next, inc_edg2->next->next);
            fps.push_back(face1);
            fps.push_back(face2);
        }

        void set_twins(EP<Scalar> ep1, EP<Scalar> ep2) {
            ep1->twin = ep2;
            ep2->twin = ep1;
        }

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
            if (d_cell.add(point)) {
                std::cerr << "add_point: " << point << std::endl;
            } else {
                std::cerr << "point: " << point << " contains" << std::endl;
            }
        }

        std::vector<triangle_2t<Scalar> > get_triangulation() {
            std::cerr << "get_triangulation" << std::endl;;
            return d_cell.get_triangles();
        }

    private:
        cell<Scalar> d_cell;
        std::vector<point_2t<Scalar> > points;
        std::vector<triangle_2t<Scalar> > triangles;
    };
}    
