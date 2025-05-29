#pragma once

#include "base_wrapper.hpp"

class RepositoryWrapper : public BaseWrapper
{
public:
    RepositoryWrapper();

    virtual ~RepositoryWrapper();

    void init(const std::string& directory, bool bare);

private:
    git_repository *_repo;
};
