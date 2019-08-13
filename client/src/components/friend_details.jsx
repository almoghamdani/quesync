import React, { Component } from "react";

import { Typography } from "@rmwc/typography";
import { Avatar } from "@rmwc/avatar";
import { Button } from "@rmwc/button";

import "./friend_details.scss";

class FriendDetails extends Component {
	callButton = React.createRef();

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
					<Button
						className="quesync-friend-call-button"
						ref={this.callButton}
						icon="call"
						label={
							this.props.inCall
								? "In Call"
								: this.props.activeCall
								? "Join Call"
								: "Call"
						}
						style={{
							color: this.props.inCall ? "gray" : "var(--mdc-theme-primary)"
						}}
						onClick={() => {
							// Call the correct handler
							this.props.activeCall
								? this.props.joinCall()
								: this.props.startCall();

							// Blur the button
							this.callButton.current.blur();
						}}
						disabled={this.props.inCall}
					/>
				</div>
			</div>
		);
	}
}

export default FriendDetails;
