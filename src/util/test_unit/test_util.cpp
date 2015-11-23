#include <vector>
#include <exception>
#include <folly/futures/Future.h>
#include <folly/futures/helpers.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include <folly/ExceptionWrapper.h>

#include "gtest/gtest.h"
#include "util/util.h"


using namespace std;
using namespace folly;

using namespace relevanced;

class SomeException: public std::exception {
 public:
  SomeException(){}
  virtual const char* what() const throw() {
    return "nothing";
  }
};

TEST(TestUtil, TestOptionOfTryHappy) {
  Try<int> thing (5);
  auto res = util::optionOfTry(thing);
  EXPECT_TRUE(res.hasValue());
  EXPECT_EQ(5, res.value());
}

TEST(TestUtil, TestOptionOfTrySad) {
  Try<int> thing(folly::make_exception_wrapper<SomeException>());
  EXPECT_TRUE(thing.hasException());
  auto res = util::optionOfTry(thing);
  EXPECT_FALSE(res.hasValue());
}



TEST(TestUtil, TestOptionOfFutureTryHappy) {
  Try<int> thing (5);
  Future<Try<int>> futureThing = makeFuture(thing);
  auto res = util::optionOfTry(futureThing).get();
  EXPECT_TRUE(res.hasValue());
  EXPECT_EQ(5, res.value());
}

TEST(TestUtil, TestOptionOfFutureTrySad) {
  Try<int> thing (folly::make_exception_wrapper<SomeException>());
  Future<Try<int>> futureThing = makeFuture(thing);
  auto res = util::optionOfTry(futureThing).get();
  EXPECT_FALSE(res.hasValue());
}


