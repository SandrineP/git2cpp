#pragma once

// #include <string>

#include <git2.h>

#include "../wrapper/repository_wrapper.hpp"

class branch_iterator_wrapper : public wrapper_base<git_branch_iterator>
{
public:

    ~branch_iterator_wrapper();

    branch_iterator_wrapper(branch_iterator_wrapper&&) = default;
    branch_iterator_wrapper& operator=(branch_iterator_wrapper&&) = default;

    static branch_iterator_wrapper branch_list(const repository_wrapper& wrapper);
    static git_branch_name

private:

    branch_iterator_wrapper() = default;
};
