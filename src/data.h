#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <folly/json.h>
#include <folly/Conv.h>
#include <folly/dynamic.h>
#include <folly/DynamicConverter.h>

#include "Article.h"

namespace {
  using namespace std;
  using namespace folly;
}

string readFile(const char *fPath) {
  ifstream ifs {fPath};
  ostringstream oss;
  char c;
  while (ifs.get(c)) {
    oss << c;
  }
  return oss.str();
}

vector<Article> loadData() {
  auto data = readFile("test_data/text.json");
  auto asJson = folly::parseJson(data);
  vector<Article> output;
  for(auto &elem: asJson["math"]) {
    auto dtitle = elem["title"];
    auto durl = elem["url"];
    auto dtext = elem["text"];
    Article art(
      folly::convertTo<string>(durl),
      folly::convertTo<string>(dtitle),
      folly::convertTo<string>(durl),
      Article::Subject::MATH,
      folly::convertTo<string>(dtext)
    );
    output.push_back(art);
  }
  for(auto &elem: asJson["politics"]) {
    auto dtitle = elem["title"];
    auto durl = elem["url"];
    auto dtext = elem["text"];
    Article art(
      folly::convertTo<string>(durl),
      folly::convertTo<string>(dtitle),
      folly::convertTo<string>(durl),
      Article::Subject::POLITICS,
      folly::convertTo<string>(dtext)
    );
    output.push_back(art);
  }

  return output;
}