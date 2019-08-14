import React, { Component } from "react";

import zxcvbn from "zxcvbn";

import "./password_meter.scss";
import { Typography } from "@rmwc/typography";

class PasswordMeter extends Component {
	classNameForScore = score => {
		switch (score) {
			case 1:
			case 2:
				return "quesync-password-meter-progress-weak";

			case 3:
				return "quesync-password-meter-progress-fair";

			case 4:
				return "quesync-password-meter-progress-good";

			case 5:
				return "quesync-password-meter-progress-strong";

			default:
				return "quesync-password-meter-progress-weak";
		}
	};

	descriptionForScore = score => {
		switch (score) {
			case 1:
			case 2:
				return "Weak Password";

			case 3:
				return "Fair Password";

			case 4:
				return "Good Password";

			case 5:
				return "Strong Password";

			default:
				return "No Password Entered";
		}
	};

	render() {
		const testScore = this.props.password
			? zxcvbn(this.props.password).score + 1
			: 0;

		return (
			<div className="quesync-password-meter">
				<progress
					className={
						"quesync-password-meter-progress " +
						this.classNameForScore(testScore)
					}
					value={testScore}
					max={5}
				/>
				<Typography use="caption">
					{this.descriptionForScore(testScore)}
				</Typography>
			</div>
		);
	}
}

export default PasswordMeter;
