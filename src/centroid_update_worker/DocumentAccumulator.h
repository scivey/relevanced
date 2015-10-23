#pragma once
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include "models/ProcessedDocument.h"
namespace relevanced {
namespace centroid_update_worker {

class DocumentAccumulatorIf {
public:
  virtual void addDocument(models::ProcessedDocument* document) = 0;
  virtual std::map<std::string, double>&& getScores() = 0;
  virtual double getMagnitude() = 0;
  virtual size_t getCount() = 0;
  virtual ~DocumentAccumulatorIf() = default;
};

class DocumentAccumulator: public DocumentAccumulatorIf {
  size_t documentCount_ {0};
  std::map<std::string, double> scores_;
  double magnitude_ {0};
  bool staleMagnitude_ {true};
public:
  void addDocument(models::ProcessedDocument* document) override;
  std::map<std::string, double>&& getScores() override;
  double getMagnitude() override;
  size_t getCount() override;
};

} // centroid_update_worker
} // relevanced
