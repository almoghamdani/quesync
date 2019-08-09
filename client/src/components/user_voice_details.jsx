import React, { Component } from "react";

import { Typography } from "@rmwc/typography";

import UserVoiceActivity from "./user_voice_activity";

import "./user_voice_details.scss";

class UserVoiceDetails extends Component {
	render() {
		return (
			<div className="quesync-user-voice-details" key={this.props.key}>
				<UserVoiceActivity
					calling={this.props.calling}
					talking={this.props.talking}
					avatar={this.props.avatar}
				/>
				<Typography className="quesync-user-voice-details-nickname" use="body1">
					{this.props.nickname}
				</Typography>
			</div>
		);
	}
}

export default UserVoiceDetails;
