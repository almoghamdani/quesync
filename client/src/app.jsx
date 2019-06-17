import React, { Component } from "react";

import { connect } from "react-redux";

import ConnectionStatus from './components/connection_status'
import PageContainer from "./components/page_container";
import FriendsPage from "./pages/friends_page";

import {
	TopAppBar,
	TopAppBarRow,
	TopAppBarSection,
	TopAppBarFixedAdjust
} from "@rmwc/top-app-bar";
import { ThemeProvider } from "@rmwc/theme";

import EventHandler from "./event-handler";

import "./app.scss";

class App extends Component {
	eventHandler = new EventHandler(this);

	componentDidMount() {
		this.eventHandler.register(this.props.client);
	}

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
				}}>
				<TopAppBar
					className="quesync-top-app-bar mdc-top-app-bar--fixed-scrolled"
					fixed
					theme="secondaryBg">
					<TopAppBarRow>
						<TopAppBarSection alignEnd>
							<ConnectionStatus />
						</TopAppBarSection>
					</TopAppBarRow>
				</TopAppBar>
				<TopAppBarFixedAdjust className="quesync-top-app-bar-seperator" />
				<PageContainer page={<FriendsPage />} />
			</ThemeProvider>
		);
	}
}

export default connect(state => ({
	client: state.client.client
}))(App);
