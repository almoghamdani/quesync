#pragma once

#include <regex>

namespace quesync {
namespace utils {
class validation {
   public:
    /**
     * Checks if a username is a valid username.
     *
     * @param username The username to be validated.
     * @return True if the username is valid or false otherwise.
     */
    static bool is_valid_username(std::string username);

    /**
     * Checks if a email is a valid email.
     *
     * @param email The email to be validated.
     * @return True if the email is valid or false otherwise.
     */
    static bool is_valid_email(std::string email);

    /**
     * Scores a password using 'zxcvbn'.
     *
     * @param password The password to be scored.
     * @return The score of the password.
     */
    static int score_password(std::string password);
};
};  // namespace utils
};  // namespace quesync
