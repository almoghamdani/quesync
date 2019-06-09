import React, { Component } from "react";

import { connect } from "react-redux";

import strongSignalImg from "../static/strong_signal.png";
import goodSignalImg from "../static/good_signal.png";
import mediumSignalImg from "../static/medium_signal.png";
import weakSignalImg from "../static/weak_signal.png";

import { Ripple } from "@rmwc/ripple";
import { Typography } from "@rmwc/typography";
import { MenuSurfaceAnchor, MenuSurface } from "@rmwc/menu";

import { Line } from "react-chartjs-2";

import "./connection_status.scss";

const STRONG_CONNECTION_RANGE = [0, 60];
const GOOD_CONNECTION_RANGE = [61, 80];
const MEDIUM_CONNECTION_RANGE = [81, 120];

const STRONG_CONNECTION = 0;
const GOOD_CONNECTION = 1;
const MEDIUM_CONNECTION = 2;
const WEAK_CONNECTION = 3;

class ConnectionStatus extends Component {
	state = {
		menuStatusOpen: false
	};

	getConnectionStatus = () => {
		const lastPing = this.props.pings.slice(-1)[0];

		// Strong connection
		if (lastPing <= STRONG_CONNECTION_RANGE[1]) return STRONG_CONNECTION;
		else if (
			lastPing >= GOOD_CONNECTION_RANGE[0] &&
			lastPing <= GOOD_CONNECTION_RANGE[1]
		)
			return GOOD_CONNECTION;
		else if (
			lastPing >= MEDIUM_CONNECTION_RANGE[0] &&
			lastPing <= MEDIUM_CONNECTION_RANGE[1]
		)
			return MEDIUM_CONNECTION;
		else return WEAK_CONNECTION;
	};

	getImageForStatus = status => {
		switch (status) {
			case STRONG_CONNECTION:
				return strongSignalImg;

			case GOOD_CONNECTION:
				return goodSignalImg;

			case MEDIUM_CONNECTION:
				return mediumSignalImg;

			default:
				return weakSignalImg;
		}
	};

	getTextForStatus = status => {
		switch (status) {
			case STRONG_CONNECTION:
				return "Strong Connection";

			case GOOD_CONNECTION:
				return "Good Connection";

			case MEDIUM_CONNECTION:
				return "Medium Connection";

			default:
				return "Weak Connection";
		}
	};

	getTextColorForStatus = status => {
		switch (status) {
			case STRONG_CONNECTION:
				return "#00e676";

			case GOOD_CONNECTION:
				return "#c6ff00";

			case MEDIUM_CONNECTION:
				return "#ff9100";

			default:
				return "#ff1744";
		}
	};

	convertHexColorToRGBA = (hex, opacity) => {
		hex = hex.replace("#", "");

		const r = parseInt(hex.substring(0, 2), 16);
		const g = parseInt(hex.substring(2, 4), 16);
		const b = parseInt(hex.substring(4, 6), 16);

		return "rgba(" + r + "," + g + "," + b + "," + opacity / 100 + ")";
	};

	render() {
		const lastPing = this.props.pings.slice(-1)[0];
		const chartPings = this.props.pings.slice(0, 10);
		const status = this.getConnectionStatus();

		return (
			<MenuSurfaceAnchor>
				<MenuSurface
					className="quesync-connection-status-menu-surface"
					open={this.state.menuStatusOpen}
					onClose={() =>
						this.setState({
							menuStatusOpen: false
						})
					}
				>
					<div className="quesync-connection-status-menu">
						<div className="quesync-connection-status-info">
							<div className="quesync-connection-status-text">
								<Typography
									use="headline6"
									style={{ color: this.getTextColorForStatus(status) }}
								>
									{this.getTextForStatus(status)}
								</Typography>
							</div>

							<div className="quesync-connection-status-current-ping">
								<Typography use="subtitle2">
									<span>Current Ping: </span>
									<span style={{ color: this.getTextColorForStatus(status) }}>
										{lastPing}ms
									</span>
								</Typography>
							</div>
						</div>
						<div className="quesync-connection-status-chart">
							<Line
								data={{
									labels: Array(chartPings.length).fill(""),
									datasets: [
										{
											fill: true,
											backgroundColor: this.convertHexColorToRGBA(
												this.getTextColorForStatus(status),
												40
											),
											borderColor: this.getTextColorForStatus(status),
											data: chartPings
										}
									]
								}}
								options={{
									legend: {
										display: false
									},
									tooltips: {
										enabled: false
									},
									scales: {
										xAxes: [
											{
												display: "none"
											}
										]
									},
									maintainAspectRatio: false
								}}
								width={80}
								height={150}
							/>
						</div>
					</div>
				</MenuSurface>
				<Ripple
					accent
					onClick={() =>
						this.setState({
							menuStatusOpen: true
						})
					}
				>
					<div className="quesync-connection-status">
						<img
							style={{ width: "20px", height: "20px" }}
							src={this.getImageForStatus(status)}
							alt={this.getTextForStatus(status)}
						/>
						<Typography
							use="subtitle1"
							style={{
								color: this.getTextColorForStatus(status),
								paddingLeft: "5px",
								userSelect: "none"
							}}
						>
							{lastPing >= 0 ? "Connected" : "Not Connected"}
						</Typography>
					</div>
				</Ripple>
			</MenuSurfaceAnchor>
		);
	}
}

export default connect(state => ({
	pings: state.client.pings
}))(ConnectionStatus);
