import store from "./store";

import { fetchUserProfile } from "./actions/usersActions";

export default async function update() {
	const state = store.getState();
	const client = state.client.client;
	const user = state.auth.user;

	console.log("Update started..");

	// For each friend, fetch it's profile
	for (const idx in user.friends) {
		const friendId = user.friends[idx];

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
	}

	console.log("Finished updating..");
}
