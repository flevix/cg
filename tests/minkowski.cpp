#include <gtest/gtest.h>

#include <cg/io/point.h>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/minkowski/minkowski.h>

#include <boost/assign/list_of.hpp>
#include <vector>
#include <algorithm>
#include <iterator>

TEST(minkowski, test1)
{
   using cg::point_2;
   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(2, 0))
                                                  (point_2(1, 1));

   std::vector<point_2> b = boost::assign::list_of(point_2(-2, -2))
                                                  (point_2(2, -2))
                                                  (point_2(2, 2))
                                                  (point_2(-2, 2));

   std::vector<point_2> c = boost::assign::list_of(point_2(-2, -2))
                                                  (point_2(4, -2))
                                                  (point_2(4, 2))
                                                  (point_2(3, 3))
                                                  (point_2(-1, 3))
                                                  (point_2(-2, 2));
   auto ans = cg::contour_2(c);

   auto res = cg::get_minkowski_sum(cg::contour_2(a), cg::contour_2(b));
   EXPECT_TRUE( std::equal(ans.begin(), ans.end(), res.begin()) );
}

TEST(minkowski, test2)
{
   using cg::point_2;
   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(4, 0))
                                                  (point_2(4, 4))
                                                  (point_2(0, 4));

   std::vector<point_2> b = boost::assign::list_of(point_2(-2, -2))
                                                  (point_2(2, -2))
                                                  (point_2(2, 2))
                                                  (point_2(-2, 2));

   std::vector<point_2> c = boost::assign::list_of(point_2(-2, -2))
                                                  (point_2(6, -2))
                                                  (point_2(6, 6))
                                                  (point_2(-2, 6));
   auto ans = cg::contour_2(c);

   auto res = cg::get_minkowski_sum(cg::contour_2(a), cg::contour_2(b));
   EXPECT_TRUE( std::equal(ans.begin(), ans.end(), res.begin()) );
}
