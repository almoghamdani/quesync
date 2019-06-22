import React, { Component } from "react";

import Moment from "react-moment";

import { IconButton } from "@rmwc/icon-button";

import DrawerItem from "./drawer_item";

import "./friend_request_item.scss";

class FriendRequestItem extends Component {
	render() {
		return (
			<DrawerItem
				className="quesync-friend-request-item"
				key={this.props.key}
				onClick={this.props.onClick}
				avatarUrl={this.props.friendAvatarUrl}
				itemName={this.props.friendName}
				itemInfo={
					<Moment unix fromNow interval={10000} date={this.props.sentAt} />
				}
			>
				<div className="quesync-friend-request-buttons-container">
					<IconButton
						className="quesync-friend-request-button quesync-friend-request-reject-button"
						icon="close"
						onClick={event => {
							// Don't call parent click event handler
							event.stopPropagation();

							this.props.rejectRequest();
						}}
					/>
					<IconButton
						className="quesync-friend-request-button quesync-friend-request-approve-button"
						icon="done"
						onClick={event => {
							// Don't call parent click event handler
							event.stopPropagation();
							
							this.props.approveRequest();
						}}
					/>
				</div>
			</DrawerItem>
		);
	}
}

export default FriendRequestItem;
