#include <memory>
#include "PersistenceService.h"
#include "CentroidDB.h"
#include "DocumentDB.h"
#include "CollectionDB.h"

namespace persistence {

PersistenceService::PersistenceService(
  std::shared_ptr<CentroidDBIf> centroidDb,
  std::shared_ptr<DocumentDBIf> documentDb,
  std::shared_ptr<CollectionDBIf> collectionDb
): centroidDb_(centroidDb), documentDb_(documentDb), collectionDb_(collectionDb) {}

std::weak_ptr<CentroidDBIf> PersistenceService::getCentroidDb() {
  weak_ptr<CentroidDBIf> res = centroidDb_;
  return std::move(res);
}
std::weak_ptr<DocumentDBIf> PersistenceService::getDocumentDb() {
  weak_ptr<DocumentDBIf> res = documentDb_;
  return std::move(res);
}
std::weak_ptr<CollectionDBIf> PersistenceService::getCollectionDb() {
  weak_ptr<CollectionDBIf> res = collectionDb_;
  return std::move(res);
}

} // persistence

