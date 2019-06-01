import React, { Component } from "react";

import Badge from "./badge";
import { Tab } from "@rmwc/tabs";

class TabBadge extends Component {
	render() {
		return (
			<div className="quesync-tab-badge" style={{ position: "relative" }}>
				<Badge
					style={{ position: "absolute", top: "7px", right: "7px" }}
					value={this.props.value}
					bgColor={this.props.badgeBGColor}
					color={this.props.badgeColor}
				/>
				<Tab {...this.props}>{this.props.children}</Tab>
			</div>
		);
	}
}

export default TabBadge;
