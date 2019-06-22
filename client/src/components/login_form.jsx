import React, { Component } from "react";

import { signIn, startAuth, finishAuth } from "../actions/userActions";

import { Typography } from "@rmwc/typography";
import { TextField } from "@rmwc/textfield";
import { Button } from "@rmwc/button";

import { connect } from "react-redux";

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
		// Set the error message by the error code
		switch (error) {
			case 0:
				return "";

			case window.errors.USER_NOT_FOUND:
				return "The requested user is not found!";

			case window.errors.INCORRECT_PASSWORD:
				return "Incorrect password!";

			default:
				return "Unknown error occurred!\nPlease try again later.";
		}
	};

	loginBtnClicked = event => {
		var username = this.refs.form[0].value,
			password = this.refs.form[1].value;

		// Prevent the default load of the form
		event.preventDefault();

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
				.dispatch(signIn(this.props.client, username, password))
				.then(async () => {
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
					}, LoginForm);
				});
		}, LoginForm);
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
				onSubmit={this.loginBtnClicked}>
				<Typography
					use="headline2"
					style={{
						color: "var(--mdc-theme-primary)",
						userSelect: "none"
					}}>
					Login
				</Typography>
				<TextField
					invalid={this.state.usernameError}
					outlined
					label="Username"
					style={{ marginTop: "38px", width: "300px" }}
				/>
				<TextField
					invalid={this.state.passwordError}
					outlined
					label="Password"
					type="password"
					style={{ marginTop: "15px", width: "300px" }}
				/>
				<Button
					type="submit"
					raised
					style={{ marginTop: "35px", width: "300px" }}
					theme={["secondary"]}>
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
					onClick={this.registerBtnClicked}>
					Don't have an account yet?
				</Button>
				<div className="quesync-error-holder">
					<Typography
						className="quesync-login-error"
						use="caption"
						style={{
							color: "#ff1744",
							paddingTop: "25px",
							userSelect: "none",
							whiteSpace: "pre-line",
							lineHeight: "12px"
						}}>
						{this.formatError(this.props.error)}
					</Typography>
				</div>
			</form>
		);
	}
}

export default connect(state => ({
	client: state.client.client,
	error: state.auth.authError
}))(LoginForm);
