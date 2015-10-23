# Allow the source files to find headers in src/
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/external/libstemmer/include)

add_cxx_compiler_flag(-Wall)
add_cxx_compiler_flag(-Wextra)

if (HAVE_CXX_FLAG_FSTRICT_ALIASING)
  add_cxx_compiler_flag(-Wstrict-aliasing)
endif()

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

set(RELEVANCED_COMMON_DYNAMIC_LIBS
    "glog"
    "gflags"
    "z"
    "snappy"
    "lz4"
    "bz2"
    "gssapi_krb5"
    "krb5"
    "crypto"
    "gnutls"
    "ssl"
    "iberty"
    "double-conversion"
    "boost_thread"
    "boost_system"
    "boost_context"
    "jemalloc"
    "atomic"
    "pthread"
    "event"
    "dl"
)

set(LIBSTEMMER ${PROJECT_SOURCE_DIR}/external/libstemmer/libstemmer.o)

set(RELEVANCED_NONSTATIC_ADDITIONAL_LIBS
    "thriftcpp2"
    "thrift"
    "thriftz"
    "thriftcpp2"
    "wangle"
    "folly"
    "mitie"
    "rocksdb"
)

find_library(THRIFT_CPP2 libthriftcpp2.a)
find_library(THRIFT libthrift.a)
find_library(THRIFTZ libthriftz.a)
find_library(SASL_STUBS libsaslstubs.a)
find_library(WANGLE libwangle.a)
find_library(FOLLY libfolly.a)
find_library(MITIE libmitie.a)
find_library(ROCKSDB librocksdb.a)

set(RELEVANCED_STATIC_STATIC_LIBS
    ${THRIFT_CPP2}
    ${THRIFT}
    ${THRIFTZ}
    ${THRIFT_CPP2}
    ${SASL_STUBS}
    ${WANGLE}
    ${FOLLY}
    ${MITIE}
    ${ROCKSDB}
)

set(RELEVANCED_NONSTATIC_LINK_LIBS
    ${RELEVANCED_NONSTATIC_ADDITIONAL_LIBS}
    ${LIBSTEMMER}
    ${RELEVANCED_COMMON_DYNAMIC_LIBS}
)
set(RELEVANCED_STATIC_LINK_LIBS
    ${RELEVANCED_STATIC_STATIC_LIBS}
    ${LIBSTEMMER}
    ${RELEVANCED_COMMON_DYNAMIC_LIBS}
)

set(RELEVANCED_BASE_SOURCES
    "centroid_update_worker/CentroidUpdater.cpp"
    "centroid_update_worker/CentroidUpdaterFactory.cpp"
    "centroid_update_worker/CentroidUpdateWorker.cpp"
    "centroid_update_worker/DocumentAccumulator.cpp"
    "centroid_update_worker/DocumentAccumulatorFactory.cpp"
    "document_processing_worker/DocumentProcessingWorker.cpp"
    "document_processing_worker/DocumentProcessor.cpp"
    "gen-cpp2/Relevanced.cpp"
    "gen-cpp2/Relevanced_client.cpp"
    "gen-cpp2/Relevanced_processmap_binary.cpp"
    "gen-cpp2/Relevanced_processmap_compact.cpp"
    "gen-cpp2/RelevancedProtocol_constants.cpp"
    "gen-cpp2/RelevancedProtocol_types.cpp"
    "models/WordVector.cpp"
    "persistence/InMemoryRockHandle.cpp"
    "persistence/Persistence.cpp"
    "persistence/RockHandle.cpp"
    "persistence/SyncPersistence.cpp"
    "persistence/CentroidMetadataDb.cpp"
    "serialization/serializers.cpp"
    "server/RelevanceServer.cpp"
    "server/ThriftRelevanceServer.cpp"
    "server/ThriftServerWrapper.cpp"
    "server/RelevanceServerOptions.cpp"
    "server/simpleServerBuilders.cpp"
    "similarity_score_worker/SimilarityScoreWorker.cpp"
    "stopwords/english_stopwords.cpp"
    "stopwords/StopwordFilter.cpp"
    "tokenizer/Tokenizer.cpp"
    "tokenizer/DestructiveConllTokenIterator.cpp"
    "util/util.cpp"
    "release_metadata/release_metadata.cpp"
    "stemmer/Utf8Stemmer.cpp"
    "text_util/ScoredWord.cpp"
    "text_util/StringView.cpp"
    "text_util/WordAccumulator.cpp"

)

add_library(relevanced_core ${RELEVANCED_BASE_SOURCES})
target_link_libraries(relevanced_core ${RELEVANCED_NONSTATIC_LINK_LIBS})

add_executable(relevanced "relevanced_server.cpp")
add_dependencies(relevanced relevanced_core)
target_link_libraries(relevanced
    relevanced_core
    ${RELEVANCED_NONSTATIC_LINK_LIBS}
)

add_library(relevanced_core_static ${RELEVANCED_BASE_SOURCES})
target_link_libraries(relevanced_core_static ${RELEVANCED_STATIC_LINK_LIBS})

add_executable(relevanced_static "relevanced_server.cpp")
add_dependencies(relevanced_static relevanced_core_static)
target_link_libraries(relevanced_static
    relevanced_core_static
    ${RELEVANCED_STATIC_LINK_LIBS}
)

set(UNIT_TEST_SOURCES
  "stopwords/test_unit/test_StopwordFilter.cpp"
  "centroid_update_worker/test_unit/test_CentroidUpdater.cpp"
  "centroid_update_worker/test_unit/test_CentroidUpdateWorker.cpp"
  "centroid_update_worker/test_unit/test_DocumentAccumulator.cpp"
  "document_processing_worker/test_unit/test_DocumentProcessor.cpp"
  "document_processing_worker/test_unit/test_DocumentProcessingWorker.cpp"
  "similarity_score_worker/test_unit/test_SimilarityScoreWorker.cpp"
  "models/test_unit/test_WordVector.cpp"
  "persistence/test_unit/test_CentroidMetadataDb.cpp"
  "persistence/test_unit/test_InMemoryRockHandle.cpp"
  "persistence/test_unit/test_SyncPersistence.cpp"
  "persistence/test_unit/test_Persistence.cpp"
  "tokenizer/test_unit/test_Tokenizer.cpp"
  "stemmer/test_unit/test_Utf8Stemmer.cpp"
  "serialization/test_unit/test_DocumentSerialization.cpp"
  "util/test_unit/test_ConcurrentMap.cpp"
  "text_util/test_unit/test_WordAccumulator.cpp"
  "text_util/test_unit/test_StringView.cpp"
  "text_util/test_unit/test_ScoredWord.cpp"
  "testing/runTests.cpp"
)

add_executable(unit_test_runner ${UNIT_TEST_SOURCES})

set(GTEST_LIBS
  "${PROJECT_SOURCE_DIR}/external/gmock-1.7.0/src/gmock-all.o"
  "${PROJECT_SOURCE_DIR}/external/gtest-1.7.0-min/gtest-all.o"
)

add_dependencies(unit_test_runner relevanced_core)
target_link_libraries(unit_test_runner
    relevanced_core
    ${GTEST_LIBS}
    ${RELEVANCED_NONSTATIC_LINK_LIBS}
)

set(SLOW_ROCK_TEST_SOURCES "persistence/test_functional/slow_RockHandle_test.cpp")
add_executable(slow_rock_handle_test ${SLOW_ROCK_TEST_SOURCES})
add_dependencies(slow_rock_handle_test relevanced_core)
target_link_libraries(slow_rock_handle_test
    relevanced_core
    ${GTEST_LIBS}
    ${RELEVANCED_NONSTATIC_LINK_LIBS}
)


set(FUNC_TEST_SOURCES
    "document_processing_worker/test_functional/test_DocumentProcessingWorker.cpp"
    "centroid_update_worker/test_functional/test_CentroidUpdateWorker.cpp"
    "similarity_score_worker/test_functional/test_SimilarityScoreWorker.cpp"
    "server/test_functional/test_RelevanceServer.cpp"
    "testing/runTests.cpp"
)
add_executable(func_test_runner ${FUNC_TEST_SOURCES})
add_dependencies(func_test_runner relevanced_core)
target_link_libraries(func_test_runner
    relevanced_core
    ${GTEST_LIBS}
    ${RELEVANCED_NONSTATIC_LINK_LIBS}
)
