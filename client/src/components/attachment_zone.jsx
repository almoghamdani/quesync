import React, { Component } from "react";
import { connect } from "react-redux";

import { Typography } from "@rmwc/typography";
import { LinearProgress } from "@rmwc/linear-progress";
import { ThemeProvider } from "@rmwc/theme";

import Dropzone from "react-dropzone";

import prettyBytes from "pretty-bytes";

import { startUpload, stopFileTransmission } from "../actions/filesActions";
import { setNewMessageAttachmentForChannel } from "../actions/messagesActions";

import "./attachment_zone.scss";

class AttachmentZone extends Component {
	state = {
		intervalId: 0,
		oldBytes: 0,
		bps: 0
	};

	componentDidMount() {
		// Start the timer for the calc upload speed
		const intervalId = setInterval(this.calcUploadSpeed, 1000);

		// Save the interval ID
		this.setState({
			intervalId
		});
	}

	componentWillUnmount() {
		// Stop the BPS interval
		if (this.state.intervalId) {
			clearInterval(this.state.intervalId);
		}
	}

	calcUploadSpeed = () => {
		const attachmentId = this.props.newMessages[this.props.channelId]
			? this.props.newMessages[this.props.channelId].attachmentId
			: null;

		const attachmentFile = attachmentId ? this.props.files[attachmentId] : null;

		const uploading = this.props.uploading.includes(attachmentId);

		if (attachmentFile && uploading) {
			this.setState({
				bps: this.props.filesProgress[attachmentId] - this.state.oldBytes, // Calc BPS
				oldBytes: this.props.filesProgress[attachmentId] // Save the current bytes
			});
		} else if (this.state.bps) {
			this.setState({
				bps: 0 // Reset BPS
			});
		}
	};

	onDrop = files => {
		const attachmentId = this.props.newMessages[this.props.channelId]
			? this.props.newMessages[this.props.channelId].attachmentId
			: null;

		const attachmentFile = attachmentId ? this.props.files[attachmentId] : null;

		// If there is a file for upload
		if (files[0]) {
			// If a file is currently uploaded, stop it's transmission
			if (
				attachmentId &&
				this.props.filesProgress[attachmentId] !== attachmentFile.size
			) {
				this.props.dispatch(stopFileTransmission(attachmentId));
			}

			// Reset state
			this.setState({
				oldBytes: 0,
				bps: 0
			});

			// Upload the file
			this.props.dispatch(startUpload(files[0].path)).then(({ action }) => {
				// Set the file as the new attachment for the message
				this.props.dispatch(
					setNewMessageAttachmentForChannel(
						action.payload.file.id,
						this.props.channelId
					)
				);
			});
		}
	};

	render() {
		const attachmentId = this.props.newMessages[this.props.channelId]
			? this.props.newMessages[this.props.channelId].attachmentId
			: null;

		const attachmentFile = attachmentId ? this.props.files[attachmentId] : null;

		return (
			<Dropzone
				minSize={1}
				maxSize={500000000}
				multiple={false}
				onDrop={this.onDrop}
			>
				{({ getRootProps, getInputProps, isDragActive }) => (
					<ThemeProvider
						className="quesync-attachment-dropzone-wrapper"
						options={{
							primary: "#00b0ff",
							secondary: "#212121",
							error: "#ff1744",
							onSurface: "rgba(255,255,255,.87)"
						}}
					>
						{attachmentFile ? (
							<div className="quesync-new-attachment-details">
								<Typography
									className="quesync-new-attachment-name"
									use="subtitle2"
								>
									<b>File:</b> {attachmentFile.name}
								</Typography>
								<LinearProgress
									className="quesync-new-attachment-upload-progress"
									progress={
										this.props.filesProgress[attachmentId] / attachmentFile.size
									}
								/>
								<Typography
									className="quesync-new-attachment-upload-progress-text"
									use="subtitle2"
								>
									<b>
										{this.props.filesProgress[attachmentId] ===
										attachmentFile.size
											? "Upload completed!"
											: `Uploaded ${prettyBytes(
													this.props.filesProgress[attachmentId]
											  )} out of ${prettyBytes(attachmentFile.size)}`}
									</b>
								</Typography>
								<Typography
									className="quesync-new-attachment-upload-bps"
									use="subtitle2"
								>
									{prettyBytes(this.state.bps)}/s
								</Typography>
							</div>
						) : null}
						<div className="quesync-attachment-dropzone" {...getRootProps()}>
							<input {...getInputProps()} />
							<Typography
								className="quesync-attachment-description"
								use="subtitle1"
							>
								<b>
									{isDragActive
										? attachmentFile
											? "Dropping this file will stop the current upload and will start a new one"
											: "Drop to start upload"
										: "Drop here a file or click to select files"}
								</b>
							</Typography>
						</div>
					</ThemeProvider>
				)}
			</Dropzone>
		);
	}
}

export default connect(state => ({
	newMessages: state.messages.newMessages,
	files: state.files.files,
	filesProgress: state.files.filesProgress,
	uploading: state.files.uploading
}))(AttachmentZone);
