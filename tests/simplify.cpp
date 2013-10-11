#include <cg/algorithms/simplify.h>
#include <gtest/gtest.h>
#include <vector>

using namespace cg;

TEST(simplify, test1) {
   std::vector<point_2> v {
      {0, 0}, {1, 1}, {2, 2}
   };

   std::vector<point_2> r;
   simplify(v.begin(), v.end(), std::back_inserter(r), 0.1);

   std::vector<point_2> s {
      {0, 0}, {2, 2}
   };
   ASSERT_EQ(s, r);
}

TEST(simplify, test2) {
   std::vector<point_2> v {
      {0, 0}, {1, 0}, {0, 0.001}
   };
   std::vector<point_2> r;
   simplify(v.begin(), v.end(), std::back_inserter(r), 0.5);

   ASSERT_EQ(v, r);
}

TEST(simplify, test3) {
   std::vector<point_2> v {
      {0, 0}, {1, 0}, {1, 0}
   };
   std::vector<point_2> r;
   simplify(v.begin(), v.end(), std::back_inserter(r), 0.1);

   std::vector<point_2> s {
      {0, 0}, {1, 0}
   };
   ASSERT_EQ(s, r);
}
