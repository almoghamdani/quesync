import React, { Component } from "react";

class App extends Component {
	render() {
		return <div className="quesync-app" style={{ ...this.props.style, width: "100vw", height: "100vh", background: "blue", position: "absolute", top: 0, left: 0 }} />;
	}
}

export default App;
