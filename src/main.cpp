#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

#include <glog/logging.h>
#include <folly/Format.h>
#include <folly/Format.h>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <eigen3/Eigen/Dense>
#include "data.h"
#include "WhitespaceTokenizer.h"
#include "stopwords.h"
#include "stringUtil.h"
#include "util.h"
#include "Centroid.h"
#include "Tfidf.h"
#include "CentroidFactory.h"

using tokenizer::WhitespaceTokenizer;
using namespace std;
using namespace folly;
using namespace Eigen;

void processElem(Article &article) {
  auto nonStops = article.getNonStopwords();
  util::Counter<string> counter;
  for (auto &elem: nonStops) {
    counter.incr(elem);
  }
  vector<pair<string, size_t>> counts;
  for (auto &elem: counter.counts) {
    counts.push_back(elem);
  }
  sort(counts.begin(), counts.end(), [](const pair<string, size_t> &p1, const pair<string, size_t> &p2) {
    return p1.second > p2.second;
  });
  size_t i = 0;
  for (auto &elem: counts) {
    if (i++ > 10) {
      break;
    }
    LOG(INFO) << elem.first << "\t" << elem.second;
  }
  LOG(INFO) << counts.size();
  LOG(INFO) << counts.at(0).second;
  LOG(INFO) << counts.at(counts.size() - 1).second;
}

int main() {
  LOG(INFO) << "start!!";
  auto data = loadData();
  for (auto &elem: data) {
    LOG(INFO) << elem.title_;
  }
  vector<Article*> mathArts;
  vector<Article*> poliArts;
  vector<Article*> allArts;
  for (auto &elem: data) {
    if (elem.subject_ == Article::Subject::POLITICS) {
      poliArts.push_back(&elem);
    } else if (elem.subject_ == Article::Subject::MATH) {
      mathArts.push_back(&elem);
    }
    allArts.push_back(&elem);
  }
  Tfidf mathTfidf(mathArts);
  Centroid mathCentroid(mathArts, &mathTfidf);
  Tfidf poliTfidf(poliArts);
  Centroid poliCentroid(poliArts, &poliTfidf);
  // CentroidFactory cfact(allArts);
  // Centroid mathCentroid = cfact.makeCentroid(mathArts);
  // Centroid poliCentroid = cfact.makeCentroid(poliArts);
  LOG(INFO) << "math v math";
  for (auto elem: mathArts) {
    mathCentroid.evalRelevance(elem);
  }
  LOG(INFO) << "math v poli";
  for (auto elem: poliArts) {
    mathCentroid.evalRelevance(elem);
  }
  LOG(INFO) << "poli v math";
  for (auto elem: mathArts) {
    poliCentroid.evalRelevance(elem);
  }
  LOG(INFO) << "poli v poli";
  for (auto elem: poliArts) {
    poliCentroid.evalRelevance(elem);
  }

  LOG(INFO) << "poli accuracy: " << poliCentroid.test(poliArts, mathArts);
  LOG(INFO) << "math accuracy: " << mathCentroid.test(mathArts, poliArts);

  // cout << endl << mathCentroid.getSV() << endl;
  // auto elem = data.at(0);
  // processElem(elem);
  // MatrixXd m(2, 2);
  // m(0, 0) = 3;
  // m(1, 0) = 2.5;
  // m(0, 1) = -1;
  // m(1, 1) = m(1, 0) + m(0, 1);
  // LOG(INFO) << "mat: ";
  // cout << m << endl;
  // VectorXd v(2);
  // v(0) = 4;
  // v(1) = v(0) - 1;
  // LOG(INFO) << "vec: ";
  // cout << v << endl;

  // VectorXd v2(2);
  // v2(0) = 1;
  // v2(1) = 0.1;
  // LOG(INFO) << "v2: ";
  // cout << v2 << endl;

  // auto res = v * v2.transpose();
  // LOG(INFO) << "res: ";

  // cout << res << endl;
  LOG(INFO) << "end!!";
}
