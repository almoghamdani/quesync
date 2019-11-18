import React, { Component } from "react";
import { connect } from "react-redux";

import FadeTransition from "./fade_transition";
import UpdateBlocker from "./update_blocker";

import { Typography } from "@rmwc/typography";
import { LinearProgress } from "@rmwc/linear-progress";
import { IconButton } from "@rmwc/icon-button";

import prettyBytes from "pretty-bytes";

import { startDownload, stopFileTransmission } from "../actions/filesActions";

import "./attachment.scss";

const { dialog } = window.require("electron").remote;

class Attachment extends Component {
	state = {
		intervalId: 0,
		oldBytes: 0,
		bps: 0,
		downloaded: false
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
		const attachmentFile = this.props.files[this.props.id];

		if (attachmentFile && this.props.filesProgress[this.props.id]) {
			this.setState({
				bps: this.props.filesProgress[this.props.id] - this.state.oldBytes, // Calc BPS
				oldBytes: this.props.filesProgress[this.props.id] // Save the current bytes
			});
		}
	};

	downloadFile = _ => {
		const attachmentFile = this.props.files[this.props.id];

		// Reset state
		this.setState({
			downloaded: false
		});

		// Get from the user the path to download the file to
		const downloadPath = dialog.showSaveDialogSync({
			title: `Choose where to save the file ${attachmentFile.name}`,
			defaultPath: attachmentFile.name
		});

		// If there is a download path
		if (downloadPath) {
			// Reset state
			this.setState({
				oldBytes: 0,
				bps: 0
			});

			// Start the download
			this.props.dispatch(startDownload(this.props.id, downloadPath)).then(() =>
				// Set as downloaded
				this.setState({
					downloaded: true
				})
			);
		}
	};

	stopDownload = _ => {
		// Reset state
		this.setState({
			downloaded: false
		});

		// Stop the download
		this.props.dispatch(stopFileTransmission(this.props.id));
	};

	render() {
		const attachmentFile = this.props.files[this.props.id];
		const downloading = this.props.downloading.includes(this.props.id);

		return (
			<div className="quesync-attachment">
				<div className="quesync-attachment-content">
					<div className="quesync-attachment-details">
						<Typography className="quesync-attachment-name" use="subtitle2">
							<b>File: </b>
							{attachmentFile.name}
						</Typography>
						<FadeTransition
							mountOnEnter
							unmountOnExit
							timeout={250}
							in={
								this.state.downloaded ||
								(downloading && !!this.props.filesProgress[this.props.id])
							}
						>
							<UpdateBlocker
								listenObject={this.props.filesProgress[this.props.id]}
							>
								<div className="quesync-attachment-download-progress-wrapper">
									<LinearProgress
										className="quesync-attachment-download-progress"
										progress={
											this.props.filesProgress[this.props.id] /
											attachmentFile.size
										}
									/>
									<Typography
										className="quesync-attachment-download-progress-text"
										use="subtitle2"
									>
										<b>
											{this.props.filesProgress[this.props.id] ===
												attachmentFile.size ||
											!this.props.filesProgress[this.props.id]
												? "Download completed!"
												: `${prettyBytes(
														this.props.filesProgress[this.props.id]
												  )} of ${prettyBytes(attachmentFile.size)}`}
										</b>
										{this.props.filesProgress[this.props.id] ===
										attachmentFile.size
											? ""
											: `, ${prettyBytes(this.state.bps)}/s`}
									</Typography>
								</div>
							</UpdateBlocker>
						</FadeTransition>
					</div>
					<IconButton
						className={
							downloading
								? "quesync-attachment-action quesync-attachment-stop-action"
								: "quesync-attachment-action quesync-attachment-download-action"
						}
						icon={downloading ? "close" : "arrow_downward"}
						onClick={downloading ? this.stopDownload : this.downloadFile}
					/>
				</div>
			</div>
		);
	}
}

export default connect(state => ({
	files: state.files.files,
	filesProgress: state.files.filesProgress,
	downloading: state.files.downloading
}))(Attachment);
