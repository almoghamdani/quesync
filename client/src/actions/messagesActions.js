export function sendMessage(client, content, channelId) {
	return {
		type: "SEND_MESSAGE",
		payload: client.messages().sendMessage(content, channelId)
	};
}

export function getChannelMessages(client, channelId, amount, offset) {
	return {
		type: "GET_CHANNEL_MESSAGES",
		payload: client.messages().getChannelMessages(channelId, amount, offset)
	};
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

export function setNewMessageForChannel(messageContent, channelId) {
	return {
		type: "SET_NEW_MESSAGE_FOR_CHANNEL",
		payload: {
			messageContent,
			channelId
		}
	}
}