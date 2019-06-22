import React, { Component } from "react";

import { NotchedOutline } from "@rmwc/notched-outline";
import { FloatingLabel } from "@rmwc/floating-label";

import "./message_field.scss";

class MessageField extends Component {
	state = {
		isFocused: false,
		notch: 0
	};

	labelRef = React.createRef();

	componentDidMount = () => {
		this.updateNotch();
	};

	componentDidUpdate = () => {
		this.updateNotch();
	};

	updateNotch = () => {
		const notch =
			this.state.isFocused || (this.props.value && this.props.value.length)
				? this.labelRef.current.getWidth() * 0.75
				: 0;

		if (this.state.notch !== notch) {
			this.setState({
				notch: notch
			});
		}
	};

	render() {
		return (
			<div className="quesync-message-field-wrapper">
				<div
					className={
						this.state.isFocused
							? "quesync-message-field mdc-text-field mdc-text-field--upgraded mdc-text-field--outlined mdc-text-field--focused quesync-message-field__focused"
							: "quesync-message-field mdc-text-field mdc-text-field--upgraded mdc-text-field--outlined"
					}
				>
					<div className="quesync-message-field-content">
						<textarea
							className="quesync-message-field-input mdc-text-field__input"
							onFocus={() => this.setState({ isFocused: true })}
							onBlur={() => this.setState({ isFocused: false })}
							value={this.props.value}
							onChange={event => this.props.setNewValue(event.target.value)}
							onKeyPress={event => {
								if (event.key === "Enter" && !event.shiftKey) {
									// Don't insert a new line
									event.preventDefault();

									// If the message isn't blank, send the message
									if (!/^\s*$/.test(this.props.value)) {
										this.props.send();
									}
								}
							}}
						/>
					</div>
					<NotchedOutline notch={this.state.notch}>
						<FloatingLabel
							float={
								this.state.isFocused ||
								(this.props.value && this.props.value.length)
							}
							ref={this.labelRef}
						>
							Message Here
						</FloatingLabel>
					</NotchedOutline>
				</div>
			</div>
		);
	}
}

export default MessageField;
