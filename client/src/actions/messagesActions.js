export function sendMessage(client, content, channelId) {
	return {
		type: "SEND_MESSAGE",
		payload: client.sendMessage(content, channelId)
	};
}

export function getChannelMessages(client, channelId, amount, offset) {
	return {
		type: "GET_CHANNEL_MESSAGES",
		payload: client.getChannelMessages(channelId, amount, offset)
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