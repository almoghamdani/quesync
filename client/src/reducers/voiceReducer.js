export default function reducer(
	state = {
		channelId: null,
		voiceStates: {},
		error: null
	},
	action
) {
	switch (action.type) {
		case "CALL_PENDING":
		case "JOIN_CALL_PENDING":
			return { ...state, error: 0 };

		case "CALL_REJECTED":
		case "JOIN_CALL_REJECTED":
			return { ...state, error: action.payload.error };

		case "CALL_FULFILLED":
		case "JOIN_CALL_FULFILLED":
			return {
				...state,
				voiceStates: action.payload.voiceStates,
				channelId: action.payload.channelId
			};

		case "SET_VOICE_STATE":
			return {
				...state,
				voiceStates: {
					...state.voiceStates,
					[action.payload.userId]: action.payload.voiceState
				}
			};

		default:
			return state;
	}
}
