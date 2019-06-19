import React, { Component } from "react";

import Moment from "react-moment";

import { Avatar } from "@rmwc/avatar";
import { Typography } from "@rmwc/typography";
import { Elevation } from "@rmwc/elevation";

import "./text_message.scss";

class TextMessage extends Component {
	render() {
		return (
			<div className="quesync-text-message">
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
						{this.props.content}
					</Typography>
				</Elevation>
				<div className="quesync-text-message-date">
					<Typography use="caption" style={{ fontSize: "0.68rem" }}>
						<Moment unix calendar interval={10000} date={this.props.sentAt} />
					</Typography>
				</div>
			</div>
		);
	}
}

export default TextMessage;
