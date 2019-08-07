import React, { Component } from "react";

import { connect } from "react-redux";

import { Elevation } from "@rmwc/elevation";

import anime from "animejs";
import { Transition } from "react-transition-group";

import FriendDetails from "./friend_details";
import CallDetails from "./call_details";
import Seperator from "./seperator";

import { call } from "../actions/voiceActions";

import "./side_panel.scss";

class SidePanel extends Component {
	parseVoiceStates() {
		let userVoiceStates = [];

		// Add the user itself
		userVoiceStates.push({
			id: this.props.user.id,
			nickname: this.props.user.nickname,
			avatar:
				"https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png",
			calling: false
		});

		// For each user
		for (const userId in this.props.voiceStates) {
			// If connected or pending
			if (this.props.voiceStates[userId]) {
				userVoiceStates.push({
					id: userId,
					nickname: this.props.profiles[userId].nickname,
					avatar:
						"https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png",
					calling: this.props.voiceStates[userId] === 2
				});
			}
		}

		return userVoiceStates;
	}

	render() {
		const enterTransition = content =>
			anime({
				targets: content,
				duration: 800,
				easing: "easeInOutCirc",
				minHeight: "50%",
				height: "50%",
				opacity: 1
			});

		return (
			<Elevation z={10} className="quesync-side-panel">
				<FriendDetails
					avatarUrl={this.props.friend.avatarUrl}
					nickname={this.props.friend.nickname}
					tag={this.props.friend.tag}
					style={{ height: "100%" }}
					startCall={() =>
						this.props.dispatch(call(this.props.client, this.props.channelId))
					}
				/>
				<Seperator style={{ opacity: this.props.inCall ? 1 : 0 }} />
				<Transition
					appear
					unmountOnExit
					in={this.props.inCall}
					timeout={800}
					onEnter={enterTransition}>
					<CallDetails
						style={{
							minHeight: 0,
							height: 0,
							opacity: 0,
							pointerEvents: this.props.inCall ? "" : "none"
						}}
						userVoiceStates={this.props.inCall ? this.parseVoiceStates() : []}
					/>
				</Transition>
			</Elevation>
		);
	}
}

export default connect(state => ({
	client: state.client.client,
	user: state.user.user,
	inCall: !!state.voice.channelId,
	voiceChannelId: state.voice.channelId,
	voiceStates: state.voice.voiceStates,
	profiles: state.users.profiles
}))(SidePanel);
