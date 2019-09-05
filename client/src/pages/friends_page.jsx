import React, { Component } from "react";
import { connect } from "react-redux";

import DrawerPage from "../components/page_layouts/drawer_page";
import DrawerItem from "../components/drawer_item";
import FriendRequestItem from "../components/friend_request_item";
import Channel from "../components/channel";
import SidePanel from "../components/side_panel";

import {
	setFriendsPageSelectedTab,
	setFriendsPageSelectedDrawerItemId
} from "../actions/itemsActions";

import {
	approveFriendRequest,
	rejectFriendRequest
} from "../actions/userActions";

import "./friends_page.scss";

class FriendsPage extends Component {
	getPrivateChannelId = friendId => this.props.privateChannels[friendId];

	getSelectedFriendId = (friends, pendingFriends, friendIdx) => {
		// Friends selected
		if (this.props.selectedTab === 0) {
			return friends[friendIdx].id;
		} else {
			return pendingFriends[friendIdx].id;
		}
	};

	approveFriendRequest = friendId => {
		// Try to approve the friend request
		this.props
			.dispatch(approveFriendRequest(this.props.client, friendId))
			.catch(() => {});
	};

	rejectFriendRequest = friendId => {
		// Try to reject the friend request
		this.props
			.dispatch(rejectFriendRequest(this.props.client, friendId))
			.catch(() => {});
	};

	render() {
		const friends = this.props.user.friends
			? this.props.user.friends.map(friendId => ({
					id: friendId,
					nickname: this.props.profiles[friendId].nickname
			  }))
			: [];

		const pendingFriends = this.props.user.friendRequests
			? this.props.user.friendRequests
					.map(({ friendId, friendType, sentAt }) => ({
						id: friendId,
						type: friendType,
						nickname: this.props.profiles[friendId].nickname,
						sentAt
					}))
					.sort((a, b) => b.sentAt - a.sentAt)
			: [];

		const pendingRequests = pendingFriends.filter(
			friend => friend.type === "requester"
		);
		const pendingApproval = pendingFriends.filter(
			friend => friend.type === "recipient"
		);

		const currentSelectedFriendId = this.props.selectedDrawerItemId;
		const currentSelectedFriend = this.props.profiles[
			this.props.selectedDrawerItemId
		];

		return (
			<DrawerPage
				className="quesync-friends-page"
				drawerTabs={["All", "Requests"]}
				selectedDrawerTab={this.props.selectedTab}
				drawerTabSelected={tabIdx =>
					this.props.dispatch(setFriendsPageSelectedTab(tabIdx))
				}
				drawerNoItemsMessage={[
					"It's time to add some friends to your friends list.\nClick the search field above to get started!",
					"You have no pending friend requests."
				]}
				drawerContent={[
					friends.map(friend => (
						<DrawerItem
							key={friend.id}
							avatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
							itemName={friend.nickname}
						/>
					)),
					pendingRequests
						.map(friend => (
							<FriendRequestItem
								key={friend.id}
								friendAvatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
								friendName={friend.nickname}
								sentAt={friend.sentAt}
								approveRequest={() => this.approveFriendRequest(friend.id)}
								rejectRequest={() => this.rejectFriendRequest(friend.id)}
							/>
						))
						.concat(
							pendingApproval.map(friend => (
								<FriendRequestItem
									key={friend.id}
									friendAvatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
									friendName={friend.nickname}
									sentAt={friend.sentAt}
									approveRequest={() => this.approveFriendRequest(friend.id)}
									rejectRequest={() => this.rejectFriendRequest(friend.id)}
									approval
								/>
							))
						)
				]}
				drawerItemClicked={friendIdx =>
					this.props.dispatch(
						setFriendsPageSelectedDrawerItemId(
							this.getSelectedFriendId(friends, pendingFriends, friendIdx)
						)
					)
				}
				badgeBGColor="red"
				badgeColor="white"
				drawerTabsBadges={[
					this.props.allFriendsBadge,
					this.props.pendingFriendsBadge
				]}>
				{currentSelectedFriendId ? (
					<div className="quesync-friend-page">
						<Channel
							channelId={this.getPrivateChannelId(currentSelectedFriendId)}
						/>
						<SidePanel
							friend={{
								nickname: currentSelectedFriend.nickname,
								tag: currentSelectedFriend.tag,
								avatarUrl:
									"https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
							}}
							channelId={this.getPrivateChannelId(currentSelectedFriendId)}
						/>
					</div>
				) : null}
			</DrawerPage>
		);
	}
}

export default connect(state => ({
	user: state.user.user,
	client: state.client.client,
	profiles: state.users.profiles,
	selectedTab: state.ui.items.selectedFriendsPageTab,
	selectedDrawerItemId: state.ui.items.selectedFriendsPageDrawerItemId,
	allFriendsBadge: state.ui.badges.allFriendsBadge,
	pendingFriendsBadge: state.ui.badges.pendingFriendsBadge,
	privateChannels: state.channels.privateChannels
}))(FriendsPage);
