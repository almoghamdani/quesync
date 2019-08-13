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

export function updateVoiceState(userId, voiceState) {
	return {
		type: "UPDATE_VOICE_STATE",
		payload: {
			userId,
			voiceState
		}
	};
}

export function updateVoiceActivationState(userId, activated) {
	return {
		type: "UPDATE_VOICE_ACTIVATED_STATE",
		payload: {
			userId,
			activated
		}
	};
}

export function setVoiceState(client, mute, deafen) {
	return {
		type: "SET_VOICE_STATE",
		payload: client.setVoiceState(mute, deafen)
	};
}

export function setActiveCall(channelId) {
	return {
		type: "SET_ACTIVE_CALL",
		payload: channelId
	};
}

export function removeActiveCall(channelId) {
	return {
		type: "REMOVE_ACTIVE_CALL",
		payload: channelId
	};
}
