# Configuration

All of **relevanced**'s settings can be managed in the following ways, in order of increasing precedence:

- Default values (built into the binary)
- Configuration file settings (generally `/etc/relevanced/relevanced.json`)
- Environment variables
- Command line flags

As an example:

- The default `port` setting is `8097`.
- If the `"port"` key is specified in `relevanced.json` as `"port": 8098`, then the server will listen on port 8098 *as long as* the port is not specified via environment variable or command line flag.
- If the `RELEVANCED_PORT` environment variable is defined as `8099`, then the server will listen on port 8099.  Both the default value and the value in `relevanced.json` will be ignored.
- If the `--port` option is passed on the command line as `--port 8100`, then the server will listen on port 8100.  The default value, any value of `"port"` in `relevanced.json`, and any value of `RELEVANCED_PORT` will all be ignored.

## Options

### `data_dir`
The directory in which **relevanced** should place its embedded RocksDB database.

- Command line flag: `--data_dir`
- Config file key: `"data_dir"`
- Environment variable: `RELEVANCED_DATA_DIR`

### `port`
The port which **relevanced**'s Thrift server should listen on.

- Command line flag: `--port`
- Config file key: `"port"`
- Environment variable: `RELEVANCED_PORT`

### `rocks_db_threads`
The number of threads to spawn in the RocksDB thread pool.

- Command line flag: `--rocks_db_threads`
- Config file key: `"rocks_db_threads"`
- Environment variable: `RELEVANCED_ROCKSDB_THREADS`

### `document_processing_threads`
The number of threads to spawn in the document processing pool.

- Command line flag: `--document_processing_threads`
- Config file key: `"document_processing_threads"`
- Environment variable: `RELEVANCED_DOCUMENT_PROCESSING_THREADS`

### `similarity_scoring_threads`
The number of threads to spawn in the similarity scoring pool.

- Command line flag: `--similarity_scoring_threads`
- Config file key: `"similarity_scoring_threads"`
- Environment variable: `RELEVANCED_SIMILARITY_SCORING_THREADS`

### `centroid_update_threads`
The number of threads to spawn in the centroid updating pool.

- Command line flag: `--centroid_update_threads`
- Config file key: `"centroid_update_threads"`
- Environment variable: `RELEVANCED_CENTROID_UPDATE_THREADS`

