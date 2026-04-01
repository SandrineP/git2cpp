#include "../subcommand/push_subcommand.hpp"

#include <iostream>
#include <optional>

#include <git2/net.h>
#include <git2/remote.h>
#include <git2/types.h>

#include "../utils/ansi_code.hpp"
#include "../utils/credentials.hpp"
#include "../utils/progress.hpp"
#include "../wrapper/repository_wrapper.hpp"

push_subcommand::push_subcommand(const libgit2_object&, CLI::App& app)
{
    auto* sub = app.add_subcommand("push", "Update remote refs along with associated objects");

    sub->add_option("<remote>", m_remote_name, "The remote to push to")->default_val("origin");
    sub->add_option("<branch>", m_branch_name, "The branch to push");
    sub->add_option("<refspec>", m_refspecs, "The refspec(s) to push");
    sub->add_flag(
        "--all,--branches",
        m_branches_flag,
        "Push all branches (i.e. refs under " + ansi_code::bold + "refs/heads/" + ansi_code::reset
            + "); cannot be used with other <refspec>."
    );


    sub->callback(
        [this]()
        {
            this->run();
        }
    );
}

int credential_cb(git_cred **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload) {
    // Replace with your actual credentials
    const char* username = user_credentials;
    const char *password = "your_password_or_token";

    return git_cred_userpass_plaintext_new(out, username, password);
}

void push_subcommand::run()
{
    auto directory = get_current_git_path();
    auto repo = repository_wrapper::open(directory);

    std::string remote_name = m_remote_name.empty() ? "origin" : m_remote_name;
    auto remote = repo.find_remote(remote_name);

    git_direction direction = GIT_DIRECTION_FETCH;

    // remote.connect(direction, );
    // auto remote_branches_ante_push = remote.ls();

    git_push_options push_opts = GIT_PUSH_OPTIONS_INIT;
    push_opts.callbacks.credentials = user_credentials;
    push_opts.callbacks.push_transfer_progress = push_transfer_progress;
    push_opts.callbacks.push_update_reference = push_update_reference;

    if (m_branches_flag)
    {
        auto iter = repo.iterate_branches(GIT_BRANCH_LOCAL);
        auto br = iter.next();
        while (br)
        {
            std::string refspec = "refs/heads/" + std::string(br->name());
            m_refspecs.push_back(refspec);
            br = iter.next();
        }
    }
    else if (m_refspecs.empty())
    {
        std::string branch;
        if (!m_branch_name.empty())
        {
            branch = m_branch_name;
        }
        else
        {
            try
            {
                auto head_ref = repo.head();
                branch = head_ref.short_name();
            }
            catch (...)
            {
                std::cerr << "Could not determine current branch to push." << std::endl;
                return;
            }
        }
        std::string refspec = "refs/heads/" + branch;
        m_refspecs.push_back(refspec);
    }
    git_strarray_wrapper refspecs_wrapper(m_refspecs);
    git_strarray* refspecs_ptr = nullptr;
    refspecs_ptr = refspecs_wrapper;

    remote.push(refspecs_ptr, &push_opts);
    auto remote_branches_post_push = remote.ls();

    std::cout << "To " << remote.url() << std::endl;
    for (const auto& refspec : m_refspecs)
    {
        std::string_view ref_view(refspec);
        std::string_view prefix = "refs/heads/";
        std::string short_name;
        if (ref_view.substr(0, prefix.size()) == prefix)
        {
            short_name = ref_view.substr(prefix.size());
        }
        else
        {
            short_name = refspec;
        }

        // std::optional<std::string> branch_upstream_name = repo.branch_upstream_name(short_name);
        std::string upstream_name;
        upstream_name = short_name;
        // if (branch_upstream_name.has_value())
        // {
        //     upstream_name = branch_upstream_name.value();
        // }
        // else
        // {
        //     // ???
        // }
        // if (std::find(remote_branches.begin(), remote_branches.end(), short_name) == remote_branches.end())
        // {
        //     std::cout << " * [new branch]      " << short_name << " -> " << short_name << std::endl;
        // }
        //
        // if (std::find(remote_branches.begin(), remote_branches.end(), short_name) == remote_branches.end())
        // {
        //     std::cout << " * [new branch]      " << short_name << " -> " << short_name << std::endl;
        // }

        auto ref = repo.find_reference(ref_view);
        if (!ref.is_remote())
        {
            std::cout << " * [new branch]      " << short_name << " -> " << upstream_name << std::endl;
        }
        // std::cout << " * [new branch]      " << short_name << " -> " << short_name << std::endl;
    }
}
