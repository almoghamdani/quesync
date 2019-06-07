import React, { Component } from "react";

import { Scrollbars } from "react-custom-scrollbars";

import { Drawer, DrawerContent, DrawerAppContent } from "@rmwc/drawer";
import { List } from "@rmwc/list";
import { TabBar } from "@rmwc/tabs";
import TabBadge from "../tab_badge";

import anime from "animejs";

import TransitionGroup from "react-transition-group/TransitionGroup";
import Transition from "react-transition-group/Transition";

import "./drawer_page.scss";

class DrawerPage extends Component {
	constructor(props) {
		super(props);

		this.state = {
			selectedDrawerTab: props.selectedDrawerTab
		};
	}

	render() {
		const animateDrawerContentIn = drawerContent =>
			anime({
				targets: drawerContent,
				easing: "easeInSine",
				duration: "150",
				opacity: "1"
			});

		const animateDrawerContentOut = drawerContent =>
			anime({
				targets: drawerContent,
				easing: "easeOutSine",
				duration: "150",
				opacity: "0"
			});

		const animationEndCallback = () =>
			this.setState({ selectedDrawerTab: this.props.selectedDrawerTab });

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
									<TabBadge
										className="quesync-drawer-tab"
										key={idx}
										badgeBGColor={this.props.badgeBGColor}
										badgeColor={this.props.badgeColor}
										value={this.props.drawerTabsBadges[idx]}>
										{tabName}
									</TabBadge>
								))}
							</TabBar>
						) : null}
						<List>
							{this.props.drawerContent.map((drawerTabContent, idx) => (
								<Transition
									appear
									unmountOnExit
									in={
										this.state.selectedDrawerTab ===
											this.props.selectedDrawerTab &&
										this.state.selectedDrawerTab === idx
									}
									onEnter={animateDrawerContentIn}
									onExit={animateDrawerContentOut}
									onExited={animationEndCallback}
									timeout={150}
									key={idx}>
									<TransitionGroup style={{ opacity: "0" }}>
										{React.Children.map(drawerTabContent, child => (
											<Transition
												appear
												onEnter={animateDrawerContentIn}
												onExit={animateDrawerContentOut}
												timeout={150}
												key={child.key}>
												<div
													key={child.key}
													className="quesync-drawer-content-item-wrapper"
													style={{ opacity: 0 }}>
													{React.cloneElement(child, {
														onClick: () =>
															this.props.drawerItemClicked(child.key)
													})}
												</div>
											</Transition>
										))}
									</TransitionGroup>
								</Transition>
							))}
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
