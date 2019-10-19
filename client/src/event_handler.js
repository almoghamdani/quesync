import React from "react";

import { store } from "./store";

import { addPingValue, disconnected } from "./actions/clientActions";
import { setUser } from "./actions/userActions";
import { setFriendsPageSelectedDrawerItemId } from "./actions/itemsActions";
import { addMessageToChannel } from "./actions/messagesActions";
import {
	joinCall,
	updateVoiceState,
	updateVoiceActivationState,
	removeActiveCall,
	addIncomingCall
} from "./actions/voiceActions";

import updater from "./updater";
import { queue } from "./messages_queue";

const { ipcRenderer } = window.require("electron");

class EventHandler {
	register = client => {
		client.registerEventHandler("ping", this.pingEvent);
		client.registerEventHandler("friend-request", this.friendRequestEvent);
		client.registerEventHandler(
			"friendship-status",
			this.friendshipStatusEvent
		);
		client.registerEventHandler("message", this.messageEvent);
		client.registerEventHandler("incoming-call", this.incomingCallEvent);
		client.registerEventHandler("voice-state", this.voiceStateEvent);
		client.registerEventHandler("voice-activity", this.voiceActivationEvent);
		client.registerEventHandler("call-ended", this.callEndedEvent);
		client.registerEventHandler("server-disconnect", this.serverDisconnectEvent);

		ipcRenderer.on("join-call", (_, channelId) => {
			// Close the window
			ipcRenderer.send("close-call-window", channelId);

			// Join the call
			store.dispatch(joinCall(channelId));
		});
	};

	pingEvent = async event => {
		const pingValue = event.ms;

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
				friendRequests: [
					...user.friendRequests,
					{ friendId, sentAt, friendType: "requester" }
				]
			})
		);

		// Show a notification with the new friend request
		queue.notify({
			title: <b>New Friend Request</b>,
			body: (
				<span>
					You have a new friend request from{" "}
					<b>{store.getState().users.profiles[friendId].nickname}</b>
				</span>
			),
			icon: "person_add",
			actions: [
				{
					title: "Dismiss"
				}
			]
		});
	};

	friendshipStatusEvent = async event => {
		const state = store.getState();
		const user = state.user.user;
		const { friendId, status } = event;

		// Try to find the friend request
		const friendRequest = user.friendRequests.find(
			req => req.friendId === friendId
		);

		// If the friend request has been found
		if (friendRequest) {
			// If the friend request has been approved
			if (status === true) {
				// Add the friend to the friends list and remove him from the friend requests
				await store.dispatch(
					setUser({
						...user,
						friendRequests: user.friendRequests.filter(
							req => req.friendId !== friendId
						),
						friends: [...user.friends, friendId]
					})
				);

				// Show a notification with the new friend
				queue.notify({
					title: <b>Friend Request Approved</b>,
					body: (
						<span>
							<b>{store.getState().users.profiles[friendId].nickname}</b> has
							approved your friend request
						</span>
					),
					icon: "person_add",
					actions: [
						{
							title: "Dismiss"
						}
					]
				});
			} else {
				// Remove the friend from the friend requests
				await store.dispatch(
					setUser({
						...user,
						friendRequests: user.friendRequests.filter(
							req => req.friendId !== friendId
						)
					})
				);

				// Show a notification with the rejected friend request
				queue.notify({
					title: (
						<b>
							{friendRequest.friendType === "requester"
								? "Friend Request Removed"
								: "Friend Request Rejected"}
						</b>
					),
					body: (
						<span>
							<b>{store.getState().users.profiles[friendId].nickname}</b> has{" "}
							{friendRequest.friendType === "requester"
								? "removed his friend request to you"
								: "rejected your friend request"}
						</span>
					),
					icon: "person_add",
					actions: [
						{
							title: "Dismiss"
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
				body: (
					<span>
						<b>{store.getState().users.profiles[friendId].nickname}</b> has
						removed you from his friends list
					</span>
				),
				icon: "person",
				actions: [
					{
						title: "Dismiss"
					}
				]
			});
		}
	};

	messageEvent = event => {
		var message = { ...event.message };
		const channelId = message.channelId;
		const senderId = message.senderId;

		// Remove channel id field from message
		delete message.channelId;

		// Add message to channel
		store.dispatch(addMessageToChannel(message, channelId));

		// Show a notification with the the new message
		queue.notify({
			title: <b>New Message</b>,
			body: (
				<span>
					<b>{store.getState().users.profiles[senderId].nickname}</b> had sent you a new message
				</span>
			),
			icon: "message",
			actions: [
				{
					title: "Dismiss"
				},
				{
					title: "Show",
					onClick: () =>
						// Set the selected drawer item id as the user's id
						store.dispatch(setFriendsPageSelectedDrawerItemId(senderId))
				}
			]
		});
	};

	incomingCallEvent = event => {
		const state = store.getState();
		const channels = state.channels.channels;
		const privateChannels = state.channels.privateChannels;
		const profiles = state.users.profiles;

		const call = event.call;
		const channelId = call.channelId
		const channel = channels[channelId];

		// Set the channel as an active call
		store.dispatch(addIncomingCall(call));

		// If the channel exists
		if (channel) {
			// If the channel is a private channel
			if (channel.isPrivate) {
				// Get the caller information
				const callerId = Object.keys(privateChannels).find(
					key => privateChannels[key] === channelId
				);
				const callerProfile = profiles[callerId];

				// Create the call window
				ipcRenderer.send("create-call-window", {
					id: channelId,
					nickname: callerProfile.nickname,
					avatar:
						"https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
				});
			}
		}
	};

	voiceStateEvent = event => {
		store.dispatch(updateVoiceState(event.userId, event.voiceState));
	};

	voiceActivationEvent = event => {
		Object.keys(event.changedActivity).map(userId =>
			store.dispatch(
				updateVoiceActivationState(userId, event.changedActivity[userId])
			)
		);
	};

	callEndedEvent = event => {
		// Remove the call as an active call
		store.dispatch(removeActiveCall(event.channelId));

		// Close the call window in case there is one
		ipcRenderer.send("close-call-window", event.channelId);
	};

	serverDisconnectEvent = _ => {
		// Set the client as disconnected
		store.dispatch(disconnected());
	}
}

export default new EventHandler();
