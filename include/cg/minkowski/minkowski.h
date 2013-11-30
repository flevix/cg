#pragma once

#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>
#include <cg/io/point.h>
#include <boost/utility.hpp>
#include <iostream>

namespace cg {
    template <typename Scalar>
    contour_2t<Scalar> get_minkowski_sum(contour_2t<Scalar> const& obstacle,
                                         contour_2t<Scalar> const& robot) {
        common::range_circulator<contour_2t<Scalar> > it_o
                = obstacle.circulator(std::min_element(
                                          obstacle.begin(), obstacle.end()));
        common::range_circulator<contour_2t<Scalar> > it_r
                = robot.circulator(std::min_element(
                                       robot.begin(), robot.end()));
        common::range_circulator<contour_2t<Scalar> > it_o_next
                                                        = boost::next(it_o);
        common::range_circulator<contour_2t<Scalar> > it_r_next
                                                        = boost::next(it_r);

        contour_2t<Scalar> sum;
        size_t max_points = obstacle.size() + robot.size();

        while (sum.size() < max_points) {
            sum.add_point(*it_o + *it_r);
            if (orientation4(*it_o, *it_o_next, *it_r, *it_r_next) == CG_LEFT) {
                it_o++;
                it_o_next++;
            } else {
                it_r++;
                it_r_next++;
            }
        }
        return sum;
    }
}
