export default function reducer(
	state = {
		channelId: null,
		voiceStates: {},
		voiceActivations: {},
		error: null
	},
	action
) {
	switch (action.type) {
		case "CALL_PENDING":
		case "JOIN_CALL_PENDING":
		case "LEAVE_CALL_PENDING":
			return { ...state, error: 0 };

		case "CALL_REJECTED":
		case "JOIN_CALL_REJECTED":
		case "LEAVE_CALL_REJECTED":
			return { ...state, error: action.payload.error };

		case "CALL_FULFILLED":
		case "JOIN_CALL_FULFILLED":
			return {
				...state,
				voiceStates: action.payload.voiceStates,
				channelId: action.payload.channelId
			};

		case "LEAVE_CALL_FULFILLED":
			return {
				...state,
				voiceStates: {},
				voiceActivations: {},
				channelId: null
			};

		case "SET_VOICE_STATE":
			return {
				...state,
				voiceStates: {
					...state.voiceStates,
					[action.payload.userId]: action.payload.voiceState
				}
			};

		case "SET_VOICE_ACTIVATED_STATE":
			return {
				...state,
				voiceActivations: {
					...state.voiceActivations,
					[action.payload.userId]: action.payload.activated
				}
			};

		default:
			return state;
	}
}
