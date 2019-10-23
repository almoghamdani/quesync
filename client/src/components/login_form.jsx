import React, { Component } from "react";

import { connect } from "react-redux";

import { Typography } from "@rmwc/typography";
import { TextField } from "@rmwc/textfield";
import { Button } from "@rmwc/button";

import { signIn, startAuth, finishAuth } from "../actions/userActions";

import updater from "../updater";

// Should be in rem units
const height = 27;
const width = 25;
const formClass = "quesync-login-form";

class LoginForm extends Component {
	state = {
		usernameError: false,
		passwordError: false
	};

	static get height() {
		return height;
	}

	static get width() {
		return width;
	}

	static get formClass() {
		return formClass;
	}

	formatError = error => {
		if (this.state.usernameError) {
			return "No username entered!";
		} else if (this.state.passwordError) {
			return "No password entered!";
		}

		// Set the error message by the error code
		switch (error) {
			case 0:
				return "";

			case window.errors.user_not_found:
				return "The requested user is not found!";

			case window.errors.incorrect_password:
				return "Incorrect password!";

			case window.errors.already_connected_in_other_location:
				return "Already connected in another location!";

			default:
				return "Unknown error occurred!\nPlease try again later.";
		}
	};

	loginBtnClicked = event => {
		var username = this.refs.form[0].value,
			password = this.refs.form[1].value;

		// Prevent the default load of the form
		event.preventDefault();

		// Blur all elements to prevent further input
		if (document && document.activeElement) document.activeElement.blur();

		// Reset errors
		this.setState({
			usernameError: false,
			passwordError: false
		});

		// If the username field is empty, set it as error
		if (username.length === 0) {
			// Set it as invalid
			this.setState({
				usernameError: true
			});
			return;
		} else if (password.length === 0) {
			// If the password field is empty, set it as error
			// Set it as invalid
			this.setState({
				passwordError: true
			});
			return;
		}

		// Start authenticating
		this.props.dispatch(startAuth());

		// Start the loading animation
		this.props.startLoadingAnimation(() => {
			this.props
				.dispatch(signIn(username, password))
				.then(async res => {
					const sessionId = res.value.sessionId;

					// Set the session id
					localStorage.setItem("_qpsid", sessionId);

					// Stop authenticating
					this.props.dispatch(finishAuth());

					// Fetch for the first time the data of the user
					await updater.update();

					// Transition to the app
					this.props.transitionToApp();
				})
				.catch(() => {
					// Stop the loading animation
					this.props.stopLoadingAnimation(() => {
						// Stop authenticating
						this.props.dispatch(finishAuth());
					});
				});
		});
	};

	registerBtnClicked = () => {
		this.props.transitionToRegister();
	};

	render() {
		return (
			<form
				className={"quesync-form " + formClass}
				ref="form"
				style={{ pointerEvents: this.props.interactable ? "" : "none" }}
				onSubmit={this.loginBtnClicked}
			>
				<Typography
					use="headline2"
					style={{
						color: "var(--mdc-theme-primary)",
						userSelect: "none"
					}}
				>
					Login
				</Typography>
				<div className="quesync-error-holder">
					<Typography
						className="quesync-login-error"
						use="caption"
						style={{
							color: "#ff1744",
							userSelect: "none",
							whiteSpace: "pre-line",
							lineHeight: "12px"
						}}
					>
						{this.formatError(this.props.error)}
					</Typography>
				</div>
				<TextField
					invalid={this.state.usernameError}
					outlined
					label="Username"
					icon="person"
					style={{ width: "300px" }}
				/>
				<TextField
					invalid={this.state.passwordError}
					outlined
					label="Password"
					type="password"
					icon="lock"
					style={{ marginTop: "15px", width: "300px" }}
				/>
				<Button
					type="submit"
					raised
					style={{ marginTop: "35px", width: "300px" }}
					theme={["secondary"]}
				>
					Login
				</Button>
				<Button
					type="button"
					raised
					style={{
						marginTop: "15px",
						width: "300px",
						background: "#00A8E8"
					}}
					onClick={this.registerBtnClicked}
				>
					Don't have an account yet?
				</Button>
			</form>
		);
	}
}

export default connect(state => ({
	error: state.user.authError
}))(LoginForm);
