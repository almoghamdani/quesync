#pragma once
#include "module.h"

#include <string>
#include <vector>

namespace quesync {
namespace client {
namespace modules {
class user : public module {
   public:
    /**
     * User module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    user(std::shared_ptr<client> client);

    /**
     * Set the profile photo of the user
     *
     * @param file_id The id of the profile photo file.
     */
    void set_profile_photo(std::string file_id);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync