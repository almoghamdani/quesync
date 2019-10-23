import React, { Component } from "react";
import { connect } from "react-redux";

import AttachmentZone from "./attachment_zone";

import { NotchedOutline } from "@rmwc/notched-outline";
import { FloatingLabel } from "@rmwc/floating-label";
import { IconButton } from "@rmwc/icon-button";
import { MenuSurface, MenuSurfaceAnchor } from "@rmwc/menu";

import {
	sendMessage,
	setNewMessageContentForChannel
} from "../actions/messagesActions";

import "./message_field.scss";

class MessageField extends Component {
	state = {
		isFocused: false,
		notch: 0,
		attachmentMenuOpen: false
	};

	labelRef = React.createRef();

	componentDidMount = () => this.updateNotch();

	componentDidUpdate = () => this.updateNotch();

	updateNotch = () => {
		const notch =
			this.state.isFocused ||
			(this.props.newMessages[this.props.channelId] &&
				this.props.newMessages[this.props.channelId].content &&
				this.props.newMessages[this.props.channelId].content.length)
				? this.labelRef.current.getWidth() * 0.75
				: 0;

		if (this.state.notch !== notch) {
			this.setState({
				notch: notch
			});
		}
	};

	onKeyPress = event => {
		if (event.key === "Enter" && !event.shiftKey) {
			// Don't insert a new line
			event.preventDefault();

			// If the message isn't blank and the attachment is finished to upload, send the message
			if (
				this.props.newMessages[this.props.channelId] &&
				!/^\s*$/.test(this.props.newMessages[this.props.channelId].content) &&
				(!this.props.newMessages[this.props.channelId].attachmentId ||
					this.props.filesProgress[
						this.props.newMessages[this.props.channelId].attachmentId
					] ===
						this.props.files[
							this.props.newMessages[this.props.channelId].attachmentId
						].size)
			) {
				this.props
					.dispatch(
						sendMessage(
							this.props.newMessages[this.props.channelId].content,
							this.props.channelId,
							this.props.newMessages[this.props.channelId].attachmentId
						)
					)
					.then(() => this.props.sendCallback());
			}
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
								this.props.newMessages[this.props.channelId] &&
								this.props.newMessages[this.props.channelId].content
									? this.props.newMessages[this.props.channelId].content
									: ""
							}
							onChange={event =>
								this.props.dispatch(
									setNewMessageContentForChannel(
										event.target.value,
										this.props.channelId
									)
								)
							}
							onKeyPress={this.onKeyPress}
						/>
					</div>
					<MenuSurfaceAnchor>
						<MenuSurface
							className="quesync-attachment-menu-surface"
							hoistToBody
							open={this.state.attachmentMenuOpen}
							onClose={_ => this.setState({ attachmentMenuOpen: false })}
						>
							<div className="quesync-attachment-menu-content">
								<AttachmentZone channelId={this.props.channelId} />
							</div>
						</MenuSurface>
						<IconButton
							className={
								this.props.newMessages[this.props.channelId] &&
								this.props.newMessages[this.props.channelId].attachmentId
									? this.props.uploading
										? "quesync-attach-button quesync-attach-button-uploading"
										: "quesync-attach-button quesync-attach-button-attached"
									: "quesync-attach-button"
							}
							icon="attach_file"
							onClick={_ =>
								this.setState({
									attachmentMenuOpen: !this.state.attachmentMenuOpen
								})
							}
						/>
					</MenuSurfaceAnchor>
					<NotchedOutline notch={this.state.notch}>
						<FloatingLabel
							float={
								this.state.isFocused ||
								(this.props.newMessages[this.props.channelId] &&
									this.props.newMessages[this.props.channelId].content &&
									this.props.newMessages[this.props.channelId].content.length)
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

export default connect((state, ownProps) => ({
	newMessages: state.messages.newMessages,
	files: state.files.files,
	uploading:
		state.messages.newMessages[ownProps.channelId] &&
		state.messages.newMessages[ownProps.channelId].attachmentId
			? state.files.uploading.includes(
					state.messages.newMessages[ownProps.channelId].attachmentId
			  )
			: false
}))(MessageField);
