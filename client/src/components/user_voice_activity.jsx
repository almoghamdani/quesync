import React, { Component } from "react";

import anime from "animejs";
import Transition from "react-transition-group/Transition";

import { Icon } from "@rmwc/icon";

import "./user_voice_activity.scss";

class UserVoiceActivity extends Component {
	static callingAnimationTimeline = anime.timeline({
		duration: 400,
		easing: "easeOutElastic(1.3, 0.7)",
		loop: true,
		direction: "alternate"
	});

	state = {
		callingAnimation: false
	};

	avatar = React.createRef();

	componentDidUpdate() {
		// If we need to stop calling, stop the animation
		if (!this.props.calling && this.state.callingAnimation) {
			this.setState({
				callingAnimation: false
			});

			// Stop animating the avatar
			anime.remove(this.avatar.current);

			// Remove all the animation css properties and set animation for the avatar to return to it's place
			this.avatar.current.style = "transition: all 0.15s ease-in-out;";
		}
		// If need to call but no animating
		else if (this.props.calling && !this.state.callingAnimation) {
			this.setState({
				callingAnimation: true
			});

			// Clear the current style of the avatar
			this.avatar.current.style = "";

			// Add an animation for the user activity calling
			UserVoiceActivity.callingAnimationTimeline.add(
				{
					targets: this.avatar.current,
					height: ["26px", "34px"],
					width: ["26px", "34px"],
					borderRadius: ["13px", "17px"]
				},
				0
			);
		}
	}

	render() {
		const voiceStateEnterAnimation = content =>
			anime({
				targets: content,
				easing: "easeInOutQuint",
				duration: 150,
				minHeight: ["0px", "14px"],
				minWidth: ["0px", "14px"],
				borderRadius: ["0px", "7px"],
				fontSize: ["0px", "12px"]
			});

		const voiceStateExitAnimation = content =>
			anime({
				targets: content,
				easing: "easeInOutQuint",
				duration: 150,
				minHeight: ["14px", "0px"],
				minWidth: ["14px", "0px"],
				borderRadius: ["7px", "0px"],
				fontSize: ["12px", "0px"]
			});

		return (
			<div
				className={
					this.props.talking
						? "quesync-user-voice-activity quesync-user-voice-activity-talking"
						: "quesync-user-voice-activity"
				}>
				<img
					className="quesync-user-voice-activity-avatar"
					ref={this.avatar}
					src={this.props.avatar}
					alt="User Voice Activity"
				/>
				<div className="quesync-voice-state-icon-wrapper quesync-voice-state-volume-icon">
					<Transition
						appear
						in={this.props.deafen}
						onEnter={voiceStateEnterAnimation}
						onExit={voiceStateExitAnimation}
						timeout={150}>
						<Icon className="quesync-voice-state-icon" icon="volume_off" />
					</Transition>
				</div>
				<div className="quesync-voice-state-icon-wrapper quesync-voice-state-mic-icon">
					<Transition
						appear
						in={this.props.muted}
						onEnter={voiceStateEnterAnimation}
						onExit={voiceStateExitAnimation}
						timeout={150}>
						<Icon className="quesync-voice-state-icon" icon="mic_off" />
					</Transition>
				</div>
			</div>
		);
	}
}

export default UserVoiceActivity;
