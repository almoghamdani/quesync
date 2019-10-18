import React, { Component } from "react";

import App from "./app";
import StartPage from "./start_page";

import { IconButton } from "@rmwc/icon-button";

import FadeTransition from "./components/fade_transition";

import "./layout.scss";

const os = window.require("os");
const electron = window.require("electron");
const BrowserWindow = electron.remote.BrowserWindow;

class Layout extends Component {
	state = {
		inStartPage: true,
		logout: false
	};

	close() {
		// Blur active element
		document.activeElement.blur();

		// Close the current window
		var currentWindow = BrowserWindow.getFocusedWindow();
		currentWindow.close();
	}

	maximize() {
		// Blur active element
		document.activeElement.blur();

		// Maximize or unmaximize the current window by the current maximized state
		var currentWindow = BrowserWindow.getFocusedWindow();
		currentWindow.isMaximized()
			? currentWindow.unmaximize()
			: currentWindow.maximize();
	}

	minimize() {
		// Blur active element
		document.activeElement.blur();

		// Minimize the current window
		var currentWindow = BrowserWindow.getFocusedWindow();
		currentWindow.minimize();
	}

	render() {
		const isMacOS = os.type() === "Darwin";

		return (
			<div className="quesync-layout">
				<div className="quesync-drag-holder">
					<div className="quesync-drag-zone" />
					<IconButton
						className="quesync-drag-zone-button"
						icon="minimize"
						onClick={this.minimize}
						disabled={isMacOS}
						style={{ opacity: isMacOS ? 0 : 1 }}
					/>
					<IconButton
						className="quesync-drag-zone-button"
						icon="fullscreen"
						onClick={this.maximize}
						disabled={isMacOS}
						style={{ opacity: isMacOS ? 0 : 1 }}
					/>
					<IconButton
						className="quesync-drag-zone-button"
						icon="close"
						onClick={this.close}
						disabled={isMacOS}
						style={{ opacity: isMacOS ? 0 : 1 }}
					/>
				</div>
				<FadeTransition in={this.state.inStartPage} timeout={800} unmountOnExit>
					<StartPage
						logout={this.state.logout}
						easing="easeInOutCirc"
						transitionToApp={() =>
							this.setState({
								inStartPage: false,
								logout: false
							})
						}
					/>
				</FadeTransition>
				<FadeTransition
					in={!this.state.inStartPage}
					timeout={800}
					easing="easeInOutCirc"
					unmountOnExit
				>
					<App
						logout={() =>
							this.setState({
								inStartPage: true,
								logout: true
							})
						}
					/>
				</FadeTransition>
			</div>
		);
	}
}

export default Layout;
