import React, { Component } from "react";

import { connect } from "react-redux";

import { Scrollbars } from "react-custom-scrollbars";
import { TransitionGroup, Transition } from "react-transition-group";

import MessageBubble from "../components/message_bubble";
import MessageField from "../components/message_field";

import {
	sendMessage,
	setNewMessageForChannel
} from "../actions/messagesActions";

import "./text_channel.scss";

class TextChannel extends Component {
	endMessagesDummy = React.createRef();

	componentDidMount = () => {
		this.scrollToBottom(false);
	};

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

	scrollToBottom = smooth =>
		this.endMessagesDummy.current.scrollIntoView({
			behavior: smooth ? "smooth" : "auto"
		});

	render() {
		const messages = this.getGroupedMessages();

		return (
			<div className="quesync-text-channel">
				<div className="quesync-text-messages-wrapper">
					<Scrollbars className="quesync-text-messages">
						<TransitionGroup className="quesync-text-messages-container">
							{messages.map(messageGroup => (
								<Transition unmountOnExit>
									<MessageBubble
										key={messageGroup[0].id}
										sender={this.getSenderNickname(messageGroup[0].senderId)}
										avatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
										messages={messageGroup}
										style={{
											paddingTop: "0.8rem"
										}}
									/>
								</Transition>
							))}
						</TransitionGroup>
						<div
							className="quesync-messages-end-dummy"
							ref={this.endMessagesDummy}
						/>
					</Scrollbars>
				</div>
				<div className="quesync-seperator" />
				<MessageField
					value={
						this.props.newMessages[this.props.channelId]
							? this.props.newMessages[this.props.channelId]
							: ""
					}
					setNewValue={value =>
						this.props.dispatch(
							setNewMessageForChannel(value, this.props.channelId)
						)
					}
					send={() =>
						this.props
							.dispatch(
								sendMessage(
									this.props.client,
									this.props.newMessages[this.props.channelId],
									this.props.channelId
								)
							)
							.then(() => this.scrollToBottom(true))
					}
				/>
			</div>
		);
	}
}

export default connect(state => ({
	client: state.client.client,
	messages: state.messages.messages,
	newMessages: state.messages.newMessages,
	profiles: state.users.profiles
}))(TextChannel);
