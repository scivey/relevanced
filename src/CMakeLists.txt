# Allow the source files to find headers in src/
include_directories(${PROJECT_SOURCE_DIR}/src)

add_cxx_compiler_flag(-Wall)
add_cxx_compiler_flag(-Wextra)

if (HAVE_CXX_FLAG_FSTRICT_ALIASING)
  add_cxx_compiler_flag(-Wstrict-aliasing)
endif()

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
    "similarity_score_worker/SimilarityScoreWorker.cpp"
    "stemmer/PorterStemmer.cpp"
    "stopwords/english_stopwords.cpp"
    "stopwords/StopwordFilter.cpp"
    "tokenizer/Tokenizer.cpp"
    "util/util.cpp"
)

# Define the source files

add_executable(relevanced "relevanced_server.cpp" ${RELEVANCED_BASE_SOURCES})

set(RELEVANCED_LINK_LIBRARIES
    "thriftcpp2"
    "thrift"
    "wangle"
    "folly"
    "rocksdb"
    "mitie"
    "glog"
    "gflags"
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
    relevanced
    ${RELEVANCED_LINK_LIBRARIES}
)
