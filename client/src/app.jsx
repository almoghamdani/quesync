import React, { Component } from "react";

import {
	TopAppBar,
	TopAppBarRow,
	TopAppBarSection,
	TopAppBarFixedAdjust
} from "@rmwc/top-app-bar";
import { TabBar, Tab } from "@rmwc/tabs";
import { ThemeProvider } from "@rmwc/theme";

import "./app.scss";

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
				options={{
					primary: "#00A8E8",
					secondary: "#212121",
					onSurface: "rgba(255,255,255,.87)"
				}}
			>
				<div
					className="quesync-seperator"
					style={{ width: "100vw", height: "30px", background: "#212121" }}
				/>
				<TopAppBar theme="secondaryBg">
					<TopAppBarRow>
						<TopAppBarSection className="quesync-page-selector-container">
							<TabBar className="quesync-page-selector">
								<Tab className="quesync-page-tab">Friends</Tab>
								<Tab className="quesync-page-tab">Recent</Tab>
							</TabBar>
						</TopAppBarSection>
					</TopAppBarRow>
				</TopAppBar>
				<TopAppBarFixedAdjust />
			</ThemeProvider>
		);
	}
}

export default App;
