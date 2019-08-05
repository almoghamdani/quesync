import React, { Component } from "react";

import anime from "animejs";

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

			anime.remove(this.avatar.current);
		}
		// If need to call but no animating
		else if (this.props.calling && !this.state.callingAnimation) {
			this.setState({
				callingAnimation: true
			});

			// Add an animation for the user activity calling
			UserVoiceActivity.callingAnimationTimeline.add(
				{
					targets: this.avatar.current,
					height: ["20px", "28px"],
					width: ["20px", "28px"],
					borderRadius: ["10px", "14px"]
				},
				0
			);
		}
	}

	render() {
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
			</div>
		);
	}
}

export default UserVoiceActivity;
