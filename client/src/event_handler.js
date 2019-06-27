import { store } from "./store";

import { addPingValue } from "./actions/clientActions";
import { setUser } from "./actions/userActions";
import { fetchUserProfile } from "./actions/usersActions";
import { addMessageToChannel } from "./actions/messagesActions";

class EventHandler {
	register = client => {
		client.registerEventHandler("ping", this.pingEvent);
		client.registerEventHandler("friend-request", this.friendRequestEvent);
		client.registerEventHandler("message", this.messageEvent);
	};

	friendRequestEvent = async event => {
		const state = store.getState();
		const client = state.client.client;
		const user = state.user.user;
		const friendId = event.requesterId;
		const sentAt = event.sentAt;

		// Fetch the user's profile
		await store
			.dispatch(fetchUserProfile(client, friendId))
			.then(() => { })
			.catch(() => {
				console.error(
					"An error occurred fetching the profile of the user {0}",
					friendId
				);
			});

		// Add the friend request
		await store.dispatch(
			setUser({
				...user,
				friendRequests: user.friendRequests.concat([
					{ friendId, sentAt, friendType: "requester" }
				])
			})
		);
	};

	pingEvent = async event => {
		const pingValue = event.ping;

		// Add the ping value to the list of pings
		await store.dispatch(addPingValue(pingValue));
	};

	messageEvent = event => {
		var message = { ...event.message };
		const channelId = message.channelId;

		console.log(message)

		// Remove channel id field from message
		delete message.channelId;

		// Add message to channel
		store.dispatch(addMessageToChannel(message, channelId));
	}
}

export default new EventHandler()