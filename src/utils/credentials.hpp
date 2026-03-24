#pragma once

#include <optional>
#include <string>

#include <git2/credential.h>

struct credentials_payload
{
    std::optional<std::string> username;
    std::optional<std::string> password;
};

// Libgit2 callback of type git_credential_acquire_cb to obtain user credentials
// (username and password) to authenticate remote https access.
int user_credentials(
    git_credential** out,
    const char* url,
    const char* username_from_url,
    unsigned int allowed_types,
    void* payload
);
