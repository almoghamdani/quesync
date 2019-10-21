export function sendMessage(content, channelId, attachmentId=null) {
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
		});
	}
}

export function addMessageToChannel(message, channelId) {
	return {
		type: "ADD_MESSAGE_TO_CHANNEL",
		payload: {
			message,
			channelId
		}
	};
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