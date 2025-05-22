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

void RepositoryWrapper::init(bool bare)
{
    std::cout << "repo init - start" << std::endl;

    // what if it is already initialised???

    // convert error code to exception
    std::string path = "repo";
    throwIfError(git_repository_init(&_repo, path.c_str(), bare));

    std::cout << "repo init - end " << std::endl;
}
