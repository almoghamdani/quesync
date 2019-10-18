import React, { Component } from "react";

import anime from "animejs";
import Transition from "react-transition-group/Transition";

const easing = "easeInSine";

class FadeTransition extends Component {
	

	render() {
		const animateIn = content =>
			anime({
				targets: content,
				easing: this.props.easing ? this.props.easing : easing,
				duration: this.props.timeout,
				opacity: [0, 1]
			});

		const aminateOut = content =>
			anime({
				targets: content,
				easing: this.props.easing ? this.props.easing : easing,
				duration: this.props.timeout,
				opacity: 0
			});

		return (
			<Transition
				onEnter={animateIn}
				onExit={aminateOut}
				{...this.props}
			>
				{this.props.children}
			</Transition>
		);
	}
}

export default FadeTransition;
