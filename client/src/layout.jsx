import React, { Component } from "react";
import ReactDOM from 'react-dom'

import "./layout.scss";

import { IconButton } from "@rmwc/icon-button";
import anime from "animejs/lib/anime.es.js";

const electron = window.require("electron");
const BrowserWindow = electron.remote.BrowserWindow;

class Layout extends Component {
	state = {
		child: null,
		newChild: null
	};

	constructor(props) {
		super(props);

		// Create ref to the current child
		this.childRef = React.createRef();

		// Add the animateTo prop to the child
		this.state.child = React.cloneElement(props.child, {
			animateTo: this.animateToNewChild,
			ref: this.childRef
		});
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

	animateToNewChild = child => {
		var newChildRef = React.createRef();

		// Set opacity as 0
		child = React.cloneElement(child, { style: { opacity: 0 }, ref: newChildRef });

		// Render the new child
		this.setState({
			newChild: child
		});

		// Create timeline for the fade animation
		var timeline = anime.timeline({
			duration: 800,
			easing: "easeInOutCirc",
			delay: 250,
			complete: () => {
				// Re-clone the element with the new ref and opacity 1
				child = React.cloneElement(child, { style: { opacity: 1 }, ref: this.childRef });

				// Remove the current child and add the new child
				this.setState({
					child: child,
					newChild: null
				})
			}
		})

		// Fade out the current child
		timeline.add({
			targets: ReactDOM.findDOMNode(this.childRef.current),
			opacity: "0"
		}, 0)

		// Fade in the new element
		timeline.add({
			targets: ReactDOM.findDOMNode(newChildRef.current),
			opacity: "1"
		}, 0)
	};

	render() {
		return (
			<div className="quesync-layout">
				<div className="quesync-drag-holder">
					<div className="quesync-drag-zone" />
					<IconButton
						className="quesync-drag-zone-button"
						icon="minimize"
						onClick={this.minimize}
					/>
					<IconButton
						className="quesync-drag-zone-button"
						icon="fullscreen"
						onClick={this.maximize}
					/>
					<IconButton
						className="quesync-drag-zone-button"
						icon="close"
						onClick={this.close}
					/>
				</div>
				{this.state.child}
				{this.state.newChild}
			</div>
		);
	}
}

export default Layout;
