import React, { Component } from "react";

import { Scrollbars } from "react-custom-scrollbars";

class DrawerScrollbars extends Component {
	render() {
		return (
			<Scrollbars
				autoHide
				renderThumbVertical={({ style, ...props }) => (
					<div
						style={{
							...style,
							width: "4px",
							background: "rgba(0, 0, 0, 0.2)",
							marginLeft: "3px",
							borderRadius: "inherit",
							cursor: "pointer"
						}}
						{...props}
					/>
				)}
				{...this.props}
			/>
		);
	}
}

export default DrawerScrollbars;
