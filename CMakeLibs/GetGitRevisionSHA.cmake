if(__get_git_revision_sha)
  return()
endif()
set(__get_git_revision_sha INCLUDED)

function(get_git_revision_sha var)
  execute_process(COMMAND git rev-parse HEAD
      RESULT_VARIABLE status
      OUTPUT_VARIABLE GIT_SHA
      ERROR_QUIET
  )
  string(STRIP ${GIT_SHA} GIT_SHA)

  # Work out if the repository is dirty
  execute_process(COMMAND git update-index -q --refresh
      OUTPUT_QUIET
      ERROR_QUIET)
  execute_process(COMMAND git diff-index --name-only HEAD --
      OUTPUT_VARIABLE GIT_DIFF_INDEX
      ERROR_QUIET)
  string(COMPARE NOTEQUAL "${GIT_DIFF_INDEX}" "" GIT_DIRTY)
  if (${GIT_DIRTY})
      set(GIT_SHA "${GIT_SHA}-dirty")
  endif()
  message("-- git sha1: ${GIT_SHA}")
  set(${var} ${GIT_SHA} PARENT_SCOPE)
endfunction()
