import React, { Component } from "react";

import { Typography } from "@rmwc/typography";

class Badge extends Component {
	render() {
		return (
			<span
				style={{
					...this.props.style,
					width: "16px",
					height: "16px",
					borderRadius: "8px",
					backgroundColor: this.props.bgColor,
					color: this.props.color,
					display: "flex",
					alignItems: "center",
					justifyContent: "center"
				}}>
				<Typography use="caption" style={{ fontSize: "10px" }}>{this.props.value}</Typography>
			</span>
		);
	}
}

export default Badge;
