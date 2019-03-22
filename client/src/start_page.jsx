import React, { Component } from "react";
import { connect } from "react-redux";

import clientSet from "./actions/clientActions";
const electron = window.require("electron");

class StartPage extends Component {
	componentWillMount() {
        // Get the client object from the global variables
        var client = electron.remote.getGlobal("client");

        // Set the client in the store
		this.props.dispatch(clientSet(client));
	}

	render() {
		return <></>;
	}
}

export default connect()(StartPage);
