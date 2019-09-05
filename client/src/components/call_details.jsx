import React, { Component } from "react";

import { Typography } from "@rmwc/typography";
import { Icon } from "@rmwc/icon";

import Moment from "react-moment";
import moment from "moment";

import "./call_details.scss";

class CallDetails extends Component {
	render() {
		const duration = moment.duration(
			this.props.endDate - this.props.startDate,
			"seconds"
		);

		return (
			<div className="quesync-call-details">
				<Icon
					className="quesync-call-details-icon"
					icon={
						this.props.missedCall && this.props.endDate
							? "call_missed"
							: "call_made"
					}
					style={{
						color:
							this.props.missedCall && this.props.endDate
								? "#ff1744"
								: "#00e676"
					}}
				/>
				<div className="quesync-call-details-text-wrapper">
					<Typography className="quesync-call-details-text" use="body2">
						{this.props.missedCall && this.props.endDate ? (
							<span>
								{"You missed a call from "}
								<span className="quesync-call-details-caller-name">
									{this.props.callerName}
								</span>
								{". "}
							</span>
						) : (
							<span>
								<span className="quesync-call-details-caller-name">
									{this.props.callerName}
								</span>
								{" started a call. "}
							</span>
						)}
						{this.props.endDate
							? "Call Duration: " + duration.format() + ". "
							: null}
					</Typography>
					<Typography
						className="quesync-call-details-date"
						use="caption"
						style={{ fontSize: "0.68rem" }}
					>
						<Moment
							unix
							calendar
							interval={10000}
							date={this.props.startDate}
						/>
					</Typography>
				</div>
			</div>
		);
	}
}

export default CallDetails;
