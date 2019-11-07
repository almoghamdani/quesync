import React, { Component } from "react";
import { connect } from "react-redux";

import DrawerPage from "../components/page_layouts/drawer_page";
import DrawerItem from "../components/drawer_item";
import FriendRequestItem from "../components/friend_request_item";
import Channel from "../components/channel";
import FriendDetails from "../components/friend_details";

import {
	setFriendsPageSelectedTab,
	setFriendsPageSelectedDrawerItemId
} from "../actions/itemsActions";
import {
	approveFriendRequest,
	rejectFriendRequest,
	removeFriend
} from "../actions/userActions";
import { call, joinCall } from "../actions/voiceActions";

import "./friends_page.scss";

const { ipcRenderer } = window.require("electron");

class FriendsPage extends Component {
	update = true;

	componentDidMount() {
		this.updateSidePanelElement();
	}

	componentDidUpdate() {
		this.updateSidePanelElement();
	}

	shouldComponentUpdate() {
		if (!this.update) {
			this.update = true;

			return false;
		}

		return true;
	}

	updateSidePanelElement = () => {
		const friends = this.props.user.friends;

		const currentSelectedFriendId = this.props.selectedDrawerItemId;

		// If no friend selected
		if (!currentSelectedFriendId) {
			return;
		}

		const currentSelectedFriend = this.props.profiles[
			this.props.selectedDrawerItemId
		];

		const channelId = this.getPrivateChannelId(currentSelectedFriendId);

		// Disable the next update due to the update of the side panel element
		this.update = false;

		this.props.setSidePanelElement(
			<FriendDetails
				avatar={currentSelectedFriend.photo}
				nickname={currentSelectedFriend.nickname}
				tag={currentSelectedFriend.tag}
				pendingFriend={!friends.includes(currentSelectedFriendId)}
				style={{ height: "100%" }}
				startCall={() => this.props.dispatch(call(channelId))}
				joinCall={() => {
					// Close the call window is case it's open
					ipcRenderer.send("close-call-window", channelId);

					// Join the call
					this.props.dispatch(joinCall(channelId));
				}}
				inCall={this.props.voiceChannelId === channelId}
				activeCall={this.props.activeCalls.includes(channelId)}
				removeFriend={() =>
					this.props.dispatch(removeFriend(currentSelectedFriendId))
				}
			/>
		);
	};

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
		this.props.dispatch(approveFriendRequest(friendId)).catch(() => {});
	};

	rejectFriendRequest = friendId => {
		// Try to reject the friend request
		this.props.dispatch(rejectFriendRequest(friendId)).catch(() => {});
	};

	render() {
		const friends = this.props.user.friends
			? this.props.user.friends.map(friendId => ({
					id: friendId,
					nickname: this.props.profiles[friendId].nickname,
					photo: this.props.profiles[friendId].photo
			  }))
			: [];

		const pendingFriends = this.props.user.friendRequests
			? this.props.user.friendRequests
					.map(({ friendId, friendType, sentAt }) => ({
						id: friendId,
						type: friendType,
						nickname: this.props.profiles[friendId].nickname,
						photo: this.props.profiles[friendId].photo,
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
							avatar={friend.photo}
							itemName={friend.nickname}
						/>
					)),
					pendingRequests
						.map(friend => (
							<FriendRequestItem
								key={friend.id}
								friendAvatar={friend.photo}
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
									friendAvatar={friend.photo}
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
				]}
			>
				{currentSelectedFriendId ? (
					<div className="quesync-friend-page">
						<Channel
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
	profiles: state.users.profiles,
	selectedTab: state.ui.items.selectedFriendsPageTab,
	selectedDrawerItemId: state.ui.items.selectedFriendsPageDrawerItemId,
	allFriendsBadge: state.ui.badges.allFriendsBadge,
	pendingFriendsBadge: state.ui.badges.pendingFriendsBadge,
	privateChannels: state.channels.privateChannels,
	voiceChannelId: state.voice.channelId,
	activeCalls: state.voice.activeCalls
}))(FriendsPage);
