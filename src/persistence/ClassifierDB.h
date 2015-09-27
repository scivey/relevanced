#pragma once

#include <string>
#include <memory>
#include <vector>

#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include "ClassifierDBHandle.h"

#include "util.h"

namespace persistence {

class ClassifierDBIf {
public:

  virtual void initialize() = 0;

  virtual folly::Future<bool>
    doesClassifierExist(const std::string&) = 0;
  virtual folly::Future<bool>
    createClassifier(const std::string&) = 0;

  virtual folly::Future<bool>
    doesClassifierHaveDocument(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    addPositiveDocumentToClassifier(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    addNegativeDocumentToClassifier(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    removeDocumentFromClassifier(const std::string&, const std::string&) = 0;

  virtual folly::Future<bool>
    deleteClassifier(const std::string&) = 0;

  virtual folly::Future<std::vector<std::string>>
    listClassifiers() = 0;

  virtual folly::Future<int>
    getClassifierDocumentCount(const std::string &) = 0;

  virtual folly::Future<std::vector<std::string>>
    listAllClassifierDocuments(const string &classifierId) = 0;

  virtual folly::Future<std::vector<std::string>>
    listAllPositiveClassifierDocuments(const string &classifierId) = 0;

  virtual folly::Future<std::vector<std::string>>
    listAllNegativeClassifierDocuments(const string &classifierId) = 0;

  virtual folly::Future<std::vector<std::string>>
    listKnownDocuments() = 0;

  virtual ~ClassifierDBIf() = default;
};

class ClassifierDB: public ClassifierDBIf {
protected:
  util::UniquePointer<ClassifierDBHandleIf> dbHandle_;
  std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool_;
  ClassifierDB(ClassifierDB const&) = delete;
  void operator=(ClassifierDB const&) = delete;
public:
  ClassifierDB(
    util::UniquePointer<ClassifierDBHandleIf> dbHandle,
    std::shared_ptr<wangle::FutureExecutor<wangle::CPUThreadPoolExecutor>> threadPool
  );
  void initialize() override;
  folly::Future<bool> doesClassifierExist(const std::string &classifierId);
  folly::Future<bool> createClassifier(const std::string &classifierId);
  folly::Future<bool> doesClassifierHaveDocument(const std::string &classifierId, const std::string &docId);
  folly::Future<bool> addPositiveDocumentToClassifier(const std::string &classifierId, const std::string &docId);
  folly::Future<bool> addNegativeDocumentToClassifier(const std::string &classifierId, const std::string &docId);
  folly::Future<bool> removeDocumentFromClassifier(const std::string &classifierId, const std::string &docId);
  folly::Future<bool> deleteClassifier(const std::string &classifierId);
  folly::Future<std::vector<std::string>> listClassifiers();
  folly::Future<int> getClassifierDocumentCount(const std::string &classifierId);
  folly::Future<std::vector<std::string>> listAllClassifierDocuments(const std::string &classifierId);
  folly::Future<std::vector<std::string>> listAllPositiveClassifierDocuments(const std::string &classifierId);
  folly::Future<std::vector<std::string>> listAllNegativeClassifierDocuments(const std::string &classifierId);
  folly::Future<std::vector<std::string>> listKnownDocuments();
};

} // persistence
