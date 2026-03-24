#include "../subcommand/push_subcommand.hpp"

#include <iostream>
#include <optional>
#include <unordered_map>
#include <string_view>

#include <git2.h>

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

// TODO: put in common
static std::string oid_to_hex(const git_oid& oid)
{
    char oid_str[GIT_OID_SHA1_HEXSIZE + 1];
    git_oid_fmt(oid_str, &oid);
    oid_str[GIT_OID_SHA1_HEXSIZE] = '\0';
    return std::string(oid_str);
}

void push_subcommand::run()
{
    auto directory = get_current_git_path();
    auto repo = repository_wrapper::open(directory);

    std::string remote_name = m_remote_name.empty() ? "origin" : m_remote_name;
    auto remote = repo.find_remote(remote_name);

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

    // Take a snapshot of remote branches to check which ones are new after push
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
    callbacks.credentials = user_credentials;
    credentials_payload creds_payload;
    callbacks.payload = &creds_payload;
    push_opts.callbacks.payload = &creds_payload;

    auto remote_heads = remote.list_heads(&callbacks);

    // Map with names of branches and their oids before push
    std::unordered_map<std::string, git_oid> remote_heads_map;
    for (const auto& h : remote_heads)
    {
        remote_heads_map.emplace(h.name, h.oid);
    }

    remote.push(refspecs_ptr, &push_opts);

    std::cout << "To " << remote.url() << std::endl;
    for (const auto& refspec : m_refspecs)
    {
        std::string_view ref_view(refspec);
        std::string_view prefix = "refs/heads/";
        std::string local_short_name;
        if (ref_view.substr(0, prefix.size()) == prefix)
        {
            local_short_name = ref_view.substr(prefix.size());
        }
        else
        {
            local_short_name = refspec;
        }

        std::optional<std::string> upstream_opt = repo.branch_upstream_name(local_short_name);

        std::string remote_branch = local_short_name;
        std::string remote_ref = "refs/heads/" + local_short_name;
        if (upstream_opt.has_value())
        {
            const std::string up_name = upstream_opt.value();
            auto pos = up_name.find('/');
            if (pos != std::string::npos && pos + 1 < up_name.size())
            {
                std::string up_remote = up_name.substr(0, pos);
                std::string up_branch = up_name.substr(pos + 1);
                if(up_remote == remote_name)
                {
                    remote_branch = up_name.substr(pos + 1);
                    remote_ref = "refs/heads/" + remote_branch;
                }
            }
        }

        auto iter = remote_heads_map.find(remote_ref);
        if (iter == remote_heads_map.end())
        {
            std::cout << " * [new branch]      " << local_short_name << " -> " << remote_branch << std::endl;
            continue;
        }

        git_oid remote_oid = iter->second;

        std::optional<git_oid> local_oid_opt;
        if (auto ref_opt = repo.find_reference_dwim(("refs/heads/" + local_short_name)))
        {
            const git_oid* target = ref_opt->target();
            local_oid_opt = *target;    // TODO: pas comprenu pourquoi je ne peux pas faire local_oid_opt = ref_opt->target();
        }

        if (!local_oid_opt)
        {
            std::cout << "   " << local_short_name << " -> " << remote_branch << std::endl;
            continue;
        }
        git_oid local_oid = local_oid_opt.value();

        if (!git_oid_equal(&remote_oid, &local_oid))
        {
            std::string old_hex = oid_to_hex(remote_oid);
            std::string new_hex = oid_to_hex(local_oid);
            // TODO: check order of hex codes
            std::cout << "   " << old_hex.substr(0, 7) << ".." << new_hex.substr(0, 7)
                        << "  " << local_short_name << " -> " << local_short_name << std::endl;
        }
    }
}
