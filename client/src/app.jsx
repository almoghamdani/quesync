import React, { Component } from "react";

import { connect } from "react-redux";

import ConnectionStatus from "./components/connection_status";
import PageContainer from "./components/page_container";
import SearchField from "./components/search_field";
import FriendsPage from "./pages/friends_page";

import {
	TopAppBar,
	TopAppBarRow,
	TopAppBarSection,
	TopAppBarFixedAdjust
} from "@rmwc/top-app-bar";
import { IconButton } from "@rmwc/icon-button";
import { ThemeProvider } from "@rmwc/theme";
import { SnackbarQueue } from "@rmwc/snackbar";

import { queue } from "./messages_queue";

import "./app.scss";

class App extends Component {
	state = {
		isSearchMenuOpen: false
	};

	render() {
		return (
			<ThemeProvider
				className="quesync-app"
				style={{
					...this.props.style
				}}
				options={{
					primary: "#00b0ff",
					secondary: "#212121",
					error: "#ff1744",
					onSurface: "rgba(255,255,255,.87)"
				}}
			>
				<SnackbarQueue
					className="quesync-notifications-queue"
					messages={queue.messages}
				/>
				<TopAppBar
					className="quesync-top-app-bar mdc-top-app-bar--fixed-scrolled"
					fixed
					theme="secondaryBg"
				>
					<TopAppBarRow>
						<TopAppBarSection alignStart>
							<SearchField />
						</TopAppBarSection>
						<TopAppBarSection alignEnd>
							<ConnectionStatus />
							<IconButton
								className="quesync-logout-button"
								icon="exit_to_app"
								onClick={() => this.props.logout()}
							/>
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
