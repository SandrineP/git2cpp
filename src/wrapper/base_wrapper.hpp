#pragma once

#include <git2.h>

class BaseWrapper
{
public:
    BaseWrapper();

    virtual ~BaseWrapper();

    BaseWrapper(const BaseWrapper&) = delete;
    BaseWrapper& operator=(const BaseWrapper&) = delete;
    BaseWrapper(BaseWrapper&&) = delete;
    BaseWrapper& operator=(BaseWrapper&&) = delete;
};
