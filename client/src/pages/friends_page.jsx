import React, { Component } from "react";
import { connect } from "react-redux";

import DrawerPage from "../components/page_layouts/drawer_page";
import DrawerItem from "../components/drawer_item";

import FriendRequestItem from "../components/friend_request_item";

import { setFriendsPageSelectedTab } from "../actions/tabsActions";

import "./friends_page.scss";

class FriendsPage extends Component {
	friendItemClicked = friendIdx => {
		console.log(friendIdx);
	};

	render() {
		const friends = this.props.user.friends
			? this.props.user.friends.map(
					friendId => this.props.profiles[friendId].nickname
			  )
			: [];

		const pendingFriends = this.props.user.friendRequests
			? this.props.user.friendRequests
					.map(({ friendId, sentAt }) => ({
						nickname: this.props.profiles[friendId].nickname,
						sentAt
					}))
					.sort((a, b) => b.sentAt - a.sentAt)
			: [];

		return (
			<DrawerPage
				className="quesync-friends-page"
				drawerTabs={["All", "Pending"]}
				selectedDrawerTab={this.props.selectedTab}
				drawerTabSelected={tabIdx =>
					this.props.dispatch(setFriendsPageSelectedTab(tabIdx))
				}
				drawerContent={[
					friends.map((friend, idx) => (
						<DrawerItem
							key={idx}
							avatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
							itemName={friend}
						/>
					)),
					pendingFriends.map((friend, idx) => (
						<FriendRequestItem
							key={idx}
							friendAvatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
							friendName={friend["nickname"]}
							sentAt={friend["sentAt"]}
						/>
					))
				]}
				tableWidth="14rem"
				drawerItemClicked={this.friendItemClicked}
				badgeBGColor="red"
				badgeColor="white"
				drawerTabsBadges={[
					this.props.allFriendsBadge,
					this.props.pendingFriendsBadge
				]}
			/>
		);
	}
}

export default connect(state => ({
	user: state.auth.user,
	profiles: state.users.profiles,
	selectedTab: state.ui.tabs.selectedFriendsPageTab,
	allFriendsBadge: state.ui.badges.allFriendsBadge,
	pendingFriendsBadge: state.ui.badges.pendingFriendsBadge
}))(FriendsPage);
