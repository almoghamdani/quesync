import React, { Component } from "react";
import { connect } from "react-redux";

import DrawerPage from "../components/page_layouts/drawer_page";

import { ListItem } from "@rmwc/list";

class FriendsPage extends Component {
	render() {
		const friends = this.props.user.friends.map(
			friendId => this.props.profiles[friendId].nickname
		);

		return (
			<DrawerPage
				className="quesync-friends-page"
				drawerContent={friends.map((friend, idx) => (
					<ListItem key={idx}>{friend}</ListItem>
				))}
			/>
		);
	}
}

export default connect(state => ({
	user: state.auth.user,
	profiles: state.users.profiles
}))(FriendsPage);
