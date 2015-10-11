#include <memory>
#include <string>
#include <vector>
#include <glog/logging.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/FutureExecutor.h>
#include <folly/futures/Future.h>
#include <folly/futures/Try.h>
#include <folly/Optional.h>
#include "models/ProcessedDocument.h"
#include "models/Centroid.h"
#include "persistence/SyncPersistence.h"

using namespace std;
using namespace folly;
using namespace relevanced;
using namespace relevanced::persistence;
using namespace relevanced::models;


class MockSyncPersistence : public persistence::SyncPersistenceIf {
 public:
  MOCK_METHOD1(doesDocumentExist, bool(const string&));
  MOCK_METHOD1(saveDocument, Try<bool>(shared_ptr<ProcessedDocument>));
  MOCK_METHOD1(deleteDocument, Try<bool>(const string&));
  MOCK_METHOD0(listAllDocuments, vector<string>(void));
  MOCK_METHOD2(listDocumentRangeFromId, vector<string>(const string&, size_t));
  MOCK_METHOD2(listDocumentRangeFromOffset, vector<string>(size_t, size_t));

  MOCK_METHOD1(loadDocument, Try<shared_ptr<ProcessedDocument>>(const string&));
  MOCK_METHOD1(loadDocumentOption,
               Optional<shared_ptr<ProcessedDocument>>(const string&));

  MOCK_METHOD1(doesCentroidExist, bool(const string&));
  MOCK_METHOD1(createNewCentroid, Try<bool>(const string&));
  MOCK_METHOD1(deleteCentroid, Try<bool>(const string&));
  MOCK_METHOD2(saveCentroid, Try<bool>(const string&, shared_ptr<Centroid>));
  MOCK_METHOD1(loadCentroid, Try<shared_ptr<Centroid>>(const string&));
  MOCK_METHOD1(loadCentroidOption,
               Optional<shared_ptr<Centroid>>(const string&));
  MOCK_METHOD0(listAllCentroids, vector<string>(void));
  MOCK_METHOD2(listCentroidRangeFromOffset, vector<string>(size_t, size_t));
  MOCK_METHOD2(listCentroidRangeFromId, vector<string>(const string&, size_t));

  MOCK_METHOD2(addDocumentToCentroid, Try<bool>(const string&, const string&));
  MOCK_METHOD2(removeDocumentFromCentroid,
               Try<bool>(const string&, const string&));
  MOCK_METHOD2(doesCentroidHaveDocument,
               Try<bool>(const string&, const string&));
  MOCK_METHOD1(listAllDocumentsForCentroid, Try<vector<string>>(const string&));
  MOCK_METHOD1(listAllDocumentsForCentroidOption,
               Optional<vector<string>>(const string&));
  MOCK_METHOD3(listCentroidDocumentRangeFromOffset,
               Try<vector<string>>(const string&, size_t, size_t));
  MOCK_METHOD3(listCentroidDocumentRangeFromOffsetOption,
               Optional<vector<string>>(const string&, size_t, size_t));

  MOCK_METHOD3(listCentroidDocumentRangeFromDocumentId,
               Try<vector<string>>(const string&, const string&, size_t));
  MOCK_METHOD3(listCentroidDocumentRangeFromDocumentIdOption,
               Optional<vector<string>>(const string&, const string&, size_t));
  MOCK_METHOD2(getCentroidMetadata,
               Optional<string>(const string&, const string&));
  MOCK_METHOD3(setCentroidMetadata,
               Try<bool>(const string&, const string&, string));
};
