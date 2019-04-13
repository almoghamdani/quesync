import React, { Component } from "react";
import { connect } from "react-redux";

import LoginForm from "./components/login_form";
import BackgroundParticles from "./components/background_particles";

import { ThemeProvider } from "@rmwc/theme";
import { Typography } from "@rmwc/typography";
import { Elevation } from "@rmwc/elevation";

import "./start_page.scss";

import clientSet from "./actions/clientActions";
const electron = window.require("electron");

class StartPage extends Component {
	componentWillMount() {
		// Get the client object from the global variables
		var client = electron.remote.getGlobal("client");

		// Save the errors object in the window to be accessible for all
		window.errors = client.errors;

		// Set the client in the store
		this.props.dispatch(clientSet(client));
	}

	render() {
		return (
			<ThemeProvider
				className="quesync-start-page"
				options={{ primary: "#007EA7", secondary: "#e0e0e0" }}>
				<BackgroundParticles
					style={{
						position: "absolute",
						top: "0",
						left: "0",
						minWidth: "100%",
						minHeight: "100%",
						zIndex: "-1"
					}}
				/>
				<div className="quesync-transition-holder">
					<div className="quesync-transition quesync-title" ref="transition">
						<Typography
							className="quesync-transition-title"
							use="headline2"
							style={{ color: "white", userSelect: "none", opacity: "1" }}>
							Quesync
						</Typography>
					</div>
				</div>
				<Elevation
					className="quesync-start-menu"
					z="8"
					style={{
						pointerEvents: true ? "none" : ""
					}}>
					<div className="quesync-form-side quesync-title" />
					<LoginForm />
				</Elevation>
			</ThemeProvider>
		);
	}
}

export default connect()(StartPage);
