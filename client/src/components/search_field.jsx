import React, { Component } from "react";
import { connect } from "react-redux";

import { TextField } from "@rmwc/textfield";
import { MenuSurfaceAnchor, MenuSurface } from "@rmwc/menu";
import { Button } from "@rmwc/button";
import { List, ListItem } from "@rmwc/list";
import { CircularProgress } from "@rmwc/circular-progress";
import { Avatar } from "@rmwc/avatar";
import { Typography } from "@rmwc/typography";

import { searchUser } from "../actions/usersActions";

import updater from "../updater";

import "./search_field.scss";

function MessageFieldItem({
	id,
	nickname,
	tag,
	avatarUrl,
	friendsAlready,
	sendFriendRequest,
	requestSent
}) {
	return (
		<ListItem key={id} className="quesync-search-field-item">
			<Avatar className="quesync-search-field-item-avatar" src={avatarUrl} />
			<Typography className="quesync-search-field-item-name" use="subtitle1">
				{nickname}#{tag}
			</Typography>
			<Button
				className="quesync-search-field-item-button"
				raised
				label={
					friendsAlready
						? "Already Friends"
						: requestSent
						? "Request Sent"
						: "Send Friend Request"
				}
				disabled={friendsAlready || requestSent}
				onClick={() => {
					// Send friend request to the user
					sendFriendRequest(id);
				}}
			/>
		</ListItem>
	);
}

class MessageFieldMenuSurface extends MenuSurface {
	handleBodyClick = evt => {
		if (this.foundation) {
			const el = evt.target;

			// Prevent from the menu to close when the search field is clicked
			if (
				this.foundation.adapter_.isElementInContainer(el) ||
				(el.className && el.className.includes("quesync-search-field")) ||
				(el.parentNode &&
					el.parentNode.className &&
					el.parentNode.className.includes("quesync-search-field"))
			) {
				return;
			}

			this.foundation.close();
		}
	};
}

class MessageField extends Component {
	state = {
		searchValue: "",
		isResultsMenuOpen: false
	};

	parseValue = value => {
		let nickname,
			tag = Number(-1),
			tagStr;

		// Check if the new value has a tag in it
		if (value.includes("#")) {
			// Get the nickname and the tag
			[nickname, tagStr] = value.split("#");

			// If the user entered a full tag, parse it to the tag value
			if (tagStr && tagStr.length === 4) {
				tag = parseInt(tagStr);
			}
		} else {
			nickname = value;
		}

		return [nickname, tag];
	};

	onFieldChange = evt => {
		const newValue = evt.target.value;

		// Check if the new value is a valid search value
		if (newValue.match(/([^#])+(#[0-9]{0,4})?/)) {
			// Open the results menu if closed
			if (!this.state.isResultsMenuOpen) {
				this.setState({
					isResultsMenuOpen: true
				});
			}

			// Set it as the value of the text field
			this.setState({
				searchValue: newValue
			});

			// Parse the new value and the old value
			const [nickname, tag] = this.parseValue(newValue);
			const oldParse = this.parseValue(this.state.searchValue);

			// If the nickname or the tag has changed, search again
			if (nickname !== oldParse[0] || tag !== oldParse[1]) {
				// Search the user
				this.props
					.dispatch(searchUser(this.props.client, nickname, tag))
					.catch(() => {});
			}
		} else if (newValue === "") {
			this.setState({
				searchValue: ""
			});
		}
	};

	checkIfFriends = userId =>
		this.props.friends && this.props.friends.includes(userId);

	friendRequestSent = userId =>
		this.props.friendRequests &&
		this.props.friendRequests.map(req => req.friendId).includes(userId);

	sendFriendRequest = userId =>
		updater.sendFriendRequest(this.props.client, userId);

	render() {
		const results = this.props.results ? this.props.results : {};

		return (
			<div className="quesync-search-field-wrapper">
				<MenuSurfaceAnchor>
					<TextField
						className="quesync-search-field"
						outlined
						label="Search for your friends"
						trailingIcon={{
							icon: this.state.searchValue ? "close" : "",
							onClick: () =>
								this.setState({
									searchValue: ""
								})
						}}
						icon="search"
						value={this.state.searchValue}
						onChange={evt => this.onFieldChange(evt)}
						onFocus={() =>
							this.setState({
								isResultsMenuOpen: true
							})
						}
					/>
					<MessageFieldMenuSurface
						className="quesync-search-field-menu"
						open={
							this.state.isResultsMenuOpen &&
							this.state.searchValue &&
							this.state.searchValue.length > 0
						}
						onFocus={() =>
							this.setState({
								isResultsMenuOpen: true
							})
						}
						onClose={() =>
							this.setState({
								isResultsMenuOpen: false
							})
						}
						anchorCorner="bottomLeft"
					>
						<List onClick={evt => evt.stopPropagation()}>
							{this.props.searching ? (
								<ListItem
									disabled
									className="quesync-search-field-loading-item"
								>
									<CircularProgress />
								</ListItem>
							) : null}
							{Object.keys(results).map(userId => (
								<MessageFieldItem
									key={userId}
									id={userId}
									nickname={results[userId].nickname}
									tag={results[userId].tag}
									avatarUrl="https://jamesmfriedman.github.io/rmwc/images/avatars/captainamerica.png"
									friendsAlready={this.checkIfFriends(userId)}
									sendFriendRequest={this.sendFriendRequest}
									requestSent={this.friendRequestSent(userId)}
								/>
							))}
							{Object.keys(results).length === 0 ? (
								<ListItem
									className="quesync-search-field-no-results-item"
								>
									No Results
								</ListItem>
							) : null}
						</List>
					</MessageFieldMenuSurface>
				</MenuSurfaceAnchor>
			</div>
		);
	}
}

export default connect(state => ({
	client: state.client.client,
	searching: state.users.searching,
	results: state.users.searchResults,
	friends: state.user.user.friends,
	friendRequests: state.user.user.friendRequests
}))(MessageField);
