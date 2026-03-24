#include "../wrapper/remote_wrapper.hpp"

#include <string>
#include <vector>

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

void remote_wrapper::fetch(const git_strarray* refspecs, const git_fetch_options* opts, const char* reflog_message)
{
    throw_if_error(git_remote_fetch(*this, refspecs, opts, reflog_message));
}

void remote_wrapper::push(const git_strarray* refspecs, const git_push_options* opts)
{
    throw_if_error(git_remote_push(*this, refspecs, opts));
}

void remote_wrapper::connect(git_direction direction, const git_remote_callbacks* callbacks) const
{
    throw_if_error(git_remote_connect(*this, direction, callbacks, nullptr, nullptr));
}

std::vector<remote_head> remote_wrapper::list_heads(const git_remote_callbacks* callbacks = nullptr) const
{
    std::vector<remote_head> result;

    this->connect(GIT_DIRECTION_FETCH, callbacks);

    const git_remote_head** heads = nullptr;
    size_t heads_len = 0;
    int err = git_remote_ls(&heads, &heads_len, *this);
    if (err != 0)
    {
        git_remote_disconnect(*this);
        throw_if_error(err);
    }

    for (size_t i = 0; i < heads_len; ++i)
    {
        const git_remote_head* h = heads[i];
        if (!h || !h->name)
        {
            continue;
        }

        remote_head rh;
        rh.name = std::string(h->name);
        rh.oid = h->oid;
        result.push_back(std::move(rh));
    }

    git_remote_disconnect(*this);
    return result;
}
