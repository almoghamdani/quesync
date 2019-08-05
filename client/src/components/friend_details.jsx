import React, { Component } from "react";

import { Typography } from "@rmwc/typography";
import { Avatar } from "@rmwc/avatar";
import { IconButton } from "@rmwc/icon-button";

import "./friend_details.scss";

class FriendDetails extends Component {
	render() {
		return (
			<div className="quesync-friend-details-wrapper" style={this.props.style}>
				<div className="quesync-friend-details">
					<Avatar
						className="quesync-friend-avatar"
						src={this.props.avatarUrl}
					/>
					<Typography className="quesync-friend-nickname-tag" use="headline6">
						<span className="quesync-friend-nickname">
							{this.props.nickname}
						</span>
						<br />
						<span className="quesync-friend-tag">#{this.props.tag}</span>
					</Typography>
				</div>
				<div className="quesync-friend-actions">
					<IconButton
						icon="call"
						label="Call"
						style={{ color: "var(--mdc-theme-primary)" }}
						onClick={() => this.props.startCall()}
					/>
				</div>
			</div>
		);
	}
}

export default FriendDetails;
