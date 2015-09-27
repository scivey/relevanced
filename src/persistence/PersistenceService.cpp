#include <memory>
#include "PersistenceService.h"
#include "CentroidDB.h"
#include "DocumentDB.h"
#include "ClassifierDB.h"

namespace persistence {

PersistenceService::PersistenceService(
  std::shared_ptr<CentroidDBIf> centroidDb,
  std::shared_ptr<DocumentDBIf> documentDb,
  std::shared_ptr<ClassifierDBIf> classifierDb
): centroidDb_(centroidDb), documentDb_(documentDb), classifierDb_(classifierDb) {}

std::weak_ptr<CentroidDBIf> PersistenceService::getCentroidDb() {
  weak_ptr<CentroidDBIf> res = centroidDb_;
  return std::move(res);
}
std::weak_ptr<DocumentDBIf> PersistenceService::getDocumentDb() {
  weak_ptr<DocumentDBIf> res = documentDb_;
  return std::move(res);
}
std::weak_ptr<ClassifierDBIf> PersistenceService::getClassifierDb() {
  weak_ptr<ClassifierDBIf> res = classifierDb_;
  return std::move(res);
}

} // persistence

