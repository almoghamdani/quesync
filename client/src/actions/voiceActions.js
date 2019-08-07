export function call(client, channelId) {
	return {
		type: "CALL",
		payload: client.call(channelId)
	};
}

export function joinCall(client, channelId) {
	return {
		type: "JOIN_CALL",
		payload: client.joinCall(channelId)
	};
}

export function setVoiceState(userId, voiceState) {
	return {
		type: "SET_VOICE_STATE",
		payload: {
			userId,
			voiceState
		}
	};
}
