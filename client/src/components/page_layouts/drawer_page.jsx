import React, { Component } from "react";

import { Scrollbars } from "react-custom-scrollbars";

import { Drawer, DrawerContent, DrawerAppContent } from "@rmwc/drawer";
import { List } from "@rmwc/list";
import { TabBar, Tab } from "@rmwc/tabs";

import "./drawer_page.scss";

class DrawerPage extends Component {
	render() {
		return (
			<div className="quesync-page">
				<Drawer
					className="quesync-drawer"
					style={{ width: this.props.tableWidth }}>
					<DrawerContent className="quesync-drawer-content">
						{this.props.drawerTabs ? (
							<TabBar
								activeTabIndex={this.props.selectedDrawerTab}
								onActivate={evt =>
									this.props.drawerTabSelected(evt.detail.index)
								}>
								{this.props.drawerTabs.map((tabName, idx) => (
									<Tab className="quesync-drawer-tab" key={idx}>
										{tabName}
									</Tab>
								))}
							</TabBar>
						) : null}
						<List>
							{React.Children.map(
								this.props.drawerTabs
									? this.props.drawerContent[this.props.selectedDrawerTab]
									: this.props.drawerContent,
								child =>
									React.cloneElement(child, {
										onClick: () => this.props.drawerItemClicked(child.key)
									})
							)}
						</List>
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
