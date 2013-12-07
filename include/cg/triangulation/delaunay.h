#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <cg/operations/orientation.h>
#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>
#include <cg/io/point.h>
#include <cg/triangulation/predicat.h>

#include <iostream>

namespace cg
{
    template<class Scalar>
    class vertex;
    template<class Scalar>
    using VP = boost::shared_ptr<vertex<Scalar> >;

    template<class Scalar>
    class edge;
    template<class Scalar>
    using EP = boost::shared_ptr<edge<Scalar> >;

    template<class Scalar>
    class face;
    template<class Scalar>
    using FP = boost::shared_ptr<face<Scalar> >;

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

        bool contains_vertex(VP<Scalar> v) {
            return first_point == v
                   || next->first_point == v
                   || next->next->first_point == v;
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

            int count_vertex = fst_and_count.second;
            EP<Scalar> cur_edge = fst_and_count.first;

            std::vector<EP<Scalar>> new_EPS(count_vertex);
            std::vector<FP<Scalar>> new_FPS(count_vertex);
            add_face(count_vertex, cur_edge, new_FPS, new_EPS, new_VP);
            add_edge(count_vertex, cur_edge, new_FPS, new_EPS);

            EP<Scalar> edge = cur_edge;
            for (int i = 0; i < count_vertex; i++, edge = edge->next->twin->next) {
                fix_edge(edge);
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
        bool is_edge_bad(EP<Scalar> e) {
            return cg::in_circle(e->first_point->p,
                                 e->next->first_point->p,
                                 e->next->next->first_point->p,
                                 e->twin->next->next->first_point->p);
        }

        void fix_edge(EP<Scalar> e) {
            if (e->contains_inf() || e->twin->next->next->first_point->is_inf) {
                return;
            }
            if (is_edge_bad(e)) {
                EP<Scalar> f1_edge = e->twin->next;
                EP<Scalar> f2_edge = e->twin->next->next;

                flip(e);

                fix_edge(f1_edge);
                fix_edge(f2_edge);
            }
        }

        void flip(EP<Scalar> f_edge) {
            EP<Scalar> new_EP(new edge<Scalar>(f_edge->next->next->first_point));
            EP<Scalar> twin_EP(new edge<Scalar>(f_edge->twin->next->next->first_point));
            set_twins(new_EP, twin_EP);

            FP<Scalar> fst_face = f_edge->incedent_face;
            FP<Scalar> snd_face = f_edge->twin->incedent_face;

            fst_face->incedent_edge = f_edge->next;
            snd_face->incedent_edge = f_edge->twin->next;
            f_edge->next->next->incedent_face = snd_face;
            f_edge->twin->next->next->incedent_face = fst_face;

            new_EP->incedent_face = fst_face;
            twin_EP->incedent_face = snd_face;
            new_EP->next = f_edge->twin->next->next;
            twin_EP->next = f_edge->next->next;

            f_edge->next->next->next = f_edge->twin->next;
            f_edge->next->next = new_EP;
            f_edge->twin->next->next->next = f_edge->next;
            f_edge->twin->next->next = twin_EP;
        }

        void add_edge(int count_vertex, EP<Scalar> cur_edge,
                      std::vector<FP<Scalar>> & new_FPS,
                      std::vector<EP<Scalar>> & new_EPS) {
            for (int i = 0; i < count_vertex; i++) {
               EP<Scalar> tmp = cur_edge->next;

               new_EPS[i]->next = cur_edge;
               new_EPS[i]->twin->next = new_EPS[(i + count_vertex - 1) % count_vertex];
               cur_edge->next = new_EPS[(i + 1) % count_vertex]->twin;

               new_EPS[i]->incedent_face = new_FPS[i];
               new_EPS[i]->twin->incedent_face = new_FPS[(i + count_vertex - 1) % count_vertex];
               cur_edge->incedent_face = new_FPS[i];

               cur_edge = tmp;
            }
        }

        void add_face(int count_vertex, EP<Scalar> cur_edge,
                      std::vector<FP<Scalar>> & new_FPS,
                      std::vector<EP<Scalar>> & new_EPS,
                      VP<Scalar> new_VP) {
            for (int i = 0; i < count_vertex; i++) {
                EP<Scalar> new_EP(new edge<Scalar>(new_VP));
                EP<Scalar> twin_EP(new edge<Scalar>(cur_edge->first_point));

                set_twins(new_EP, twin_EP);
                new_EPS[i] = new_EP;

                face<Scalar> * cur_face = new face<Scalar>();
                new_FPS[i] = FP<Scalar>(cur_face);
                cur_face->incedent_edge = cur_edge;
                fps.push_back(new_FPS[i]);

                cur_edge = cur_edge->next;
            }
        }

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

                   for (int i = 0; i < fs_index.size() - 1;
                                    i++,
                                    cur_edge = cur_edge->next) {
                      cur_edge->next = cur_edge->next->twin->next;
                   }
                }

                first_edge = min_max_ep.first;
                count_vertex = fs_index.size() + 2;

                std::vector<bool> to_delete(fps.size(), false);
                for (int cur_index : fs_index) {
                   to_delete[cur_index] = true;
                }

                std::vector<FP<Scalar>> new_fps;
                for (int i = 0; i < fps.size(); i++) {
                   if (!to_delete[i]) {
                       new_fps.push_back(fps[i]);
                   }
                }
                fps.swap(new_fps);
            } else { //in face
                first_edge = fps[fs_index[0]]->incedent_edge;
                count_vertex = 3;

                fps.erase(fps.begin() + fs_index[0]);
            }
            return std::pair<EP<Scalar>, int>(first_edge, count_vertex);
        }

        std::pair<EP<Scalar>, EP<Scalar> > get_minmax_ep
                                          (point_2t<Scalar> const & p,
                                           std::vector<int> fs_index) {
            std::pair<EP<Scalar>, EP<Scalar>> min_max_ep;
            for (size_t i = 0; i < fs_index.size(); i++) {
                if (!fps[fs_index[i]]->is_inf) {
                    continue;
                }
                EP<Scalar> cur = fps[fs_index[i]]->incedent_edge;
                for (int j = 0; j < 3; j++, cur = cur->next) {
                    if (!cur->contains_inf()) {
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
                }
            }
            return min_max_ep;
        }

        std::vector<int> get_faces(point_2t<Scalar> const & p) {
            std::vector<int> result;
            bool was_inf = false;
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
                if (!good_face) {
                    continue;
                }
                if (f->is_inf && !was_inf) {
                    was_inf = true;
                }
                if (was_inf) {
                    if (f->is_inf) {
                        result.push_back(i);
                    }
                } else {
                    result.push_back(i);
                    return result;
                }
            }
            return result;
        }

        void init() {
            FP<Scalar> face1 = create_face(vps[0], vps[1], vps[2]);
            FP<Scalar> face2 = create_face(vps[1], vps[0], vps[2]);

            EP<Scalar> inc_edg1 = face1->incedent_edge;
            EP<Scalar> inc_edg2 = face2->incedent_edge;
            set_twins(inc_edg1, inc_edg2);
            set_twins(inc_edg1->next, inc_edg2->next->next);
            set_twins(inc_edg2->next, inc_edg1->next->next);
            fps.push_back(face1);
            fps.push_back(face2);
        }

        FP<Scalar> create_face(VP<Scalar> a, VP<Scalar> b, VP<Scalar> c) {
            FP<Scalar> n_face = boost::make_shared< face<Scalar> >();
            EP<Scalar> eps[3] = {
                boost::make_shared< edge<Scalar> >(a),
                boost::make_shared< edge<Scalar> >(b),
                boost::make_shared< edge<Scalar> >(c),
            };

            eps[0]->incedent_face = FP<Scalar>(n_face);
            eps[1]->incedent_face = FP<Scalar>(n_face);
            eps[2]->incedent_face = FP<Scalar>(n_face);

            eps[0]->next = eps[1];
            eps[1]->next = eps[2];
            eps[2]->next = eps[0];
            n_face->incedent_edge = eps[0];

            return n_face;
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
//                std::cerr << "add_point: " << point << std::endl;
            } else {
//                std::cerr << "point: " << point << " contains" << std::endl;
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
