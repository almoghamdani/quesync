import React, { Component } from "react";

import { Scrollbars } from "react-custom-scrollbars";
import { TransitionGroup } from "react-transition-group";

import { Typography } from "@rmwc/typography";
import { List } from "@rmwc/list";
import { Button } from "@rmwc/button";

import FadeTransition from "./fade_transition";

import UserVoiceDetails from "./user_voice_details";

import "./current_call_details.scss";

class CurrentCallDetails extends Component {
	render() {
		return (
			<div className="quesync-call-details-wrapper" style={this.props.style}>
				<div className="quesync-call-participants-wrapper">
					<Typography
						use="subtitle1"
						style={{ color: "var(--mdc-theme-primary)", marginBottom: "5px" }}>
						Currently in Call:
					</Typography>
					<Scrollbars autoHide>
						<List className="quesync-call-participants">
							<TransitionGroup>
								{this.props.userVoiceStates.map(userVoiceState => (
									<FadeTransition timeout={200} key={userVoiceState.id}>
										<UserVoiceDetails
											key={userVoiceState.id}
											talking={this.props.userVoiceActivations[userVoiceState.id]}
											{...userVoiceState}
										/>
									</FadeTransition>
								))}
							</TransitionGroup>
						</List>
					</Scrollbars>
				</div>
				<div className="quesync-call-actions">
					<Button
						className="quesync-leave-call-button"
						raised
						danger
						icon="call_end"
						label="Leave Call"
						onClick={this.props.leaveCall}
					/>
				</div>
			</div>
		);
	}
}

export default CurrentCallDetails;
