import React, { Component } from "react";
import { connect } from "react-redux";

import { Scrollbars } from "react-custom-scrollbars";

import { Drawer, DrawerContent, DrawerAppContent } from "@rmwc/drawer";
import { List, ListItem } from "@rmwc/list";

import "./page.scss";

class FriendsPage extends Component {
	render() {
		const friends = this.props.user.friends.map(
			friendId => this.props.profiles[friendId].nickname
		);

		return (
			<div className="quesync-page quesync-friends-page">
				<Drawer className="quesync-drawer">
					<DrawerContent>
						<List>
							{friends.map((friend, idx) => (
								<ListItem key={idx}>{friend}</ListItem>
							))}
						</List>
					</DrawerContent>
				</Drawer>
				<DrawerAppContent className="quesync-drawer-content">
					<Scrollbars>
						<div className="quesync-app-content" />
					</Scrollbars>
				</DrawerAppContent>
			</div>
		);
	}
}

export default connect(state => ({
	user: state.auth.user,
	profiles: state.users.profiles
}))(FriendsPage);
