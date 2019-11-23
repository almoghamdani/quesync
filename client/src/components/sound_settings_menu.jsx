import React, { Component } from "react";
import { connect } from "react-redux";

import { Select } from "@rmwc/select";
import {
	Dialog,
	DialogTitle,
	DialogContent,
	DialogActions,
	DialogButton
} from "@rmwc/dialog";

import "./sound_settings_menu.scss";

class SoundSettingsMenu extends Component {
	dummyInputRef = React.createRef();

	render() {
		const inputDevices = this.props.client.voice().getInputDevices();
		const outputDevices = this.props.client.voice().getOutputDevices();

		let inputDevicesObj = {};
		inputDevices.forEach(
			device =>
				(inputDevicesObj[device.id] =
					device.name + (device.isDefault ? " (Default)" : ""))
		);

		let outputDevicesObj = {};
		outputDevices.forEach(
			device =>
				(outputDevicesObj[device.id] =
					device.name + (device.isDefault ? " (Default)" : ""))
		);

		let selectedInputDevice = inputDevices.filter(
			({ isSelected }) => isSelected
		);
		selectedInputDevice = selectedInputDevice.length
			? selectedInputDevice[0].id.toString()
			: 0;

		let selectedOutputDevice = outputDevices.filter(
			({ isSelected }) => isSelected
		);
		selectedOutputDevice = selectedOutputDevice.length
			? selectedOutputDevice[0].id.toString()
			: 0;

		return (
			<Dialog
				className="quesync-sound-settings-dialog"
				preventOutsideDismiss
				open={this.props.open}
				onClose={this.props.onClose}
			>
				<DialogTitle>
					<b>Sound Settings</b>
				</DialogTitle>
				<DialogContent className="quesync-sound-settings-dialog-content">
					<input
						ref={this.dummyInputRef}
						type="text"
						autoFocus
						style={{ display: "none" }}
					/>
					<Select
						className="quesync-sound-settings-select"
						key="output"
						enhanced
						label="Output Device"
						value={selectedOutputDevice}
						icon="headset"
						options={outputDevicesObj}
						onChange={evt =>
							this.props.client
								.voice()
								.setOutputDevice(parseInt(evt.detail.value))
								.then(() => this.forceUpdate())
								.catch(() => this.forceUpdate())
						}
					/>
					<Select
						className="quesync-sound-settings-select"
						key="input"
						enhanced
						label="Input Device"
						value={selectedInputDevice}
						icon="mic"
						options={inputDevicesObj}
						onChange={evt =>
							this.props.client
								.voice()
								.setInputDevice(parseInt(evt.detail.value))
								.then(() => this.forceUpdate())
								.catch(() => this.forceUpdate())
						}
					/>
				</DialogContent>
				<DialogActions>
					<DialogButton action="close" isDefaultAction>
						Close
					</DialogButton>
				</DialogActions>
			</Dialog>
		);
	}
}

export default connect(state => ({
	client: state.client.client
}))(SoundSettingsMenu);
