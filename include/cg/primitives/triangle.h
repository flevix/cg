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

      point_2t<Scalar> circumcenter() {
          point_2t<Scalar> a = pts_[0], b = pts_[1], c = pts_[2];
          Scalar ma = a.x * a.x + a.y * a.y;
          Scalar mb = b.x * b.x + b.y * b.y;
          Scalar mc = c.x * c.x + c.y * c.y;
          Scalar d = ( a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y) ) * 2;
          Scalar x = (ma * (b.y - c.y) + mb * (c.y - a.y) + mc * (a.y - b.y)) / d;
          Scalar y = (ma * (c.x - b.x) + mb * (a.x - c.x) + mc * (b.x - a.x)) / d;

          return point_2t<Scalar>(x, y);
      }

      Scalar circumradius() {
          return cg::distance(circumcenter(), pts_[0]);
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
