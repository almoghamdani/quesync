import React, { Component } from "react";
import { connect } from "react-redux";

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
		inStartPage: true
	};

	static getDerivedStateFromProps(props, state) {
		// If the client isn't connected, return to the start page
		if (!props.connected) {
			return { ...state, inStartPage: true };
		}

		return state;
	}

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
				<FadeTransition
					in={this.state.inStartPage}
					timeout={300}
					easing="easeInOutCirc"
					unmountOnExit
				>
					<StartPage
						transitionToApp={() =>
							this.setState({
								inStartPage: false
							})
						}
					/>
				</FadeTransition>
				<FadeTransition
					in={!this.state.inStartPage}
					timeout={300}
					easing="easeInOutCirc"
					unmountOnExit
				>
					<App
						transitionToStartPage={() =>
							this.setState({
								inStartPage: true
							})
						}
					/>
				</FadeTransition>
			</div>
		);
	}
}

export default connect(state => ({
	connected: state.client.connected
}))(Layout);
