#pragma once

#include <memory>
#include "CentroidDB.h"
#include "DocumentDB.h"
#include "CollectionDB.h"

namespace persistence {

class PersistenceServiceIf {
public:
  virtual std::weak_ptr<CentroidDBIf> getCentroidDb() = 0;
  virtual std::weak_ptr<DocumentDBIf> getDocumentDb() = 0;
  virtual std::weak_ptr<CollectionDBIf> getCollectionDb() = 0;
};

class PersistenceService: public PersistenceServiceIf {
protected:
  std::shared_ptr<CentroidDBIf> centroidDb_;
  std::shared_ptr<DocumentDBIf> documentDb_;
  std::shared_ptr<CollectionDBIf> collectionDb_;
public:
  PersistenceService(
    std::shared_ptr<CentroidDBIf>,
    std::shared_ptr<DocumentDBIf>,
    std::shared_ptr<CollectionDBIf>
  );
  std::weak_ptr<CentroidDBIf> getCentroidDb() override;
  std::weak_ptr<DocumentDBIf> getDocumentDb() override;
  std::weak_ptr<CollectionDBIf> getCollectionDb() override;
};

} // persistence

