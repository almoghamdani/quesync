import React, { Component } from "react";

import BackgroundParticles from "./components/background_particles";

import { ThemeProvider } from "@rmwc/theme";
import { TextField } from "@rmwc/textfield";
import { Typography } from "@rmwc/typography";
import { Elevation } from "@rmwc/elevation";
import { Button } from "@rmwc/button";

import "./server_ip_window.scss";

const { ipcRenderer } = window.require("electron");

class ServerIPWindow extends Component {
	formSubmit = e => {
		const serverIP = this.refs.form[0].value;

		// Prevent the default action for the form
		e.preventDefault();

		// Save Server IP
		this.saveServerIP(serverIP);
	};

	saveServerIP = serverIP => {
		// Send the Server IP to the renderer
		ipcRenderer.send("save-server-ip", serverIP);
	};

	render() {
		return (
			<ThemeProvider
				className="quesync-server-ip-window"
				options={{
					primary: "#00b0ff",
					secondary: "#282828",
					error: "#ff1744",
					onSurface: "rgba(255,255,255,.87)"
				}}
			>
				<BackgroundParticles
					style={{
						position: "absolute",
						top: "0",
						left: "0",
						minWidth: "100%",
						minHeight: "100%"
					}}
				/>
				<Elevation className="quesync-server-ip-form-wrapper" z={7}>
					<form
						className="quesync-server-ip-form"
						ref="form"
						onSubmit={this.formSubmit}
					>
						<Typography use="subtitle1">
							Please enter the Server's IP Address:
						</Typography>
						<TextField
							className="quesync-server-ip-field"
							outlined
							label="Server's IP Address"
							required
							pattern={
								"^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
							}
						/>
						<div className="quesync-server-ip-form-actions">
							<Button
								raised
								type="button"
								style={{ background: "#00c853" }}
								onClick={() => this.saveServerIP("127.0.0.1")}
							>
								Local Host
							</Button>
							<Button raised>Connect</Button>
						</div>
					</form>
				</Elevation>
			</ThemeProvider>
		);
	}
}

export default ServerIPWindow;
