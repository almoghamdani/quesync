#pragma once

#ifdef QUESYNC_SERVER
#include <string>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#define MAX_TAG 9999

namespace quesync {
namespace utils {
class server {
   public:
    /**
     * Generate a tag.
     *
     * @param nickname The nickname to be attached for the tag.
     * @param users_table The users SQL table.
     * @return The new tag for the user.
     */
    static int generate_tag(std::string nickname, sql::Table &users_table);
};
};  // namespace utils
};  // namespace quesync
#endif