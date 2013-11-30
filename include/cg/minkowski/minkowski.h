#pragma once

#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>
#include <cg/io/point.h>

#include <iostream>

namespace cg {
        template <class Scalar>
        contour_2t<Scalar> get_minkovsky_sum(contour_2t<Scalar> const & obstacle,
                                             contour_2t<Scalar> const & robot) {
                contour_2t<Scalar> sum =
                        contour_2t<Scalar>(std::vector<point_2t<Scalar> >());
                return sum;
        }
}
