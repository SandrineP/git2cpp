#pragma once

#include <CLI/CLI.hpp>

class BaseSubcommand
{
public:
    BaseSubcommand() = default;

    virtual ~BaseSubcommand() = default;

    BaseSubcommand(const BaseSubcommand&) = delete;
    BaseSubcommand& operator=(const BaseSubcommand&) = delete;
    BaseSubcommand(BaseSubcommand&&) = delete;
    BaseSubcommand& operator=(BaseSubcommand&&) = delete;
};
