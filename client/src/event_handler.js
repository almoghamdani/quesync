import React from 'react';

import { store } from "./store";

import { addPingValue } from "./actions/clientActions";
import { setUser } from "./actions/userActions";
import { addMessageToChannel } from "./actions/messagesActions";

import updater from "./updater"
import { queue } from "./messages_queue";

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

		// Update the user
		await updater.updateUser(client, friendId);

		// Add the friend request
		await store.dispatch(
			setUser({
				...user,
				friendRequests: user.friendRequests.concat([
					{ friendId, sentAt, friendType: "requester" }
				])
			})
		);

		// Show a notification with the new friend request
		queue.notify({
			title: <b>New Friend Request</b>,
			body: <span>You have a new friend request from <b>{store.getState().users.profiles[friendId].nickname}</b></span>,
			icon: "person_add",
			actions: [
				{
					title: 'Dismiss'
				}
			]
		})
	};

	pingEvent = async event => {
		const pingValue = event.ping;

		// Add the ping value to the list of pings
		await store.dispatch(addPingValue(pingValue));
	};

	messageEvent = event => {
		var message = { ...event.message };
		const channelId = message.channelId;

		// Remove channel id field from message
		delete message.channelId;

		// Add message to channel
		store.dispatch(addMessageToChannel(message, channelId));
	}
}

export default new EventHandler()