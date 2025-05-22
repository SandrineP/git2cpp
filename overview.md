# Overview

The motivation is to provide a `git`-like command-line interface (CLI) to the
[JupyterLite terminal](https://github.com/jupyterlite/terminal) but allow most of the development
work to occur outside of a WebAssembly toolchain for ease.

[libgit2](https://libgit2.org/) is a portable pure C implementation of `git` core functionality to
allow library access rather than a CLI. There are many different programming language wrappers for
it. It does include some CLI functionality in the form of examples, but these are not of core
interest to most maintainers and users of the library, and are therefore not complete.

The idea here is to write a C++ wrapper to `libgit2` to provide a CLI, starting with simple `git`
functionality and expanding the scope over time. All development work can occur on POSIX systems
away from a WebAssembly toolchain, and the final step is to use
[Emscripten-forge](https://emscripten-forge.org/) recipes to build it for
[cockle](https://github.com/jupyterlite/cockle), just like other commands.

There are two areas of functionality to be written in C++:
1. Thin C++ wrappers of the various `libgit2` C objects to give automatical deallocation of
   resources as necessary.
2. `git` command and subcommand implementations using [CLI11](https://github.com/CLIUtils/CLI11)
   for argument parsing, as it used in [mamba](https://github.com/mamba-org/mamba) for example.

`libgit2` is well tested and well maintained, so we do not envisage unit testing of our C++
wrappers. The testing is therefore of the CLI, and hence does not have to be performed in C++ but
can use `python` or `node` or similar instead.

We will ignore some aspects of `git` initially that are hard to implement in a single-thread
WebAssembly runtime, such as opening an editor for `git rebase` and so on. Instead we will
probably use a fairly simple sequence of prompts and accepting `stdin` inputs from the user, and
work towards full integration with editors at a later date.

The `git2cpp` name of the project and executable follow from `libgit2` which includes its own `git2`
and hence it would not be sensible to duplicate that name. In a terminal in which this is the only
`git` implementation available we would expect users to set up aliases so that the usual `git`
command name can be used.

The alternative approach which we have rejected would be to start with the `git` source code itself,
and build that for WebAssembly. That would involve removing functionality such as starting new
processes to open editors by patching the source code for the WebAssembly build. That approach would
be a much less pleasant developer experience than the one proposed here, given that all work would
have to occur using a WebAssembly toolchain.

All design decisions here are subject to change based on who the main developers will be and their
preferred tools, etc.
