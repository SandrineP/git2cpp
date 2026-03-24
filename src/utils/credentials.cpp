#include "credentials.hpp"

#include <iostream>

#include <git2/credential.h>

#include "input_output.hpp"

// git_credential_acquire_cb
int user_credentials(
    git_credential** out,
    const char* url,
    const char* username_from_url,
    unsigned int allowed_types,
    void* payload
)
{
    credentials_payload* cached = payload ? static_cast<credentials_payload*>(payload) : nullptr;

    // Check for cached credentials here, if desired.
    // It might be necessary to make this function stateful to avoid repeating unnecessary checks.

    *out = nullptr;

    if (allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT)
    {
        std::string username;
        if (username_from_url && username_from_url[0] != '\0')
        {
            username = username_from_url;
        }
        else if (cached && cached->username.has_value())
        {
            username = *cached->username;
        }
        else
        {
            username = prompt_input("Username: ");
            if (cached && !username.empty())
            {
                cached->username = username;
            }
        }

        if (username.empty())
        {
            giterr_set_str(GIT_ERROR_HTTP, "No username specified");
            return GIT_EAUTH;
        }

        std::string password = prompt_input("Password: ", false);
        if (cached && cached->password.has_value())
        {
            password = *cached->password;
        }
        else
        {
            password = prompt_input("Password: ", false);
            if (cached && !password.empty())
            {
                cached->password = password;
            }
        }

        if (password.empty())
        {
            giterr_set_str(GIT_ERROR_HTTP, "No password specified");
            return GIT_EAUTH;
        }

        // If successful, this will create and return a git_credential* in the out argument.
        return git_credential_userpass_plaintext_new(out, username.c_str(), password.c_str());
    }

    giterr_set_str(GIT_ERROR_HTTP, "Unexpected credentials request");
    return GIT_ERROR;
}
