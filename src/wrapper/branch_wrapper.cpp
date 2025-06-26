// #include "../utils/git_exception.hpp"
#include "branch_wrapper.hpp"


branch_iterator_wrapper::~branch_iterator_wrapper()
{
    git_branch_iterator_free(p_resource);
    p_resource=nullptr;
}

branch_iterator_wrapper branch_iterator_wrapper::branch_list(const repository_wrapper &wrapper)
{

}

// repository_wrapper repository_wrapper::open(const std::string& directory)
// {
//     repository_wrapper rw;
//     throwIfError(git_repository_open(&(rw.p_ressource), directory.c_str()));
//     return rw;
// }

// repository_wrapper repository_wrapper::init(const std::string& directory, bool bare)
// {
//     repository_wrapper rw;
//     throwIfError(git_repository_init(&(rw.p_ressource), directory.c_str(), bare));
//     return rw;
// }
