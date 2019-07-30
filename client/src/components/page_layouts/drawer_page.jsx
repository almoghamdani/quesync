import React, { Component } from "react";

import DrawerScrollbars from "../drawer_scrollbars";

import { Drawer, DrawerContent, DrawerAppContent } from "@rmwc/drawer";
import { List, ListItem } from "@rmwc/list";
import { TabBar } from "@rmwc/tabs";
import TabBadge from "../tab_badge";

import TransitionGroup from "react-transition-group/TransitionGroup";
import FadeTransition from "../fade_transition";

import "./drawer_page.scss";

function NoItemsMessage({ message }) {
	return (
		<ListItem className="quesync-no-items-list-item" disabled>
			{message}
		</ListItem>
	);
}

class DrawerPage extends Component {
	constructor(props) {
		super(props);

		this.state = {
			selectedDrawerTab: props.selectedDrawerTab
		};
	}

	render() {
		const animationEndCallback = () =>
			this.setState({ selectedDrawerTab: this.props.selectedDrawerTab });

		return (
			<div className={"quesync-page " + this.props.className}>
				<Drawer className="quesync-drawer">
					<DrawerContent className="quesync-drawer-content">
						{this.props.drawerTabs ? (
							<TabBar
								activeTabIndex={this.props.selectedDrawerTab}
								onActivate={evt =>
									this.props.drawerTabSelected(evt.detail.index)
								}
							>
								{this.props.drawerTabs.map((tabName, idx) => (
									<TabBadge
										className="quesync-drawer-tab"
										key={idx}
										badgeBGColor={this.props.badgeBGColor}
										badgeColor={this.props.badgeColor}
										value={this.props.drawerTabsBadges[idx]}
										style={{ width: "50%" }}
									>
										{tabName}
									</TabBadge>
								))}
							</TabBar>
						) : null}
						<DrawerScrollbars style={{ marginTop: "2px" }}>
							<List className="quesync-drawer-list">
								{this.props.drawerContent.map((drawerTabContent, idx) => (
									<FadeTransition
										appear
										unmountOnExit
										in={
											this.state.selectedDrawerTab ===
												this.props.selectedDrawerTab &&
											this.state.selectedDrawerTab === idx
										}
										onExited={animationEndCallback}
										timeout={115}
										key={idx}
									>
										{drawerTabContent.length > 0 ? (
											<TransitionGroup>
												{React.Children.map(
													drawerTabContent,
													(child, childIdx) => (
														<FadeTransition timeout={115} key={child.key}>
															<div
																key={child.key}
																className="quesync-drawer-content-item-wrapper"
															>
																{React.cloneElement(child, {
																	onClick: () =>
																		this.props.drawerItemClicked(childIdx)
																})}
															</div>
														</FadeTransition>
													)
												)}
											</TransitionGroup>
										) : (
											<NoItemsMessage
												message={this.props.drawerNoItemsMessage[idx]}
											/>
										)}
									</FadeTransition>
								))}
							</List>
						</DrawerScrollbars>
					</DrawerContent>
				</Drawer>
				<DrawerAppContent className="quesync-app-drawer-content">
					<div className="quesync-app-content">{this.props.children}</div>
				</DrawerAppContent>
			</div>
		);
	}
}

export default DrawerPage;
