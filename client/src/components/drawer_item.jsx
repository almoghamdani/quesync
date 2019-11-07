import React, { Component } from "react";

import { ListItem, ListItemGraphic, ListItemText } from "@rmwc/list";

import "./drawer_item.scss";

class DrawerItem extends Component {
	render() {
		return (
			<ListItem
				className={this.props.className + " quesync-drawer-list-item"}
				key={this.props.key}
				onClick={this.props.onClick}
			>
				<img className="quesync-drawer-avatar" src={this.props.avatar} />
				<ListItemText>
					<div
						className="quesync-drawer-name"
						style={{ marginBottom: this.props.itemInfo ? "-6px" : "0" }}
					>
						{this.props.itemName}
					</div>
					{this.props.itemInfo ? (
						<div className="quesync-drawer-info">{this.props.itemInfo}</div>
					) : null}
				</ListItemText>
				{this.props.children}
			</ListItem>
		);
	}
}

export default DrawerItem;
