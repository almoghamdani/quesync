import React, { Component } from "react";

import { signIn, startAuth, finishAuth } from "../actions/userActions";

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
    
    static get height() {
		return height;
	}

	static get width() {
		return width;
    }
    
    static get formClass() {
        return formClass;
    }

	render() {
		return (
			<form
				className={"quesync-form " + formClass}
				ref="form"
				onSubmit={this.registerBtnClicked}
                style={{ opacity: "0", pointerEvents: this.props.interactable ? "" : "none" }}>
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
