import React, { Component } from "react";

import Moment from "react-moment";

import DrawerItem from "./drawer_item";

class FriendRequestItem extends Component {
	render() {
		return (
			<DrawerItem
				className="quesync-friend-request-item"
				key={this.props.key}
				avatarUrl={this.props.friendAvatarUrl}
				itemName={this.props.friendName}
				itemInfo={<Moment unix fromNow interval={10000} date={this.props.sentAt}/>}
			/>
		);
	}
}

export default FriendRequestItem;
