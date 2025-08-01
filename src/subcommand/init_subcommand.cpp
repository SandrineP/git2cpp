// #include <filesystem>
#include "init_subcommand.hpp"
#include "../wrapper/repository_wrapper.hpp"

init_subcommand::init_subcommand(const libgit2_object&, CLI::App& app)
{
    auto *sub = app.add_subcommand("init", "Explanation of init here");

    sub->add_flag("--bare", m_bare, "info about bare arg");

    // If directory not specified, uses cwd.
    sub->add_option("directory", m_directory, "info about directory arg")
        ->check(CLI::ExistingDirectory | CLI::NonexistentPath)
        ->default_val(get_current_git_path());

    sub->callback([this]() { this->run(); });
}

void init_subcommand::run()
{
    repository_wrapper::init(m_directory, m_bare);
}
