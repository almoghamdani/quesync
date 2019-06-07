import React, { Component } from "react";

import { Avatar } from "@rmwc/avatar";
import {
	ListItem,
	ListItemGraphic,
	ListItemText
} from "@rmwc/list";

import "./drawer_item.scss";

class DrawerItem extends Component {
	render() {
		return (
			<ListItem className={this.props.className + " quesync-drawer-list-item"} key={this.props.key}>
				<ListItemGraphic
					icon={
						<Avatar
							className="quesync-drawer-avatar"
							src={this.props.avatarUrl}
						/>
					}
				/>
				<ListItemText>
					<div className="quesync-drawer-name" style={{ marginBottom: this.props.itemInfo ? "-6px" : "0" }}>{this.props.itemName}</div>
					{this.props.itemInfo ? (
						<div className="quesync-drawer-info">
							{this.props.itemInfo}
						</div>
					) : null}
				</ListItemText>
			</ListItem>
		);
	}
}

export default DrawerItem;
