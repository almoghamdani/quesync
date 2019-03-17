#pragma once

#include <regex>

class Validation
{
  public:
    static bool IsValidUsername(std::string username);
    static bool IsValidEmail(std::string email);
    static int ScorePassword(std::string password);
};