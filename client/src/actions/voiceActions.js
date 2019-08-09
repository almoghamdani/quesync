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

export function leaveCall(client) {
	return {
		type: "LEAVE_CALL",
		payload: client.leaveCall()
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

export function setVoiceActivationState(userId, activated) {
	return {
		type: "SET_VOICE_ACTIVATED_STATE",
		payload: {
			userId,
			activated
		}
	};
}
