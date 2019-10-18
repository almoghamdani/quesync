import { store } from "./store";

import { sendFriendRequest } from "./actions/userActions";
import { getChannelMessages } from "./actions/messagesActions";
import { getPrivateChannel } from "./actions/channelsActions";
import { fetchUserProfile } from "./actions/usersActions";
import { setActiveCallInChannel, getChannelCalls } from "./actions/voiceActions";
import { resetUI } from "./actions/uiActions";

import Logger from "./logger";

const AMOUNT_OF_INITIAL_MESSAGES = 100;

class Updater {
	logger = new Logger("Updater", "#00b2ff");

	update = async () => {
		const state = store.getState();
		const user = state.user.user;

		// If the connected user isn't the last connected user, reset the UI persist store
		if (user.id !== this.getLastLoggedUserId()) {
			// Reset the UI
			store.dispatch(resetUI());

			// Set the last logged user
			this.setLastLoggedUserId(user.id);
		}

		this.logger.info("Update started!");

		// Get the user's profile
		await store
			.dispatch(fetchUserProfile(user.id))
			.then(() => { })
			.catch(ex => {
				this.logger.error(
					`An error occurred fetching the user's profile. Error: ${ex}`
				);
			});

		// For each friend, fetch it's profile and private channel
		this.logger.info("Fetching friends' profiles!");
		for (const idx in user.friends) {
			const friendId = user.friends[idx];

			// Update the user
			await this.updateUser(friendId);
		}

		// For each pending friend, fetch it's profile
		this.logger.info("Fetching pending friends' profiles!");
		for (const idx in user.friendRequests) {
			const friendId = user.friendRequests[idx].friendId;

			// Update the user
			await this.updateUser(friendId);
		}

		// Fetch the user that is currently selected in the UI if not already fetched
		this.logger.info("Fetching the currently selected user if needed!");
		const currentlySelectedUserId =
			state.ui.items.selectedFriendsPageDrawerItemId;
		if (
			currentlySelectedUserId &&
			!Object.keys(store.getState().users.profiles).includes(
				currentlySelectedUserId
			)
		) {
			await this.updateUser(currentlySelectedUserId);
		}

		this.logger.info("Update finished!");
	};

	updateUser = async (userId) => {
		// Fetch the user's profile
		await store
			.dispatch(fetchUserProfile(userId))
			.then(async () => {
				// Get the user's private channel
				await store
					.dispatch(getPrivateChannel(userId))
					.then(async res => {
						const channelId = res.action.payload.channel.id;
						const callActive = res.action.payload.channel.callActive;

						// If a call is active in the channel
						if (callActive) {
							store.dispatch(setActiveCallInChannel(channelId));
						}

						// Update the channel
						await this.updateChannel(channelId);
					})
					.catch(ex => {
						this.logger.error(
							`An error occurred getting the private channel of the user ${userId}. Error: ${ex}`
						);
					});
			})
			.catch(ex => {
				this.logger.error(
					`An error occurred fetching the profile of the user ${userId}. Error: ${ex}`
				);
			});
	};

	updateChannel = async (channelId) => {
		// Get the channel's messages
		await store
			.dispatch(
				getChannelMessages(channelId, AMOUNT_OF_INITIAL_MESSAGES, 0)
			)
			.then(() => store
				.dispatch(
					getChannelCalls(channelId, AMOUNT_OF_INITIAL_MESSAGES, 0) // Get channel calls
				))
			.catch(ex => {
				this.logger.error(
					`An error occurred getting the messages of the channel ${channelId}. Error: ${ex}`
				);
			});
	};

	getLastLoggedUserId = () => localStorage.getItem("lastLoggedUserId");
	setLastLoggedUserId = userId =>
		localStorage.setItem("lastLoggedUserId", userId);

	sendFriendRequest = async (userId) => {
		// Fetch the user's profile
		await store
			.dispatch(fetchUserProfile(userId))
			.then(() => store.dispatch(sendFriendRequest(userId))) // Send friend request
			.then(() => store.dispatch(getPrivateChannel(userId))) // Get the private channel
			.then(res => this.updateChannel(res.action.payload.channel.id)) // Update the channel
			.catch(ex =>
				this.logger.error(
					`An error occurred sending the friend request to the user ${userId}. Error: ${ex}`
				)
			);
	};
}

const updater = new Updater();

export default updater;
