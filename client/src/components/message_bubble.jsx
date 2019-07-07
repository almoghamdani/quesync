import React, { Component } from "react";

import TransitionGroup from "react-transition-group/TransitionGroup";
import FadeTransition from "./fade_transition";

import Moment from "react-moment";

import { Avatar } from "@rmwc/avatar";
import { Typography } from "@rmwc/typography";
import { Elevation } from "@rmwc/elevation";

import Twemoji from "react-twemoji";
import emojiRegex from "emoji-regex";

import "./message_bubble.scss";

class MessageBubble extends Component {
	stringIsEmoji = str => {
		const matches = str.match(emojiRegex());
		const matchedStr = matches ? matches.join("") : "";

		// Remove all blanks and check if matched to matched str
		return str.replace(/\s/g, "") === matchedStr;
	};

	render() {
		return (
			<div className="quesync-message-bubble" style={this.props.style}>
				<Elevation z={7} className="quesync-text-bubble">
					<div className="quesync-text-sender-info">
						<Avatar
							className="quesync-text-avatar"
							src={this.props.avatarUrl}
						/>
						<div>
							<Typography use="subtitle1" style={{ fontSize: "0.86rem" }}>
								{this.props.sender}
							</Typography>
						</div>
					</div>
					<Typography className="quesync-text-content" use="body2">
						<TransitionGroup>
							{this.props.messages.map(message => (
								<FadeTransition timeout={200} key={message.id}>
									<Twemoji
										key={message.id}
										options={{
											className: this.stringIsEmoji(message.content)
												? "quesync-message-bubble-only-emoji"
												: "quesync-message-bubble-text-emoji"
										}}
									>
										{message.content}
									</Twemoji>
								</FadeTransition>
							))}
						</TransitionGroup>
					</Typography>
				</Elevation>
				<div className="quesync-text-message-date">
					<Typography use="caption" style={{ fontSize: "0.68rem" }}>
						<Moment
							unix
							calendar
							interval={10000}
							date={this.props.messages[0].sentAt}
						/>
					</Typography>
				</div>
			</div>
		);
	}
}

export default MessageBubble;
