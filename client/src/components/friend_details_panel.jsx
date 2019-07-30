import React, { Component } from "react";

import { Elevation } from "@rmwc/elevation";
import { Typography } from "@rmwc/typography";
import { Avatar } from "@rmwc/avatar";

import "./friend_details_panel.scss";

class FriendDetailsPanel extends Component {
	render() {
		return (
			<Elevation z={10} className="quesync-friend-details-panel">
				<Avatar className="quesync-friend-avatar" src={this.props.avatarUrl} />
				<Typography className="quesync-friend-nickname-tag" use="headline6">
					<span className="quesync-friend-nickname">{this.props.nickname}</span>
					<br />
					<span className="quesync-friend-tag">#{this.props.tag}</span>
				</Typography>
			</Elevation>
		);
	}
}

export default FriendDetailsPanel;
