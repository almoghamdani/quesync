import React, { Component } from "react";

import { Typography } from "@rmwc/typography";
import { TextField } from "@rmwc/textfield";
import { Button } from "@rmwc/button";

import { connect } from "react-redux";

// Should be in rem units
const height = 27;
const width = 25;

class LoginFrom extends Component {
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

	formatError = error => {
		// Set the error message by the error code
		switch (error) {
			case window.errors.USER_NOT_FOUND:
				return "The requested user is not found!";

			case window.errors.INCORRECT_PASSWORD:
				return "Incorrect password!";

			default:
				return "Unknown error occurred!\nPlease try again later.";
		}
    };
    
    loginBtnClicked = event => {
        // Prevent the default load of the form
        event.preventDefault();

        // Start the loading animation
        this.props.startLoadingAnimation(() => {
            
        });
    };

	render() {
		return (
			<form
				className="quesync-form quesync-login-form"
				ref="form"
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
							lineHeight: "12px",
							opacity: "0"
						}}>
						{this.formatError(this.props.error)}
					</Typography>
				</div>
			</form>
		);
	}
}

export default connect(state => ({
	error: state.user.authError
}))(LoginFrom);
