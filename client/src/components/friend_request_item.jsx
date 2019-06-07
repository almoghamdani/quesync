import React, { Component } from "react";

import moment from "moment";

import DrawerItem from "./drawer_item";

class FriendRequestItem extends Component {
	render() {
		return (
			<DrawerItem
				key={this.props.key}
				avatarUrl={this.props.friendAvatarUrl}
				itemName={this.props.friendName}
				itemInfo={"Sent " + moment(this.props.sentAt, "X").fromNow()}
			/>
		);
	}
}

export default FriendRequestItem;
