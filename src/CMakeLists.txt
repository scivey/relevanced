# Allow the source files to find headers in src/
include_directories(${PROJECT_SOURCE_DIR}/src)

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
    ${RELEVANCED_COMMON_DYNAMIC_LIBS}
)
set(RELEVANCED_STATIC_LINK_LIBS
    ${RELEVANCED_STATIC_STATIC_LIBS}
    ${RELEVANCED_COMMON_DYNAMIC_LIBS}
)

set(RELEVANCED_BASE_SOURCES
    "centroid_update_worker/CentroidUpdater.cpp"
    "centroid_update_worker/CentroidUpdaterFactory.cpp"
    "centroid_update_worker/CentroidUpdateWorker.cpp"
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
    "server/RelevanceServer.cpp"
    "server/ThriftRelevanceServer.cpp"
    "server/ThriftServerWrapper.cpp"
    "server/RelevanceServerOptions.cpp"
    "server/simpleServerBuilders.cpp"
    "similarity_score_worker/SimilarityScoreWorker.cpp"
    "stemmer/PorterStemmer.cpp"
    "stopwords/english_stopwords.cpp"
    "stopwords/StopwordFilter.cpp"
    "tokenizer/Tokenizer.cpp"
    "util/util.cpp"
    "release_metadata/release_metadata.cpp"
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

