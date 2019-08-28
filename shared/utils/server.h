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
    static int generate_tag(std::string nickname, sql::Table &users_table);
};
};  // namespace utils
};  // namespace quesync
#endif