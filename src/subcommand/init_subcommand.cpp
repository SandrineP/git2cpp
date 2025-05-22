#include "init_subcommand.hpp"

//#include "../wrapper/repository_wrapper.hpp"

InitSubcommand::InitSubcommand(CLI::App& app)
{
    auto *sub = app.add_subcommand("init", "Explanation of init here");

    sub->add_flag("--bare", bare, "--- bare ---");

    sub->callback([this]() { this->run(); });
}

void InitSubcommand::run()
{
    std::cout << "RUN " << bare << std::endl;
    //RepositoryWrapper repo;
    //repo.init(bare);
}
