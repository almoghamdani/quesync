import React, { Component } from "react";

import { Typography } from "@rmwc/typography";
import { Button } from "@rmwc/button";

import "./friend_details.scss";

class FriendDetails extends Component {
	callButton = React.createRef();

	render() {
		return (
			<div className="quesync-friend-details-wrapper" style={this.props.style}>
				<div className="quesync-friend-details">
					<img
						className="quesync-friend-avatar"
						src={this.props.avatar}
						alt={this.props.nickname}
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
					{this.props.friend ? (
						<Button
							className="quesync-remove-friend-button"
							raised
							icon="remove"
							label="Remove Friend"
							onClick={() => this.props.removeFriend()}
						/>
					) : this.props.pendingFriend ? (
						<div className="quesync-friend-request-actions">
							{this.props.pendingFriendType === "recipient" ? (
								<Button
									className="quesync-remove-friend-button"
									raised
									icon="remove"
									label="Cancel Request"
									onClick={() => this.props.rejectFriendRequest()}
								/>
							) : (
								<>
									<Button
										className="quesync-remove-friend-button"
										raised
										icon="remove"
										label="Reject"
										onClick={() => this.props.rejectFriendRequest()}
									/>
									<Button
										className="quesync-add-friend-button"
										raised
										icon="add"
										label="Accept"
										onClick={() => this.props.approveFriendRequest()}
									/>
								</>
							)}
						</div>
					) : (
						<Button
							className="quesync-add-friend-button"
							raised
							icon="add"
							label="Add Friend"
							onClick={() => this.props.sendFriendRequest()}
						/>
					)}
				</div>
			</div>
		);
	}
}

export default FriendDetails;
