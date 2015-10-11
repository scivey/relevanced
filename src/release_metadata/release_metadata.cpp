#include <string>

#include "release_metadata/release_metadata.h"
#include "config.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifndef RELEVANCED_BUILD_GIT_VERSION
#define RELEVANCED_BUILD_GIT_VERSION UNKNOWN_VERSION
#endif

#ifndef RELEVANCED_BUILD_GIT_REVISION_SHA
#define RELEVANCED_BUILD_GIT_REVISION_SHA UNKNOWN_REVISION
#endif

#ifndef RELEVANCED_BUILD_TIMESTAMP_UTC
#define RELEVANCED_BUILD_TIMESTAMP_UTC 0001-01-01T00:00:00
#endif

using namespace std;

namespace relevanced {
namespace release_metadata {

string getGitVersion() {
  string version = TOSTRING(RELEVANCED_BUILD_GIT_VERSION);
  return version;
}

string getGitRevisionSha() {
  string revision = TOSTRING(RELEVANCED_BUILD_GIT_REVISION_SHA);
  return revision;
}

string getUtcBuildTimestamp() {
  string timestamp = TOSTRING(RELEVANCED_BUILD_TIMESTAMP_UTC);
  return timestamp;
}
}
}
