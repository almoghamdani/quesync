import React, { Component } from "react";

import { connect } from "react-redux";
import { TransitionGroup } from "react-transition-group";

import MessageBubble from "../components/message_bubble";

class TextChannel extends Component {
	getGroupedMessages = () => {
		const channelMessages = this.props.messages[this.props.channelId];
		let currentSender,
			lastSendDate,
			currentGroupIdx = 0,
			groupedMessages = [];

		// For each messages sort it by groups
		channelMessages.forEach(message => {
			// If the message has the current sender of the last message and is in within an hour from the last message, add it to last group
			if (
				currentSender != null &&
				currentSender === message.senderId &&
				message.sentAt - lastSendDate <= 60 * 60
			) {
				groupedMessages[currentGroupIdx].push(message);

				// Set the last send date
				lastSendDate = message.sentAt;
			} else {
				// Create a new group with the message
				currentGroupIdx = groupedMessages.push([message]) - 1;

				// Set the new current sender
				currentSender = message.senderId;

				// Set the last send date
				lastSendDate = message.sentAt;
			}
		});

		return groupedMessages;
	};

	getSenderNickname = senderId => this.props.profiles[senderId].nickname;

	render() {
		const messages = this.getGroupedMessages();

		return (
			<div className="quesync-text-channel">
				<TransitionGroup className="quesync-text-messages">
					{messages.map(messageGroup => (
						<MessageBubble
							sender={this.getSenderNickname(messageGroup[0].senderId)}
							avatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
							messages={messageGroup}
						/>
					))}
				</TransitionGroup>
			</div>
		);
	}
}

export default connect(state => ({
	messages: state.messages.messages,
	profiles: state.users.profiles
}))(TextChannel);
