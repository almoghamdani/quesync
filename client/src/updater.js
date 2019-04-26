import store from "./store";

import { fetchUserProfile } from "./actions/usersActions";

import Logger from "./logger";

const logger = new Logger("Updater", "#00b2ff");

export default async function update() {
	const state = store.getState();
	const client = state.client.client;
	const user = state.auth.user;

	logger.info("Update started!");

	// For each fend, fetch it's profile
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

	logger.info("Update finished!");
}
