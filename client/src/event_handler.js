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
		client.registerEventHandler("friendship-status", this.friendshipStatusEvent);
		client.registerEventHandler("message", this.messageEvent);
	};

	pingEvent = async event => {
		const pingValue = event.ping;

		// Add the ping value to the list of pings
		await store.dispatch(addPingValue(pingValue));
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
				friendRequests: [...user.friendRequests, { friendId, sentAt, friendType: "requester" }]
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
		});
	};

	friendshipStatusEvent = async event => {
		const state = store.getState();
		const user = state.user.user;
		const { friendId, status } = event;

		// Try to find the friend request
		const friendRequest = user.friendRequests.find(req => req.friendId === friendId);

		// If the friend request has been found
		if (friendRequest) {
			// If the friend request has been approved
			if (status === true) {
				// Add the friend to the friends list and remove him from the friend requests
				await store.dispatch(
					setUser({
						...user,
						friendRequests: user.friendRequests.filter(req => req.friendId !== friendId),
						friends: [...user.friends, friendId]
					})
				);

				// Show a notification with the new friend
				queue.notify({
					title: <b>Friend Request Approved</b>,
					body: <span><b>{store.getState().users.profiles[friendId].nickname}</b> has approved your friend request</span>,
					icon: "person_add",
					actions: [
						{
							title: 'Dismiss'
						}
					]
				});
			} else {
				// Remove the friend from the friend requests
				await store.dispatch(
					setUser({
						...user,
						friendRequests: user.friendRequests.filter(req => req.friendId !== friendId)
					})
				);

				// Show a notification with the rejected friend request
				queue.notify({
					title: <b>{friendRequest.friendType === "requester" ? "Friend Request Removed" : "Friend Request Rejected"}</b>,
					body: <span><b>{store.getState().users.profiles[friendId].nickname}</b> has {friendRequest.friendType === "requester" ? "removed his friend request to you" : "rejected your friend request"}</span>,
					icon: "person_add",
					actions: [
						{
							title: 'Dismiss'
						}
					]
				});
			}
		} else if (status === false && user.friends.indexOf(friendId) !== -1) {
			// Remove the friend from the friends list
			await store.dispatch(
				setUser({
					...user,
					friendRequests: user.friends.splice(user.friends.indexOf(friendId), 1)
				})
			);

			// Show a notification with the rejected friend request
			queue.notify({
				title: <b>Friend Removed</b>,
				body: <span><b>{store.getState().users.profiles[friendId].nickname}</b> has removed you from his friends list</span>,
				icon: "person",
				actions: [
					{
						title: 'Dismiss'
					}
				]
			});
		}
	}

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