#pragma once

#include <string>
#include "base_subcommand.hpp"

class InitSubcommand : public BaseSubcommand
{
public:
    InitSubcommand(CLI::App& app);
    void run();

private:
    bool bare;
    std::string directory;
};
