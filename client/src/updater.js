import store from "./store";

import { getChannelMessages } from "./actions/messagesActions";
import { getPrivateChannel } from "./actions/channelsActions";
import { fetchUserProfile } from "./actions/usersActions";

import Logger from "./logger";

const AMOUNT_OF_INITIAL_MESSAGES = 100;

class Updater {
	logger = new Logger("Updater", "#00b2ff");

	update = async () => {
		const state = store.getState();
		const client = state.client.client;
		const user = state.user.user;

		this.logger.info("Update started!");

		// Get the user's profile
		await store
			.dispatch(fetchUserProfile(client, user.id))
			.then(() => {})
			.catch((ex) => {
				this.logger.error(
					`An error occurred fetching the user's profile. Error: ${ex}`
				);
			});

		// For each friend, fetch it's profile and private channel
		this.logger.info("Fetching friends' profiles!");
		for (const idx in user.friends) {
			const friendId = user.friends[idx];

			// Update the user
			this.updateUser(client, friendId);
		}

		// For each pending friend, fetch it's profile
		this.logger.info("Fetching pending friends' profiles!");
		for (const idx in user.friendRequests) {
			const friendId = user.friendRequests[idx].friendId;

			// Update the user
			this.updateUser(client, friendId);
		}

		this.logger.info("Update finished!");
	}

	updateUser = async (client, userId) => {
		// Fetch the user's profile
		await store
			.dispatch(fetchUserProfile(client, userId))
			.then(async () => {
				// Get the user's private channel
				await store
					.dispatch(getPrivateChannel(client, userId))
					.then(async (res) => {
						const channelId = res.action.payload.channel.id

						// Update the channel
						await this.updateChannel(client, channelId);
					})
					.catch((ex) => {
						this.logger.error(
							`An error occurred getting the private channel of the user ${userId}. Error: ${ex}`
						);
					});
			})
			.catch((ex) => {
				this.logger.error(
					`An error occurred fetching the profile of the user ${userId}. Error: ${ex}`
				);
			});
	}

	updateChannel = async (client, channelId) => {
		// Get the channel's messages
		await store
			.dispatch(getChannelMessages(client, channelId, AMOUNT_OF_INITIAL_MESSAGES, 0))
			.then(() => { })
			.catch((ex) => {
				this.logger.error(
					`An error occurred getting the messages of the channel ${channelId}. Error: ${ex}`
				);
			});
	}
}

const updater = new Updater();

export default updater;