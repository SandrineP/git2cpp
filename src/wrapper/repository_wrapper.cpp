#include <iostream>
#include <string>

#include "../git_exception.hpp"
#include "repository_wrapper.hpp"

RepositoryWrapper::RepositoryWrapper()
    : _repo(nullptr)
{}

RepositoryWrapper::~RepositoryWrapper()
{
    if (_repo != nullptr) {
        git_repository_free(_repo);  // no return
    }
}

void RepositoryWrapper::init(const std::string& directory, bool bare)
{
    // what if it is already initialised?  Throw exception or delete and recreate?
    throwIfError(git_repository_init(&_repo, directory.c_str(), bare));
}
