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
					...this.props.style
				}}
				options={{
					primary: "#00A8E8",
					secondary: "#212121",
					onSurface: "rgba(255,255,255,.87)"
				}}
			>
				<TopAppBar className="quesync-top-app-bar" fixed theme="secondaryBg">
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
