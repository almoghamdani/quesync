import store from "./store";

import { addPingValue } from "./actions/clientActions";
import { setUser } from "./actions/authActions";
import { fetchUserProfile } from "./actions/usersActions";

export default class EventHandler {
	constructor(app) {
		this.app = app;
	}

	register = client => {
		client.registerEventHandler("friend-request", this.friendRequestEvent);
		client.registerEventHandler("ping", this.pingEvent);
	};

	friendRequestEvent = async event => {
		const state = store.getState();
		const client = state.client.client;
		const user = state.auth.user;
		const friendId = event.requester_id;
		const sentAt = event.sent_at;

		// Fetch the user's profile
		await store
			.dispatch(fetchUserProfile(client, friendId))
			.then(() => {})
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
}
