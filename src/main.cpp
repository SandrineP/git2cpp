#include <CLI/CLI.hpp>
#include <git2.h>  // For version number only
#include <iostream>

#include "git_exception.hpp"
#include "version.hpp"
#include "subcommand/init_subcommand.hpp"

int main(int argc, char** argv)
{
    int exitCode = 0;
    try
    {
        CLI::App app{"Git using C++ wrapper of libgit2"};

        // Top-level command options.
        auto version = app.add_flag("-v,--version", "Show version");

        // Sub commands
        InitSubcommand init(app);

        app.parse(argc, argv);

        if (version->count())
        {
            std::cout << "git2cpp version " << GIT2CPP_VERSION_STRING << " (libgit2 " << LIBGIT2_VERSION << ")" << std::endl;
        }
    }
    catch (const CLI::Error& e)
    {
        std::cerr << e.what() << std::endl;
        exitCode = 1;
    }
    catch (const GitException& e)
    {
        std::cerr << e.what() << std::endl;
        exitCode = e.errorCode();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exitCode = 1;
    }

    return exitCode;
}
