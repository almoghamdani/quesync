import React, { Component } from "react";

import SidePanel from "./side_panel";

class PageContainer extends Component {
	constructor(props) {
		super(props);

		this.state = {
			currentPage: props.page,
			sidePanelElement: null
		};
	}

	render() {
		const page = React.cloneElement(this.state.currentPage, {
			setSidePanelElement: e =>
				this.setState({
					sidePanelElement: e
				})
		});

		return (
			<div
				className="quesync-page-container"
				style={{ width: "100%", flexGrow: 1, display: "flex" }}
			>
				{page}
				<SidePanel element={this.state.sidePanelElement} />
			</div>
		);
	}
}

export default PageContainer;
