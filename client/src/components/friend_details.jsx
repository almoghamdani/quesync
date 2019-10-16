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
						className={
							this.props.inCall
								? "quesync-friend-call-button quesync-friend-in-call-button"
								: this.props.activeCall
								? "quesync-friend-call-button quesync-friend-join-button"
								: "quesync-friend-call-button"
						}
						raised
						ref={this.callButton}
						icon="call"
						label={
							this.props.inCall
								? "In Call"
								: this.props.activeCall
								? "Join Call"
								: "Call"
						}
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