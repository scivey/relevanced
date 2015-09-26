#include <string>
#include <memory>
#include <vector>
#include <folly/futures/Future.h>
#include <glog/logging.h>
#include "gen-cpp2/Relevance.h"
#include "ThriftRelevanceServer.h"

using namespace std;
using namespace folly;

ThriftRelevanceServer::ThriftRelevanceServer(shared_ptr<RelevanceServer> server)
  :server_(server) {}

void ThriftRelevanceServer::ping(){
  server_->ping();
}

Future<double> ThriftRelevanceServer::future_getRelevanceForDoc(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->getRelevanceForDoc(std::move(collId), std::move(docId));
}

Future<double> ThriftRelevanceServer::future_getRelevanceForText(unique_ptr<string> collId, unique_ptr<string> text) {
  return server_->getRelevanceForText(std::move(collId), std::move(text));
}

Future<unique_ptr<string>> ThriftRelevanceServer::future_createDocument(unique_ptr<string> text) {
  return server_->createDocument(std::move(text));
}

Future<unique_ptr<string>> ThriftRelevanceServer::future_createDocumentWithID(unique_ptr<string> id, unique_ptr<string> text) {
  return server_->createDocumentWithID(std::move(id), std::move(text));
}

Future<bool> ThriftRelevanceServer::future_deleteDocument(unique_ptr<string> id) {
  return server_->deleteDocument(std::move(id));
}

Future<unique_ptr<string>> ThriftRelevanceServer::future_getDocument(unique_ptr<string> id) {
  return server_->getDocument(std::move(id));
}

Future<bool> ThriftRelevanceServer::future_createCollection(unique_ptr<string> collId) {
  return server_->createCollection(std::move(collId));
}

Future<bool> ThriftRelevanceServer::future_deleteCollection(unique_ptr<string> collId) {
  return server_->deleteCollection(std::move(collId));
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listCollectionDocuments(unique_ptr<string> collId) {
  return server_->listCollectionDocuments(std::move(collId));
}

Future<bool> ThriftRelevanceServer::future_addPositiveDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->addPositiveDocumentToCollection(std::move(collId), std::move(docId));
}

Future<bool> ThriftRelevanceServer::future_addNegativeDocumentToCollection(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->addNegativeDocumentToCollection(std::move(collId), std::move(docId));
}

Future<bool> ThriftRelevanceServer::future_removeDocumentFromCollection(unique_ptr<string> collId, unique_ptr<string> docId) {
  return server_->removeDocumentFromCollection(std::move(collId), std::move(docId));
}

Future<bool> ThriftRelevanceServer::future_recompute(unique_ptr<string> collId) {
  return server_->recompute(std::move(collId));
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listCollections() {
  return server_->listCollections();
}

Future<unique_ptr<vector<string>>> ThriftRelevanceServer::future_listDocuments() {
  return server_->listDocuments();
}

Future<int> ThriftRelevanceServer::future_getCollectionSize(unique_ptr<string> collId) {
  return server_->getCollectionSize(std::move(collId));
}
