#include <git2.h>

#include "../git_exception.hpp"
#include "base_wrapper.hpp"

BaseWrapper::BaseWrapper()
{
    // Fine to initialise libgit2 multiple times.
    throwIfError(git_libgit2_init());
}

BaseWrapper::~BaseWrapper()
{
    git_libgit2_shutdown();
}
