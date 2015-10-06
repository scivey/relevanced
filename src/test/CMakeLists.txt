# Allow the source files to find headers in src/

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/external/gtest-1.7.0-min/include)
include_directories(${PROJECT_SOURCE_DIR}/external/gmock-1.7.0/include)
include_directories(${PROJECT_SOURCE_DIR}/src/test/unit)

set(RELEVANCED_BASE_SOURCES
    "${PROJECT_SOURCE_DIR}/src/document_processing_worker/DocumentProcessingWorker.cpp"
    "${PROJECT_SOURCE_DIR}/src/document_processing_worker/DocumentProcessor.cpp"
    "${PROJECT_SOURCE_DIR}/src/server/ThriftRelevanceServer.cpp"
    "${PROJECT_SOURCE_DIR}/src/server/RelevanceServer.cpp"
    "${PROJECT_SOURCE_DIR}/src/centroid_update_worker/CentroidUpdateWorker.cpp"
    "${PROJECT_SOURCE_DIR}/src/centroid_update_worker/CentroidUpdaterFactory.cpp"
    "${PROJECT_SOURCE_DIR}/src/centroid_update_worker/CentroidUpdater.cpp"
    "${PROJECT_SOURCE_DIR}/src/models/WordVector.cpp"
    "${PROJECT_SOURCE_DIR}/src/similarity_score_worker/SimilarityScoreWorker.cpp"
    "${PROJECT_SOURCE_DIR}/src/tokenizer/Tokenizer.cpp"
    "${PROJECT_SOURCE_DIR}/src/stemmer/PorterStemmer.cpp"
    "${PROJECT_SOURCE_DIR}/src/stopwords/StopwordFilter.cpp"
    "${PROJECT_SOURCE_DIR}/src/stopwords/english_stopwords.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/Persistence.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/SyncPersistence.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/InMemoryRockHandle.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/RockHandle.cpp"
    "${PROJECT_SOURCE_DIR}/src/util/util.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevanced.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevanced_client.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevanced_processmap_binary.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevanced_processmap_compact.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/RelevancedProtocol_constants.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/RelevancedProtocol_types.cpp"
)

set(RELEVANCED_LINK_LIBRARIES
    "thriftcpp2"
    "thrift"
    "wangle"
    "folly"
    "rocksdb"
    "mitie"
    "glog"
    "z"
    "snappy"
    "lz4"
    "bz2"
    "double-conversion"
    "boost_thread"
    "boost_system"
    "jemalloc"
    "atomic"
    "pthread"
)


set(UNIT_TEST_SOURCES
  "unit/test_CentroidUpdateWorker.cpp"
  "unit/test_DocumentProcessingWorker.cpp"
  "unit/test_DocumentProcessor.cpp"
  "unit/test_DocumentSerialization.cpp"
  "unit/test_InMemoryRockHandle.cpp"
  "unit/test_PorterStemmer.cpp"
  "unit/test_SimilarityScoreWorker.cpp"
  "unit/test_StopwordFilter.cpp"
  "unit/test_SyncPersistence.cpp"
  "unit/test_Persistence.cpp"
  "unit/test_Tokenizer.cpp"
  "unit/runTests.cpp"
)

set(GTEST_LIBS
  "${PROJECT_SOURCE_DIR}/external/gmock-1.7.0/src/gmock-all.o"
  "${PROJECT_SOURCE_DIR}/external/gtest-1.7.0-min/gtest-all.o"
)

add_executable(
  unit_test_runner ${UNIT_TEST_SOURCES} ${RELEVANCED_BASE_SOURCES}
)

target_link_libraries(
  unit_test_runner
  ${GTEST_LIBS}
  ${RELEVANCED_LINK_LIBRARIES}
)

set(ROCK_HANDLE_TEST_SOURCES
  "rock_handle/test_RockHandle_basic.cpp"
  "rock_handle/runTests.cpp"
)

add_executable(
  rock_handle_test ${ROCK_HANDLE_TEST_SOURCES} ${RELEVANCED_BASE_SOURCES}
)

target_link_libraries(
  rock_handle_test
  ${GTEST_LIBS}
  ${RELEVANCED_LINK_LIBRARIES}
)
