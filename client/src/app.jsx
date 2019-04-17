import React, { Component } from "react";

import {
	TopAppBar,
	TopAppBarRow,
	TopAppBarSection,
	TopAppBarNavigationIcon,
	TopAppBarTitle,
	TopAppBarFixedAdjust
} from "@rmwc/top-app-bar";
import { ThemeProvider } from "@rmwc/theme";

class App extends Component {
	render() {
		return (
			<ThemeProvider
				className="quesync-app"
				style={{
					...this.props.style,
					width: "100vw",
					height: "100vh",
					background: "#424242",
					position: "absolute",
					top: 0,
					left: 0
				}}
				options={{ primary: "#212121", secondary: "#e0e0e0" }}
			>
				<div
					className="quesync-seperator"
					style={{ width: "100vw", height: "30px", background: "black" }}
				/>
				<TopAppBar>
					<TopAppBarRow>
						<TopAppBarSection alignStart>
							<TopAppBarNavigationIcon icon="menu" />
							<TopAppBarTitle>All Features</TopAppBarTitle>
						</TopAppBarSection>
					</TopAppBarRow>
				</TopAppBar>
				<TopAppBarFixedAdjust />
			</ThemeProvider>
		);
	}
}

export default App;
