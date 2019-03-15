#include "validation.h"

bool Validation::IsValidUsername(std::string username)
{
    // Check if the username is a valid username using the username regex
    return std::regex_search(username, std::regex(R"(^[a-zA-Z0-9]+([_ -]?[a-zA-Z0-9]){3,20}$)"));
}

bool Validation::IsValidEmail(std::string email)
{
    // Check if the e-mail is a valid e-mail using the e-mail regex
    return std::regex_search(email, std::regex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)"));
}
