#pragma once
#include "manager.h"

#include <vector>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "../../shared/channel.h"

namespace quesync {
namespace server {
// Prevent loop header include
class session;

class channel_manager : manager {
   public:
    /**
     * Channel manager constructor.
     *
     * @param server A shared pointer to the server object.
     */
    channel_manager(std::shared_ptr<server> server);

    /**
     * Checks if a channel exists
     *
     * @param channel_id The id of the channel to check if exists.
     * @return True if the channel exists or false otherwise.
     */
    bool does_channel_exists(std::string channel_id);

    /**
     * Get the private channel of 2 users.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param user_id The id of the user to get their private channel.
     * @return A shared pointer to the private channel object.
     */
    std::shared_ptr<channel> get_private_channel(std::shared_ptr<session> sess,
                                                 std::string user_id);

    /**
     * Get channel info.
     *
     * @param channel_id The id of the channel.
     * @return A shared pointer to the channel object.
     */
    std::shared_ptr<channel> get_channel(std::string channel_id);

    /**
     * Checks if a user is part of a channel.
     *
     * @param user_id The id of the user to check if he is part of the channel.
     * @param channel_id The id of the channel.
     * @return True if the user is part of the channel or false otherwise.
     */
    bool is_user_member_of_channel(std::string user_id, std::string channel_id);

    /**
     * Adds a user to a channel as a member.
     *
     * @param channel_id The id of the channel.
     * @param member_id The id of the user to add as a member to a channel.
     */
    void add_member_to_channel(std::string channel_id, std::string member_id);

    /**
     * Get all channel members.
     *
     * @param sess A shared pointer to the session object of the user.
     * @param channel_id The id of the channel.
     * @return A vector containing all user ids of the members in the channel.
     */
    std::vector<std::string> get_channel_members(std::shared_ptr<session> sess,
                                                 std::string channel_id);

   private:
    std::shared_ptr<channel> create_channel(sql::Session &sql_sess, bool is_private);
};
};  // namespace server
};  // namespace quesync
