#include "../wrapper/remote_wrapper.hpp"

#include <string>
#include <vector>

#include <git2/remote.h>
#include <git2/types.h>

#include "../utils/git_exception.hpp"

remote_wrapper::remote_wrapper(git_remote* remote)
    : base_type(remote)
{
}

remote_wrapper::~remote_wrapper()
{
    git_remote_free(p_resource);
    p_resource = nullptr;
}

std::string_view remote_wrapper::name() const
{
    const char* out = git_remote_name(*this);
    return out ? std::string_view(out) : std::string_view();
}

std::string_view remote_wrapper::url() const
{
    const char* out = git_remote_url(*this);
    return out ? std::string_view(out) : std::string_view();
}

std::string_view remote_wrapper::pushurl() const
{
    const char* out = git_remote_pushurl(*this);
    return out ? std::string_view(out) : std::string_view();
}

std::vector<std::string> remote_wrapper::refspecs() const
{
    git_strarray refspecs = {0};
    std::vector<std::string> result;

    if (git_remote_get_fetch_refspecs(&refspecs, *this) == 0)
    {
        for (size_t i = 0; i < refspecs.count; ++i)
        {
            result.emplace_back(refspecs.strings[i]);
        }
        git_strarray_dispose(&refspecs);
    }

    return result;
}

std::vector<const git_remote_head*> remote_wrapper::ls() const
{
    const git_remote_head** remote_heads;
    size_t remote_heads_size;
    throw_if_error(git_remote_ls(&remote_heads, &remote_heads_size, *this));

    std::vector<const git_remote_head*> remote_heads_vec;
    for (size_t i = 0; i < remote_heads_size; i++)
   {
       remote_heads_vec.push_back(remote_heads[i]);
   }
    return remote_heads_vec;
}


// std::vector<std::string> remote_wrapper::ls() const
// {
//     const git_remote_head** remote_heads;
//     size_t remote_heads_size;
//     throw_if_error(git_remote_ls(&remote_heads, &remote_heads_size, *this));

//     std::vector<std::string> remote_branches;
//     for (size_t i = 0; i < remote_heads_size; i++)
//    {
//        const git_remote_head* head = remote_heads[i];
//        if (!head->local)
//        {
//            remote_branches.push_back(head->name);
//        }
//    }
//     return remote_branches;
// }

void remote_wrapper::connect(git_direction direction, const git_remote_callbacks* callbacks)
{
    throw_if_error(git_remote_connect(*this, direction, callbacks, NULL, NULL));
}

void remote_wrapper::fetch(const git_strarray* refspecs, const git_fetch_options* opts, const char* reflog_message)
{
    throw_if_error(git_remote_fetch(*this, refspecs, opts, reflog_message));
}

void remote_wrapper::push(const git_strarray* refspecs, const git_push_options* opts)
{
    throw_if_error(git_remote_push(*this, refspecs, opts));
}
