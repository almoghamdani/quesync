import { getFileInfo } from "./filesActions";

export function sendMessage(content, channelId, attachmentId = null) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "SEND_MESSAGE",
			payload: client.messages().sendMessage(content, channelId, attachmentId)
		});
	}
}

export function getChannelMessages(channelId, amount, offset) {
	return (dispatch, getState) => {
		const client = getState().client.client;

		return dispatch({
			type: "GET_CHANNEL_MESSAGES",
			payload: client.messages().getChannelMessages(channelId, amount, offset)
		}).then(async ({ action }) => {
			const messages = action.payload.messages;
			const messagesWithAttachments = messages.filter(message => message.attachmentId);

			// For each messages with attachments, get it's attachment's info
			for (const messageIdx in messagesWithAttachments) {
				await dispatch(getFileInfo(messagesWithAttachments[messageIdx].attachmentId));
			}
		});
	}
}

export function addMessageToChannel(message, channelId) {
	return (dispatch, _) => {
		// If the message has an attachment
		if (message.attachmentId) {
			return dispatch(getFileInfo(message.attachmentId))
				.then(() => dispatch({
					type: "ADD_MESSAGE_TO_CHANNEL",
					payload: {
						message,
						channelId
					}
				}));
		} else {
			return dispatch({
				type: "ADD_MESSAGE_TO_CHANNEL",
				payload: {
					message,
					channelId
				}
			});
		}
	}
}

export function setNewMessageContentForChannel(content, channelId) {
	return {
		type: "SET_NEW_MESSAGE_CONTENT_FOR_CHANNEL",
		payload: {
			content,
			channelId
		}
	}
}

export function setNewMessageAttachmentForChannel(attachmentId, channelId) {
	return {
		type: "SET_NEW_MESSAGE_ATTACHMENT_FOR_CHANNEL",
		payload: {
			attachmentId,
			channelId
		}
	}
}