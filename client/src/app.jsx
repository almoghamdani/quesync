import React, { Component } from "react";

import ConnectionStatus from "./components/connection_status";
import PageContainer from "./components/page_container";
import SearchField from "./components/search_field";
import FriendsPage from "./pages/friends_page";
import SoundSettingsMenu from "./components/sound_settings_menu";

import {
	TopAppBar,
	TopAppBarRow,
	TopAppBarSection,
	TopAppBarFixedAdjust
} from "@rmwc/top-app-bar";
import { IconButton } from "@rmwc/icon-button";
import { ThemeProvider } from "@rmwc/theme";
import { SnackbarQueue } from "@rmwc/snackbar";
import { Typography } from "@rmwc/typography";

import { queue } from "./notifications_queue";

import "./app.scss";

class App extends Component {
	state = {
		isSearchMenuOpen: false,
		isSoundSettingsOpen: false
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
				<SoundSettingsMenu
					open={this.state.isSoundSettingsOpen}
					onClose={() => this.setState({ isSoundSettingsOpen: false })}
				/>
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
							<Typography className="quesync-app-title" use="headline5">
								Quesync
							</Typography>
							<SearchField />
						</TopAppBarSection>
						<TopAppBarSection alignEnd>
							<IconButton
								className="quesync-app-bar-button quesync-settings-button"
								icon="settings"
								onClick={() => this.setState({ isSoundSettingsOpen: true })}
							/>
							<ConnectionStatus />
							<IconButton
								className="quesync-app-bar-button quesync-logout-button"
								icon="exit_to_app"
								onClick={() => this.props.transitionToStartPage()}
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

export default App;
