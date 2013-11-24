#pragma once

#include "cg/primitives/point.h"
#include <boost/numeric/interval.hpp>
#include <gmpxx.h>

#include <boost/optional.hpp>

namespace cg
{
    struct in_circle_d {
        boost::optional<bool> operator() (point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d) const {
            double a11 = a.x - d.x, a12 = a.y - d.y, a13 = (a.x * a.x - d.x * d.x) + (a.y * a.y - d.y * d.y);
            double a21 = b.x - d.x, a22 = b.y - d.y, a23 = (b.x * b.x - d.x * d.x) + (b.y * b.y - d.y * d.y);
            double a31 = c.x - d.x, a32 = c.y - d.y, a33 = (c.x * c.x - d.x * d.x) + (c.y * c.y - d.y * d.y);
            double d1 = a11 * a22 * a33 - a11 * a23 * a32;
            double d2 = a12 * a21 * a33 - a12 * a23 * a31;
            double d3 = a13 * a21 * a32 - a13 * a22 * a31;
            double res = d1 - d2 + d3;
            double eps = (fabs(d1) + fabs(d2) + fabs(d3)) * 16 * std::numeric_limits<double>::epsilon();

            if (res > eps) {
                return true;
            }
            if (res < -eps) {
                return false;
            }
            return boost::none;
        }
   };

    struct in_circle_i {
        boost::optional<bool> operator() (point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d) const {
            typedef boost::numeric::interval_lib::unprotect<boost::numeric::interval<double> >::type interval;

            boost::numeric::interval<double>::traits_type::rounding _;
            interval a11 = interval(a.x) - d.x, a12 = interval(a.y) - d.y,
            a13 = (interval(a.x) * a.x - interval(d.x) * d.x) + (interval(a.y) * a.y - interval(d.y) * d.y);
            interval a21 = interval(b.x) - d.x, a22 = interval(b.y) - d.y,
            a23 = (interval(b.x) * b.x - interval(d.x) * d.x) + (interval(b.y) * b.y - interval(d.y) * d.y);
            interval a31 = interval(c.x) - d.x, a32 = interval(c.y) - d.y,
            a33 = (interval(c.x) * c.x - interval(d.x) * d.x) + (interval(c.y) * c.y - interval(d.y) * d.y);
            interval d1 = a11 * a22 * a33 - a11 * a23 * a32;
            interval d2 = a12 * a21 * a33 - a12 * a23 * a31;
            interval d3 = a13 * a21 * a32 - a13 * a22 * a31;
            interval res = d1 - d2 + d3;

            if (res.lower() > 0) {
                return true;
            }
            if (res.upper() < 0) {
                return false;
            }
            if (res.upper() == res.lower()) {
                return false; // on border
            }
            return boost::none;
        }
    };

    struct in_circle_r {
        boost::optional<bool> operator() (point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d) const {
            mpq_class a11 = mpq_class(a.x) - d.x, a12 = mpq_class(a.y) - d.y,
                a13 = (mpq_class(a.x) * a.x - mpq_class(d.x) * d.x) + (mpq_class(a.y) * a.y - mpq_class(d.y) * d.y);
            mpq_class a21 = mpq_class(b.x) - d.x, a22 = mpq_class(b.y) - d.y,
                a23 = (mpq_class(b.x) * b.x - mpq_class(d.x) * d.x) + (mpq_class(b.y) * b.y - mpq_class(d.y) * d.y);
            mpq_class a31 = mpq_class(c.x) - d.x, a32 = mpq_class(c.y) - d.y,
                a33 = (mpq_class(c.x) * c.x - mpq_class(d.x) * d.x) + (mpq_class(c.y) * c.y - mpq_class(d.y) * d.y);
            mpq_class d1 = a11 * a22 * a33 - a11 * a23 * a32;
            mpq_class d2 = a12 * a21 * a33 - a12 * a23 * a31;
            mpq_class d3 = a13 * a21 * a32 - a13 * a22 * a31;
            mpq_class res = d1 - d2 + d3;

            int cres = cmp(res, 0);

            if (cres > 0) {
                return true;
            } else {
                return false;
            }
        }
   };

    inline bool in_circle(point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d) {
        if (boost::optional<bool> v = in_circle_d()(a, b, c, d)) {
            return *v;
        }
        if (boost::optional<bool> v = in_circle_i()(a, b, c, d)) {
            return *v;
        }
        return *in_circle_r()(a, b, c, d);
    }
}
