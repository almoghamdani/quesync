import React, { Component } from "react";
import { connect } from "react-redux";

import { NotchedOutline } from "@rmwc/notched-outline";
import { FloatingLabel } from "@rmwc/floating-label";

import {
	sendMessage,
	setNewMessageForChannel
} from "../actions/messagesActions";

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
			this.state.isFocused ||
			(this.props.newMessages[this.props.channelId] &&
				this.props.newMessages[this.props.channelId].length)
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
							value={
								this.props.newMessages[this.props.channelId]
									? this.props.newMessages[this.props.channelId]
									: ""
							}
							onChange={event =>
								this.props.dispatch(
									setNewMessageForChannel(
										event.target.value,
										this.props.channelId
									)
								)
							}
							onKeyPress={event => {
								if (event.key === "Enter" && !event.shiftKey) {
									// Don't insert a new line
									event.preventDefault();

									// If the message isn't blank, send the message
									if (
										!/^\s*$/.test(this.props.newMessages[this.props.channelId])
									) {
										this.props
											.dispatch(
												sendMessage(
													this.props.newMessages[this.props.channelId],
													this.props.channelId
												)
											)
											.then(() => this.props.sendCallback());
									}
								}
							}}
						/>
					</div>
					<NotchedOutline notch={this.state.notch}>
						<FloatingLabel
							float={
								this.state.isFocused ||
								(this.props.newMessages[this.props.channelId] &&
									this.props.newMessages[this.props.channelId].length)
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

export default connect(state => ({
	newMessages: state.messages.newMessages
}))(MessageField);
