import React, { Component } from "react";

import "./seperator.scss";

class Seperator extends Component {
	render() {
		return <div className="quesync-seperator" style={this.props.style} />;
	}
}

export default Seperator;
