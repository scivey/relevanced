# Allow the source files to find headers in src/
include_directories(${PROJECT_SOURCE_DIR}/src)

add_cxx_compiler_flag(-Wall)
add_cxx_compiler_flag(-Wextra)

if (HAVE_CXX_FLAG_FSTRICT_ALIASING)
  add_cxx_compiler_flag(-Wstrict-aliasing)
endif()

set(RELEVANCED_BASE_SOURCES
    "DocumentProcessingWorker.cpp"
    "DocumentProcessor.cpp"
    "ThriftRelevanceServer.cpp"
    "RelevanceServer.cpp"
    "CentroidUpdateWorker.cpp"
    "CentroidUpdaterFactory.cpp"
    "CentroidUpdater.cpp"
    "Centroid.cpp"
    "SimilarityScoreWorker.cpp"
    "tokenizer/Tokenizer.cpp"
    "stemmer/PorterStemmer.cpp"
    "stopwords/StopwordFilter.cpp"
    "stopwords/english_stopwords.cpp"
    "persistence/Persistence.cpp"
    "persistence/SyncPersistence.cpp"
    "persistence/InMemoryRockHandle.cpp"
    "persistence/RockHandle.cpp"
    "util.cpp"
    "gen-cpp2/Relevance.cpp"
    "gen-cpp2/Relevance_client.cpp"
    "gen-cpp2/Relevance_processmap_binary.cpp"
    "gen-cpp2/Relevance_processmap_compact.cpp"
    "gen-cpp2/TextRelevance_constants.cpp"
    "gen-cpp2/TextRelevance_types.cpp"
)

# Define the source files

add_executable(relevanced "main.cpp" ${RELEVANCED_BASE_SOURCES})

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
    relevanced
    ${RELEVANCED_LINK_LIBRARIES}
)
