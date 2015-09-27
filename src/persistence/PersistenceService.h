#pragma once

#include <memory>
#include "CentroidDB.h"
#include "DocumentDB.h"
#include "ClassifierDB.h"

namespace persistence {

class PersistenceServiceIf {
public:
  virtual std::weak_ptr<CentroidDBIf> getCentroidDb() = 0;
  virtual std::weak_ptr<DocumentDBIf> getDocumentDb() = 0;
  virtual std::weak_ptr<ClassifierDBIf> getClassifierDb() = 0;
};

class PersistenceService: public PersistenceServiceIf {
protected:
  std::shared_ptr<CentroidDBIf> centroidDb_;
  std::shared_ptr<DocumentDBIf> documentDb_;
  std::shared_ptr<ClassifierDBIf> classifierDb_;
public:
  PersistenceService(
    std::shared_ptr<CentroidDBIf>,
    std::shared_ptr<DocumentDBIf>,
    std::shared_ptr<ClassifierDBIf>
  );
  std::weak_ptr<CentroidDBIf> getCentroidDb() override;
  std::weak_ptr<DocumentDBIf> getDocumentDb() override;
  std::weak_ptr<ClassifierDBIf> getClassifierDb() override;
};

} // persistence

