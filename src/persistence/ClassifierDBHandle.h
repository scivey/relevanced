#pragma once
#include <memory>
#include <cassert>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <folly/Format.h>
#include <glog/logging.h>
#include "PrefixedRockHandle.h"
#include "RockHandle.h"

#include "util.h"

namespace persistence {

class ClassifierDBHandleIf {
public:
  virtual bool doesClassifierExist(const std::string&) = 0;
  virtual bool createClassifier(const std::string&) = 0;
  virtual bool doesClassifierHaveDocument(const std::string&, const std::string&) = 0;
  virtual bool addPositiveDocumentToClassifier(const std::string &classifierId, const std::string &docId) = 0;
  virtual bool addNegativeDocumentToClassifier(const std::string &classifierId, const std::string &docId) = 0;
  virtual bool removeDocumentFromClassifier(const std::string &classifierId, const std::string &docId) = 0;
  virtual bool deleteClassifier(const std::string &classifierId) = 0;
  virtual std::vector<std::string> listClassifiers() = 0;
  virtual int getClassifierDocumentCount(const std::string &classifierId) = 0;
  virtual std::vector<std::string> listAllClassifierDocuments(const std::string &classifierId) = 0;
  virtual std::vector<std::string> listAllPositiveClassifierDocuments(const std::string &classifierId) = 0;
  virtual std::vector<std::string> listAllNegativeClassifierDocuments(const std::string &classifierId) = 0;
  virtual std::vector<std::string> listKnownDocuments() = 0;
  virtual ~ClassifierDBHandleIf() = default;
};

class ClassifierDBHandle: public ClassifierDBHandleIf {
protected:
  util::UniquePointer<RockHandleIf> classifierDocsHandle_;
  util::UniquePointer<RockHandleIf> classifierListHandle_;
  bool addDocumentToClassifier(const std::string&, const std::string&, bool isPositive);
public:
  ClassifierDBHandle(util::UniquePointer<RockHandleIf>, util::UniquePointer<RockHandleIf>);
  bool doesClassifierExist(const std::string&) override;
  bool createClassifier(const std::string&) override;
  bool doesClassifierHaveDocument(const std::string&, const std::string&) override;
  bool addPositiveDocumentToClassifier(const std::string &classifierId, const std::string &docId) override;
  bool addNegativeDocumentToClassifier(const std::string &classifierId, const std::string &docId) override;
  bool removeDocumentFromClassifier(const std::string &classifierId, const std::string &docId) override;
  bool deleteClassifier(const std::string &classifierId) override;
  std::vector<std::string> listClassifiers() override;
  int getClassifierDocumentCount(const std::string &classifierId) override;
  std::vector<std::string> listAllClassifierDocuments(const std::string &classifierId) override;
  std::vector<std::string> listAllPositiveClassifierDocuments(const std::string &classifierId) override;
  std::vector<std::string> listAllNegativeClassifierDocuments(const std::string &classifierId) override;
  std::vector<std::string> listKnownDocuments() override;
};

} // persistence

