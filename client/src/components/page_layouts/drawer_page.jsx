import React, { Component } from "react";

import { Scrollbars } from "react-custom-scrollbars";

import { Drawer, DrawerContent, DrawerAppContent } from "@rmwc/drawer";
import { List } from "@rmwc/list";

import "./drawer_page.scss";

class DrawerPage extends Component {
	render() {
		return (
			<div className="quesync-page">
				<Drawer className="quesync-drawer" style={{ width: this.props.tableWidth }}>
					<DrawerContent className="quesync-drawer-content">
						<List>{this.props.drawerContent}</List>
					</DrawerContent>
				</Drawer>
				<DrawerAppContent className="quesync-app-drawer-content">
					<Scrollbars>
						<div className="quesync-app-content">{this.props.children}</div>
					</Scrollbars>
				</DrawerAppContent>
			</div>
		);
	}
}

export default DrawerPage;
