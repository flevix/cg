#pragma once

#include "viewer.h"

#include "cg/primitives/rectangle.h"
#include "cg/primitives/contour.h"
#include "cg/primitives/triangle.h"

namespace cg {
namespace visualization
{
   void draw(drawer_type & drawer, cg::rectangle_2f    const & rect);
   void draw(drawer_type & drawer, cg::contour_2f      const & cnt, bool draw_vertices = false);
   void draw(drawer_type & drawer, cg::triangle_2      const & tr);
   void draw(drawer_type & drawer, cg::point_2f         const & center, const double radius);
}}
