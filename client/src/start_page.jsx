import React, { Component } from "react";
import { connect } from "react-redux";

import BackgroundParticles from "./components/background_particles";

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
			<div className="quesync-start-page">
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
			</div>
		);
	}
}

export default connect()(StartPage);
