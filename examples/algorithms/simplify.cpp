#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>
#include <cg/algorithms/simplify.h>

#include <cg/operations/has_intersection/segment_segment.h>

using cg::point_2;
using cg::point_2f;

namespace  {
   void draw_vector(std::vector<point_2> const& p, cg::visualization::drawer_type & drawer)
   {
      if (p.empty())
         return;

      auto b = p.begin();
      auto a = b++;
      while (b != p.end())
      {
         drawer.draw_line(*a++, *b++);
      }
   }
}

struct simplify_viewer : cg::visualization::viewer_adapter
{
   simplify_viewer()
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      draw_vector(points, drawer);
      drawer.set_color(Qt::blue);
      draw_vector(simplified, drawer);
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "Simplifying polyline."
                        << "Double click to clear, right-click to add point.";
   }

   bool on_press(const point_2f & p)
   {
      points.push_back({p.x, p.y});
      simplified.clear();
      cg::simplify(points.begin(), points.end(), std::back_inserter(simplified), 5.0);
      return true;
   }

   bool on_double_click(point_2f const&)
   {
      points.clear();
      simplified.clear();
      return true;
   }

   bool on_release(const point_2f & p)
   {
      return false;
   }

   bool on_move(const point_2f & p)
   {
      return false;
   }

private:

   std::vector<point_2> points;
   std::vector<point_2> simplified;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   simplify_viewer viewer;
   cg::visualization::run_viewer(&viewer, "Simplify");
}
