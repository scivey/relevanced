#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <glog/logging.h>

#include "util.h"
#include "Document.h"
#include "DocumentProcessor.h"
#include "ProcessedDocument.h"
#include "ProcessedCentroid.h"
#include "CentroidFactory.h"
#include "persistence/PersistenceService.h"
#include "CentroidManager.h"


class RelevanceCollectionManager {
protected:
  std::shared_ptr<persistence::PersistenceServiceIf> persistence_;
  std::shared_ptr<CentroidManager> centroidManager_;
  std::shared_ptr<DocumentProcessor> documentProcessor_;
  std::map<std::string, ProcessedCentroid*> centroids_;
public:
  RelevanceCollectionManager(
    std::shared_ptr<persistence::PersistenceService>,
    std::shared_ptr<CentroidManager>,
    std::shared_ptr<DocumentProcessor>
  );
  void initialize();
  double getRelevanceForDoc(const std::string &collectionId, const std::string &docId);
  double getRelevanceForText(const std::string &collectionId, const std::string &text);
  bool createCollection(std::string id);
  bool deleteCollection(std::string id);
  bool deleteDocument(std::string id);
  std::string createDocument(std::string text);
  bool createDocumentWithId(std::string id, std::string text);
  std::string addNewNegativeDocumentToCollection(std::string collectionId, std::string text);
  std::string addNewPositiveDocumentToCollection(std::string collectionId, std::string text);
  bool reloadCentroid(std::string id);
};

