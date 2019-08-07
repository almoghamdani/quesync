import React, { Component } from "react";

import anime from "animejs";

import { ThemeProvider } from "@rmwc/theme";
import { Elevation } from "@rmwc/elevation";
import { Typography } from "@rmwc/typography";
import { Button } from "@rmwc/button";

import "./call_window.scss";

const { ipcRenderer } = window.require("electron");

class CallWindow extends Component {
	componentDidMount() {
		anime({
			targets: ".quesync-call-avatar",
			duration: 350,
			easing: "easeOutElastic(1.2, 0.55)",
			loop: true,
			direction: "alternate",
			height: ["70px", "110px"],
			width: ["70px", "110px"],
			borderRadius: ["35px", "55px"]
		});
	}

	render() {
		return (
			<ThemeProvider
				className="quesync-call-window"
				options={{
					primary: "#00b0ff",
					secondary: "#282828",
					error: "#ff1744",
					onSurface: "rgba(255,255,255,.87)"
				}}>
				<Elevation className="quesync-call-window-content" z={6}>
					<div className="quesync-call-avatar-wrapper">
						<img
							className="quesync-call-avatar"
							src={this.props.avatar}
							alt="Incoming Call"
						/>
					</div>
					<Typography className="quesync-call-nickname" use="headline5">
						{this.props.nickname}
					</Typography>
					<div className="quesync-call-actions">
						<Button
							raised
							className="quesync-call-action-button quesync-call-accept-button"
							label="Answer"
							onClick={() => {
								ipcRenderer.send("call-main-window-event", {
									name: "join-call",
									arg: this.props.id
								});
							}}
						/>
						<Button
							raised
							danger
							className="quesync-call-action-button quesync-call-decline-button"
							label="Reject"
						/>
					</div>
				</Elevation>
			</ThemeProvider>
		);
	}
}

export default CallWindow;
