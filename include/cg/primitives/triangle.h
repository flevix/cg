#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cg/operations/distance.h>

#include <boost/array.hpp>

namespace cg
{
   template <class Scalar>
   struct triangle_2t;

   typedef triangle_2t<double> triangle_2;

   template <class Scalar>
   struct triangle_2t
   {
      triangle_2t() {}
      triangle_2t(point_2t<Scalar> const & a, point_2t<Scalar> const & b, point_2t<Scalar> const & c)
         : pts_( {{a, b, c}} ) {}

      point_2t<Scalar> &         operator [] (size_t id)       { return pts_[id]; }
      point_2t<Scalar> const &   operator [] (size_t id) const { return pts_[id]; }

      segment_2t<Scalar> side(size_t id) const { return segment_2t<Scalar>(pts_[(id + 1) % 3], pts_[(id + 2) % 3]); }

      point_2t<Scalar> center() {
          point_2t<Scalar> a = pts_[0];
          point_2t<Scalar> b = pts_[1];
          point_2t<Scalar> c = pts_[2];
          Scalar ab = cg::distance(a, b);
          Scalar bc = cg::distance(b, c);
          Scalar ac = cg::distance(a, c);
          Scalar d = ab + bc + ac;

          Scalar x = (a.x * bc + b.x * ac + c.x * ab) / d;
          Scalar y = (a.y * bc + b.y * ac + c.y * ab) / d;
          return point_2t<Scalar>(x, y);
      }

      Scalar circumradius() {
          return cg::distance(center(), pts_[0]);
      }

   private:
      boost::array<point_2t<Scalar>, 3 > pts_;
   };

   template <class Scalar>
   bool operator == (triangle_2t<Scalar> const &a, triangle_2t<Scalar> const &b)
   {
      return (a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]);
   }

   template <class Scalar>
   bool operator != (triangle_2t<Scalar> const &a, triangle_2t<Scalar> const &b)
   {
      return !(a == b);
   }
}
