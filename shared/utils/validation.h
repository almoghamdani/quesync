#pragma once

#include <regex>

namespace quesync {
namespace utils {
class validation {
   public:
    static bool is_valid_username(std::string username);
    static bool is_valid_email(std::string email);
    static int score_password(std::string password);
};
};  // namespace utils
};  // namespace quesync
