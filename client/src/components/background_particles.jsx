import React, { Component } from "react";

import Particles from "react-particles-js";

export default class BackgroundParticles extends Component {
	shouldComponentUpdate() {
		return false;
	}

	render() {
		var particlesParams = {
			particles: {
				number: {
					value: 80,
					density: {
						enable: true,
						value_area: 800
					}
				},
				color: {
					value: "#ffffff"
				},
				shape: {
					type: "circle",
					stroke: {
						width: 0,
						color: "#000000"
					},
					polygon: {
						nb_sides: 5
					},
					image: {
						src: "img/github.svg",
						width: 100,
						height: 100
					}
				},
				opacity: {
					value: 1,
					random: true,
					anim: {
						enable: true,
						speed: 1,
						opacity_min: 0,
						sync: false
					}
				},
				size: {
					value: 3,
					random: true,
					anim: {
						enable: false,
						speed: 4,
						size_min: 0.3,
						sync: false
					}
				},
				line_linked: {
					enable: false
				},
				move: {
					enable: true,
					speed: 0.3,
					direction: "top",
					random: true,
					straight: false,
					out_mode: "out",
					bounce: false
				}
			}
		};

		return (
			<Particles
				className="quesync-particles"
				ref="particles"
				params={particlesParams}
				style={this.props.style}
			/>
		);
	}
}
