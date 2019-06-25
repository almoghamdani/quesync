import React, { Component } from "react";
import { connect } from "react-redux";

import DrawerPage from "../components/page_layouts/drawer_page";
import DrawerItem from "../components/drawer_item";
import FriendRequestItem from "../components/friend_request_item";
import TextChannel from "../components/text_channel";

import {
	setFriendsPageSelectedTab,
	setFriendsPageSelectedDrawerItem
} from "../actions/itemsActions";

import {
	approveFriendRequest,
	rejectFriendRequest
} from "../actions/userActions";

import "./friends_page.scss";

class FriendsPage extends Component {
	getPrivateChannelId = friendId => this.props.privateChannels[friendId];
	getSelectedFriendId = (friends, pendingFriends) => {
		// Friends selected
		if (this.props.selectedTab === 0) {
			return friends[this.props.selectedDrawerItem].id;
		} else {
			return pendingFriends[this.props.selectedDrawerItem].id;
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

		const currentSelectedFriendId =
			this.props.selectedDrawerItem !== -1
				? this.getSelectedFriendId(friends, pendingFriends)
				: null;

		return (
			<DrawerPage
				className="quesync-friends-page"
				drawerTabs={["All", "Requests"]}
				selectedDrawerTab={this.props.selectedTab}
				drawerTabSelected={tabIdx => {
					this.props.dispatch(setFriendsPageSelectedTab(tabIdx));

					// Reset the choice
					this.props.dispatch(setFriendsPageSelectedDrawerItem(-1));
				}}
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
					this.props.dispatch(setFriendsPageSelectedDrawerItem(friendIdx))
				}
				badgeBGColor="red"
				badgeColor="white"
				drawerTabsBadges={[
					this.props.allFriendsBadge,
					this.props.pendingFriendsBadge
				]}
			>
				{this.props.selectedDrawerItem !== -1 ? (
					<TextChannel
						channelId={this.getPrivateChannelId(currentSelectedFriendId)}
					/>
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
	selectedDrawerItem: state.ui.items.selectedFriendsPageDrawerItem,
	allFriendsBadge: state.ui.badges.allFriendsBadge,
	pendingFriendsBadge: state.ui.badges.pendingFriendsBadge,
	privateChannels: state.channels.privateChannels
}))(FriendsPage);
