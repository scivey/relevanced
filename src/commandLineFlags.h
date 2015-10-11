#pragma once
#include <gflags/gflags.h>

DEFINE_bool(
    integration_test_mode,
    false,
    "Enable dangerous commands used for testing (e.g. erase everything)");
DEFINE_int32(port, 0, "Port for relevanced's Thrift server to listen on");
DEFINE_string(data_dir,
              "",
              "Directory where the server should place its RocksDB database");
DEFINE_string(config_file, "", "Path to a JSON-formatted config file.");
DEFINE_int32(rocks_db_threads,
             0,
             "Number of threads to spawn in the RocksDB thread pool");
DEFINE_int32(centroid_update_threads,
             0,
             "Number of threads in the centroid updating pool");
DEFINE_int32(similarity_score_threads,
             0,
             "Number of threads in the similarity scoring pool");
DEFINE_int32(document_processing_threads,
             0,
             "Number of threads in the document processing pool");
