import { Component } from "react";

class UpdateBlocker extends Component {
	shouldComponentUpdate(nextProps, _) {
		return (
			JSON.stringify(this.props.listenObject) !==
			JSON.stringify(nextProps.listenObject)
		);
	}

	render() {
		return this.props.children;
	}
}

export default UpdateBlocker;
