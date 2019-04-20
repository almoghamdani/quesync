import React, { Component } from "react";

class PageContainer extends Component {
	constructor(props) {
		super(props);

		this.state = {
			currentPage: props.page
		};
	}

	render() {
		return (
			<div
				className="quesync-page-container"
				style={{ width: "100%", flexGrow: 1, display: "flex" }}
			>
				{this.state.currentPage}
			</div>
		);
	}
}

export default PageContainer;
