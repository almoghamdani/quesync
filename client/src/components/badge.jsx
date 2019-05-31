import React, { Component } from "react";

import { Typography } from "@rmwc/typography";

class Badge extends Component {
	render() {
		return (
			<span
				style={{
					width: "20px",
					height: "20px",
					borderRadius: "10px",
					backgroundColor: this.props.bgColor,
					color: this.props.color,
					textAlign: "center"
				}}>
				<Typography use="caption">{this.props.value}</Typography>
			</span>
		);
	}
}

export default Badge;
