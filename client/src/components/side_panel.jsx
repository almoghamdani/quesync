import React, { Component } from "react";

import { Elevation } from "@rmwc/elevation";

import anime from "animejs";
import { Transition } from "react-transition-group";

import FriendDetails from "./friend_details";
import CallDetails from "./call_details";
import Seperator from "./seperator";

import "./side_panel.scss";

class SidePanel extends Component {
	state = {
		inCall: false
	};

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
						this.setState({
							inCall: true
						})
					}
				/>
				<Seperator style={{ opacity: this.state.inCall ? 1 : 0 }} />
				<Transition
					appear
					unmountOnExit
					in={this.state.inCall}
					timeout={800}
					onEnter={enterTransition}>
					<CallDetails
						style={{
							minHeight: 0,
							height: 0,
							opacity: 0,
							pointerEvents: this.state.inCall ? "" : "none"
						}}
					/>
				</Transition>
			</Elevation>
		);
	}
}

export default SidePanel;
