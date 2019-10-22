#pragma once
#include "module.h"

#include <string>
#include <vector>

namespace quesync {
namespace client {
namespace modules {
class user : public module {
   public:
    user(std::shared_ptr<client> client);

    void set_profile_photo(std::string file_id);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync