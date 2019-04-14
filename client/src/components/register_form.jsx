import React, { Component } from "react";

import { register, startAuth, finishAuth } from "../actions/userActions";

import { Typography } from "@rmwc/typography";
import { TextField } from "@rmwc/textfield";
import { Button } from "@rmwc/button";

import { connect } from "react-redux";

// Should be in rem units
const height = 35;
const width = 25;
const formClass = "quesync-register-form";

class RegisterForm extends Component {
	state = {
		usernameError: false,
		passwordError: false,
		passwordMismatchError: false,
		emailError: false
	};

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

	registerBtnClicked = event => {
		var username = this.refs.form[0].value,
			email = this.refs.form[1].value,
			password = this.refs.form[2].value,
			passwordVerification = this.refs.form[3].value;

		// Prevent the default load of the form
		event.preventDefault();

		// Reset errors
		this.setState({
			usernameError: false,
			passwordError: false,
			emailError: false,
			passwordMismatchError: false
		});

		// If the username field is empty
		if (username.length === 0) {
			this.setState({
				usernameError: true
			});
			return;
		}
		// If the e-mail field is empty
		else if (email.length === 0) {
			this.setState({
				emailError: true
			});
			return;
		}
		// If the password field is empty
		else if (password.length === 0) {
			this.setState({
				passwordError: true
			});
			return;
		}
		// If the password verification field doesn't match the password
		else if (password !== passwordVerification) {
			this.setState({
				passwordMismatchError: true
			});
			return;
		}

		// Start authenticating
		this.props.dispatch(startAuth());

		// Start the loading animation
		this.props.startLoadingAnimation(() => {
			this.props
                .dispatch(register(this.props.client, username, email, password))
                .then(() => {
                    // Stop authenticating
                    this.props.dispatch(finishAuth());
                    
                    // Transition to the app
                    this.props.transitionToApp();
                })
				.catch(() => {
					// Stop the loading animation
					this.props.stopLoadingAnimation(() => {
						// Stop authenticating
						this.props.dispatch(finishAuth());
					}, RegisterForm);
				});
		}, RegisterForm);
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

	haveAccountBtnClicked = () => {
		this.props.transitionToLogin();
	};

	render() {
		return (
			<form
				className={"quesync-form " + formClass}
				ref="form"
				onSubmit={this.registerBtnClicked}
				style={{
					opacity: "0",
					pointerEvents: this.props.interactable ? "" : "none"
				}}>
				<Typography
					use="headline2"
					style={{
						color: "var(--mdc-theme-primary)",
						userSelect: "none"
					}}>
					Register
				</Typography>
				<TextField
					invalid={this.state.usernameError}
					outlined
					label="Username"
					style={{ marginTop: "38px", width: "300px" }}
				/>
				<TextField
					invalid={this.state.emailError}
					outlined
					label="E-mail"
					style={{ marginTop: "15px", width: "300px" }}
				/>
				<TextField
					invalid={this.state.passwordError}
					outlined
					label="Password"
					type="password"
					style={{ marginTop: "15px", width: "300px" }}
				/>
				<TextField
					invalid={this.state.passwordMismatchError}
					outlined
					label="Re-enter password"
					type="password"
					style={{ marginTop: "15px", width: "300px" }}
				/>
				<Button
					type="submit"
					raised
					style={{ marginTop: "30px", width: "300px" }}
					theme={["secondary"]}>
					Register
				</Button>
				<Button
					type="button"
					raised
					style={{
						marginTop: "15px",
						width: "300px",
						background: "#00A8E8"
					}}
					onClick={this.haveAccountBtnClicked}>
					Already have an account?
				</Button>
				<div className="quesync-error-holder">
					<Typography
						className="quesync-register-error"
						use="caption"
						style={{
							color: "#ff1744",
							paddingTop: "25px",
							userSelect: "none",
							whiteSpace: "pre-line",
							lineHeight: "12px",
							opacity: "0"
						}}>
						{this.state.registerError}
					</Typography>
				</div>
			</form>
		);
	}
}

export default connect(state => ({
	client: state.client.client,
	error: state.user.authError
}))(RegisterForm);
