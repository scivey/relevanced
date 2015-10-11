#pragma once
#include <string>

namespace relevanced {
namespace release_metadata {

std::string getGitVersion();
std::string getGitRevisionSha();
std::string getUtcBuildTimestamp();
}
}
