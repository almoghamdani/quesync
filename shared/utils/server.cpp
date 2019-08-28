#include "server.h"

#include "../exception.h"
#include "rand.h"

#ifdef QUESYNC_SERVER
int quesync::utils::server::generate_tag(std::string nickname, sql::Table &users_table) {
    int tag;
    std::vector<int> tags;

    // Get all tags from the users table
    std::list<sql::Row> res = users_table.select("tag")
                                  .where("nickname = :nickname")
                                  .bind("nickname", nickname)
                                  .execute()
                                  .fetchAll();

    // Move all tags found into a vector
    for (auto it = res.begin(); it != res.end(); it++) {
        tags.push_back((*it)[0]);
    }

    // If all the tags are used, throw exception
    if (tags.size() == MAX_TAG) {
        throw exception(error::nickname_full);
    }

    // Generate tags until found one that is free
    do {
        tag = rand::num(0, MAX_TAG);
    } while (find(tags.begin(), tags.end(), tag) != tags.end());

    return tag;
}
#endif