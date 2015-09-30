# Allow the source files to find headers in src/

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/external/gtest-1.7.0-min/include)
include_directories(${PROJECT_SOURCE_DIR}/external/gmock-1.7.0/include)
include_directories(${PROJECT_SOURCE_DIR}/src/test/unit)

set(RELEVANCED_BASE_SOURCES
    "${PROJECT_SOURCE_DIR}/src/DocumentProcessingWorker.cpp"
    "${PROJECT_SOURCE_DIR}/src/DocumentProcessor.cpp"
    "${PROJECT_SOURCE_DIR}/src/ThriftRelevanceServer.cpp"
    "${PROJECT_SOURCE_DIR}/src/RelevanceServer.cpp"
    "${PROJECT_SOURCE_DIR}/src/CentroidUpdateWorker.cpp"
    "${PROJECT_SOURCE_DIR}/src/CentroidUpdaterFactory.cpp"
    "${PROJECT_SOURCE_DIR}/src/CentroidUpdater.cpp"
    "${PROJECT_SOURCE_DIR}/src/Centroid.cpp"
    "${PROJECT_SOURCE_DIR}/src/SimilarityScoreWorker.cpp"
    "${PROJECT_SOURCE_DIR}/src/tokenizer/Tokenizer.cpp"
    "${PROJECT_SOURCE_DIR}/src/stemmer/PorterStemmer.cpp"
    "${PROJECT_SOURCE_DIR}/src/stopwords/StopwordFilter.cpp"
    "${PROJECT_SOURCE_DIR}/src/stopwords/english_stopwords.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/Persistence.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/SyncPersistence.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/InMemoryRockHandle.cpp"
    "${PROJECT_SOURCE_DIR}/src/persistence/RockHandle.cpp"
    "${PROJECT_SOURCE_DIR}/src/util.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevance.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevance_client.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevance_processmap_binary.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/Relevance_processmap_compact.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/TextRelevance_constants.cpp"
    "${PROJECT_SOURCE_DIR}/src/gen-cpp2/TextRelevance_types.cpp"
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
  "unit/test_Tokenizer.cpp"
  "unit/runTests.cpp"
)

add_executable(
  unit_test_runner ${UNIT_TEST_SOURCES} ${RELEVANCED_BASE_SOURCES}
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

target_link_libraries(
  unit_test_runner
  "${PROJECT_SOURCE_DIR}/external/gmock-1.7.0/src/gmock-all.o"
  "${PROJECT_SOURCE_DIR}/external/gtest-1.7.0-min/gtest-all.o"
  ${RELEVANCED_LINK_LIBRARIES}
)
