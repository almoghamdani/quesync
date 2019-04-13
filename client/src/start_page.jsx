import React, { Component } from "react";
import { connect } from "react-redux";

import LoginForm from "./components/login_form";
import BackgroundParticles from "./components/background_particles";

import { ThemeProvider } from "@rmwc/theme";
import { Typography } from "@rmwc/typography";
import { Elevation } from "@rmwc/elevation";
import { CircularProgress } from "@rmwc/circular-progress";
import anime from "animejs/lib/anime.es.js";

import "@rmwc/circular-progress/circular-progress.css";
import "./start_page.scss";

import clientSet from "./actions/clientActions";
const electron = window.require("electron");

class StartPage extends Component {
	componentWillMount() {
		// Get the client object from the global variables
		var client = electron.remote.getGlobal("client");

		// Save the errors object in the window to be accessible for all
		window.errors = client.errors;

		// Set the client in the store
		this.props.dispatch(clientSet(client));
	}

	startLoadingAnimation = (completeCallback, form) => {
		// Create an animation timeline for the title transition + loading animation
		var timeline = anime.timeline({
			duration: 1300,
			easing: "easeInOutCirc",
			delay: 250,
			complete: completeCallback
		});

		// Add animation for the title to fill the menu
		timeline.add({
			targets: ".quesync-title-moving",
			width: form.width * 2 + "rem"
		});

		// Reset the title position to make space for the loading indicator
		timeline.add(
			{
				targets: ".quesync-title-text",
				marginTop: "0px"
			},
			900
		);

		// Fade in the loading indicator
		timeline.add(
			{
				targets: ".quesync-loading",
				opacity: "1"
			},
			900
		);
    };
    
    stopLoadingAnimation = (completeCallback, form) => {
		// Create a timeline for the return of the title animation
        var timeline = anime.timeline({
            duration: 800,
            easing: "easeInOutCirc",
            delay: 250,
            complete: completeCallback
        });

        // Animate the quesync title moving part to return to it's place
        timeline.add({
            targets: ".quesync-title-moving",
            width: form.width + "rem"
        });

        // Fade out the loading indicator
        timeline.add(
            {
                targets: ".quesync-loading",
                opacity: "0"
            },
            0
        );

        // Return the title text to the center
        timeline.add(
            {
                targets: ".quesync-title-text",
                marginTop: "62px"
            },
            0
        );
	};

	render() {
		return (
			<ThemeProvider
				className="quesync-start-page"
				options={{ primary: "#007EA7", secondary: "#e0e0e0" }}>
				<BackgroundParticles
					style={{
						position: "absolute",
						top: "0",
						left: "0",
						minWidth: "100%",
						minHeight: "100%",
						zIndex: "-1"
					}}
				/>
				<div className="quesync-transition-holder">
					<div className="quesync-transition quesync-title" ref="transition">
						<Typography
							className="quesync-transition-title"
							use="headline2"
							style={{ color: "white", userSelect: "none", opacity: "1" }}>
							Quesync
						</Typography>
					</div>
				</div>
				<Elevation
					className="quesync-start-menu"
					z="8"
					style={{ pointerEvents: this.props.authenticating ? "none" : "" }}>
					<div className="quesync-form-side quesync-title" />
					<div className="quesync-form-side quesync-title quesync-title-moving">
						<Typography
							className="quesync-title-text"
							use="headline2"
							style={{ color: "white", userSelect: "none", marginTop: "55px" }}>
							Quesync
						</Typography>
						<CircularProgress
							className="quesync-loading"
							theme="secondary"
							style={{ marginTop: "38px", opacity: "0" }}
						/>
					</div>
					<div
						className="quesync-form-side quesync-form-holder"
						style={{
							width: LoginForm.width + "rem",
							height: LoginForm.height + "rem"
						}}>
						<LoginForm startLoadingAnimation={this.startLoadingAnimation} stopLoadingAnimation={this.stopLoadingAnimation} />
					</div>
				</Elevation>
			</ThemeProvider>
		);
	}
}

export default connect(state => ({
	authenticating: state.user.authenticating
}))(StartPage);
