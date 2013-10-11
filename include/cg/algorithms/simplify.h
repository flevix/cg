#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/operations/distance.h>
#include <vector>

namespace cg {
   namespace detail {
      template<class Itr, class OutItr>
      OutItr douglas_peucker_impl(Itr begin, Itr end, OutItr out, double eps)
      {
         end--;
         if (begin == end)
            return out;

         segment_2 segment {*begin, *end};
         auto max_dist_itr = std::max_element(std::next(begin), end, [begin, end, &segment] (point_2 const& a, point_2 const& b)
         {
            return distance(segment, a) < distance(segment, b);
         });

         auto max_dist = distance(segment, *max_dist_itr);

         if (max_dist < eps) {
            *(out++) = *end;
            return out;
         } else
         {
            out = douglas_peucker_impl(begin, std::next(max_dist_itr), out, eps);
            return douglas_peucker_impl(max_dist_itr, ++end, out, eps);
         }
      }
   }

   template<class Itr, class OutItr>
   OutItr simplify(Itr begin, Itr end, OutItr out, double eps)
   {
      if (begin == end)
         return out;

      *out++ = *begin;
      return detail::douglas_peucker_impl(begin, end, out, eps);
   }
}
