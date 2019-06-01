import React, { Component } from "react";

import Badge from "./badge";
import { Tab } from "@rmwc/tabs";

class TabBadge extends Component {
	render() {
		const { badgeBGColor, badgeColor, ...propsWithBadgeProps } = this.props;

		return (
			<div className="quesync-tab-badge" style={{ position: "relative" }}>
				<Badge
					style={{ position: "absolute", top: "7px", right: "7px" }}
					value={this.props.value}
					bgColor={badgeBGColor}
					color={badgeColor}
				/>
				<Tab {...propsWithBadgeProps}>{this.props.children}</Tab>
			</div>
		);
	}
}

export default TabBadge;
