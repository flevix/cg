#pragma once

#include <boost/shared_ptr.hpp>

#include <cg/operations/orientation.h>
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

        bool contains_inf() {
            return first_point->is_inf || next->first_point->is_inf;
        }

        bool less(EP<Scalar> ep) {
           point_2t<Scalar> edg1_fp = first_point->p;
           point_2t<Scalar> edg1_sp = next->first_point->p;
           point_2t<Scalar> edg2_fp = ep->first_point->p;
           point_2t<Scalar> edg2_sp = ep->next->first_point->p;
           return cg::orientation(edg1_fp,
                                  edg1_sp,
                                  edg1_sp + (edg2_sp - edg2_fp))
                                                                == cg::CG_RIGHT;
        }

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

        bool add(point_2t<Scalar> const & p) {
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
            std::pair<EP<Scalar>, int> fst_and_count = get_first_edge(get_faces(p), p);

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
        std::pair<EP<Scalar>, int> get_first_edge(std::vector<int> fs_index,
                                                  point_2t<Scalar> const & p) {
            EP<Scalar> first_edge;
            int count_vertex;

            if (fps[fs_index[0]]->is_inf) {
                std::pair<EP<Scalar>, EP<Scalar> > min_max_ep
                        = get_minmax_ep(p, fs_index);

                EP<Scalar> cur_edge = min_max_ep.first;
                if (fs_index.size() != 1) {
                   min_max_ep.second->next->next = min_max_ep.first->next->next;

                   for (int i = 0; i < fs_index.size() - 1; i++) {
                      cur_edge->next = cur_edge->next->twin->next;
                      cur_edge = cur_edge->next;
                   }
                }

                first_edge = min_max_ep.first;
                count_vertex = fs_index.size() + 2;

                std::vector<bool> to_delete(fps.size(), false);
                std::vector<FP<Scalar>> new_fps;
                for (int cur_index : fs_index) {
                   to_delete[cur_index] = true;
                }
                for (int i = 0; i < fs_index.size(); i++) {
                   if (!to_delete[i]) new_fps.push_back(fps[i]);
                }
                fps.swap(new_fps);
            } else {
                if (fs_index.size() < 2) {
                    first_edge = fps[fs_index[0]]->incedent_edge;
                    count_vertex = 3;

                    fps.erase(fps.begin() + fs_index[0]);
                } else { //fs_index.size == 2 -> point on the edge
                    EP<Scalar> cur_edge = fps[fs_index[0]]->incedent_edge;
                    EP<Scalar> common_edge;

                    for (int i = 0; i < 3; i++, cur_edge = cur_edge->next) {
                        EP<Scalar> cur_snd_edge = fps[fs_index[1]]->incedent_edge;
                        for (int j = 0; j < 3; j++, cur_snd_edge = cur_snd_edge->next) {
                            if (cur_snd_edge->twin == cur_edge) {
                                common_edge = cur_edge;
                            }
                        }
                    }
                    common_edge->next->next->next = common_edge->twin->next;
                    common_edge->twin->next->next->next = common_edge->next;

                    first_edge = common_edge->next;
                    count_vertex = 4;

                    fps.erase(fps.begin() + fs_index[0]);
                    fps.erase(fps.begin() + fs_index[1]);
                }
            }
            return std::pair<EP<Scalar>, int>(first_edge, count_vertex);
        }

        std::pair<EP<Scalar>, EP<Scalar> > get_minmax_ep
                                          (point_2t<Scalar> const & p,
                                           std::vector<int> fs_index) {
            std::pair<EP<Scalar>, EP<Scalar>> min_max_ep;
            for (size_t i = 0; i < fs_index.size(); i++) {
                EP<Scalar> cur = fps[fs_index[i]]->incedent_edge;
                for (int j = 0; j < 3; j++) {
                    if (!cur->contains_inf() &&
                            cg::orientation(cur->first_point->p,
                                            cur->next->first_point->p,
                                            p) != CG_RIGHT) {
                        if (min_max_ep.second == nullptr) {
                            min_max_ep.first = cur;
                            min_max_ep.second = cur;
                        } else {
                            if (cur->less(min_max_ep.first)) {
                                min_max_ep.first = cur;
                            }
                            if (min_max_ep.second->less(cur)) {
                                min_max_ep.second = cur;
                            }
                        }
                        break;
                    }
                    cur = cur->next;
                }
            }
            return min_max_ep;
        }

        std::vector<int> get_faces(point_2t<Scalar> const & p) {
            std::vector<int> result;
            for (size_t i = 0; i < fps.size(); i++) {
                FP<Scalar> f = fps[i];
                f->is_inf = false;
                EP<Scalar> cur = f->incedent_edge;
                bool good_face = true;
                for (int j = 0; j < 3; j++) {
                    if (cur->contains_inf()) {
                        cur = cur->next;
                        f->is_inf = true;
                        continue;
                   }

                   if (cg::orientation(cur->first_point->p,
                                       cur->next->first_point->p,
                                       p) == CG_RIGHT) {
                       good_face = false;
                       break;
                   }
                   cur = cur->next;
                }

                if (good_face) {
                    result.push_back(i);
                }
            }
            return result;
        }

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
            return d_cell.get_triangles();
        }

    private:
        cell<Scalar> d_cell;
        std::vector<point_2t<Scalar> > points;
        std::vector<triangle_2t<Scalar> > triangles;
    };
}    
