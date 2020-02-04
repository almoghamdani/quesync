#pragma once
#include "module.h"

#include <string>
#include <vector>

#include "../../../../shared/friend_request.h"
#include "../../../../shared/profile.h"

namespace quesync {
namespace client {
namespace modules {
class users : public module {
   public:
    /**
     * Users module constructor.
     *
     * @param client A shared pointer to the client object.
     */
    users(std::shared_ptr<client> client);

    /**
     * Get a user's profile info.
     *
     * @param user_id The id of the user to get it's profile.
     * @return A shared pointer to the profile info.
     */
    std::shared_ptr<profile> get_user_profile(std::string user_id);

    /**
     * Search a user by nickname and optional tag.
     *
     * @param nickname The nickname of the user.
     * @param tag The tag of the user.
     * @return A vector containing the search results.
     */
    std::vector<profile> search(std::string nickname, int tag = -1);

    /**
     * Sends a friend request to a user.
     *
     * @param friend_id The id of the user to send friend request to.
     * @return A shared pointer to the generated friend request object.
     */
    std::shared_ptr<friend_request> send_friend_request(std::string friend_id);

    /**
     * Sets the friendship status between the user and a friend.
     *
     * @param friend_id The id of the friend to set their friendship status.
     * @param status The new status of their friendship.
     */
    void set_friendship_status(std::string friend_id, bool status);
};
};  // namespace modules
};  // namespace client
};  // namespace quesync