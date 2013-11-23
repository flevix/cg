#include "stdafx.h"

#include <cmath>
#include "cg/visualization/draw_util.h"

#include "cg/common/range.h"

#include "cg/io/point.h"

namespace cg {
namespace visualization
{
   void draw(drawer_type & drawer, rectangle_2f const & rect)
   {
      drawer.draw_line(rect.corner(0, 0), rect.corner(1, 0));
      drawer.draw_line(rect.corner(1, 0), rect.corner(1, 1));
      drawer.draw_line(rect.corner(1, 1), rect.corner(0, 1));
      drawer.draw_line(rect.corner(0, 1), rect.corner(0, 0));
   }

   void draw(drawer_type & drawer, contour_2f const & cnt, bool draw_vertices)
   {
      contour_circulator beg(cnt), it = beg;

      do
      {
         point_2f pt = *it;
         if (draw_vertices)
            drawer.draw_point(pt, 3);
         ++it;
         drawer.draw_line(segment_2f(pt, *it));
      }
      while (it != beg);
   }

   void draw(drawer_type & drawer, triangle_2 const & tr)
   {
       drawer.draw_line(tr[0], tr[2]);
       drawer.draw_line(tr[2], tr[1]);
       drawer.draw_line(tr[1], tr[0]);
   }

   void draw(drawer_type & drawer, point_2f const & center, const double radius)
   {
        const double PI = 3.14159265358979323846;
        const int MAX_ANGLE = 360;
        cg::point_2f current(center.x + radius, center.y);

        for (int angle = 0; angle < MAX_ANGLE; angle++)
        {
            double alpha = angle * PI / (MAX_ANGLE / 2);
            cg::point_2f next(center.x + radius * cos(alpha), center.y + radius * sin(alpha));
            drawer.draw_line(current, next);
            current = next;
        }
   }
}}
