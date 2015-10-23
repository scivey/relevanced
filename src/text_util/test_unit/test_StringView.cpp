#include "gtest/gtest.h"
#include "text_util/StringView.h"
#include "util/util.h"

using namespace std;
using namespace relevanced;
using namespace relevanced::text_util;
using namespace relevanced::util;


TEST(TestStringView, TestEquality1) {
  string dog1 {"dog"};
  string dog2 {"dog"};

  StringView view1(dog1.c_str(), dog1.size());
  StringView view2(dog2.c_str(), dog2.size());

  EXPECT_EQ(view1, view2);
  EXPECT_EQ(view2, view1);
}

TEST(TestStringView, TestEquality2) {
  string dog {"dog"};
  string cat {"cat"};
  string fish {"fish"};

  StringView view1(dog.c_str(), dog.size());
  StringView view2(cat.c_str(), cat.size());
  StringView view3(fish.c_str(), fish.size());

  EXPECT_FALSE(view1 == view2);
  EXPECT_FALSE(view2 == view1);
  EXPECT_FALSE(view1 == view3);
  EXPECT_FALSE(view3 == view1);
  EXPECT_FALSE(view2 == view3);
  EXPECT_FALSE(view3 == view2);

}