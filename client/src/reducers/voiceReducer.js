export default function reducer(
	state = {
		channelId: null,
		activeCalls: [],
		voiceStates: {},
		voiceActivations: {},
		muted: false,
		deafen: false,
		error: null
	},
	action
) {
	switch (action.type) {
		case "CALL_PENDING":
		case "JOIN_CALL_PENDING":
		case "LEAVE_CALL_PENDING":
		case "SET_VOICE_STATE_PENDING":
			return { ...state, error: 0 };

		case "CALL_REJECTED":
		case "JOIN_CALL_REJECTED":
		case "LEAVE_CALL_REJECTED":
		case "SET_VOICE_STATE_REJECTED":
			return { ...state, error: action.payload.error };

		case "CALL_FULFILLED":
			const callDetails = action.payload.callDetails;

			return {
				...state,
				activeCalls: [...state.activeCalls, action.payload.channelId],
				voiceStates: callDetails.voiceStates,
				channelId: action.payload.channelId
			};

		case "JOIN_CALL_FULFILLED":
			return {
				...state,
				voiceStates: action.payload.voiceStates,
				channelId: action.payload.channelId
			};

		case "LEAVE_CALL_FULFILLED":
			let activeCalls = state.activeCalls;
			const connectedUsers = Object.keys(state.voiceStates).filter(
				user => state.voiceStates[user].state === 1
			);

			// If no one is connected to the channel, remove the channel as an active channel
			if (!connectedUsers.length) {
				activeCalls = [...state.activeCalls].filter(
					channel => channel !== state.channelId
				);
			}

			return {
				...state,
				voiceStates: {},
				voiceActivations: {},
				activeCalls,
				channelId: null
			};

		case "UPDATE_VOICE_STATE":
			return {
				...state,
				voiceStates: {
					...state.voiceStates,
					[action.payload.userId]: action.payload.voiceState
				}
			};

		case "UPDATE_VOICE_ACTIVATED_STATE":
			return {
				...state,
				voiceActivations: {
					...state.voiceActivations,
					[action.payload.userId]: action.payload.activated
				}
			};

		case "SET_VOICE_STATE_FULFILLED":
			return {
				...state,
				muted: action.payload.voiceState.muted,
				deafen: action.payload.voiceState.deafen
			};

		case "SET_ACTIVE_CALL":
			return {
				...state,
				activeCalls: [...state.activeCalls, action.payload]
			};

		case "REMOVE_ACTIVE_CALL": {
			const activeCalls = [...state.activeCalls].filter(
				channel => channel !== action.payload
			);

			return {
				...state,
				activeCalls: activeCalls
			};
		}

		default:
			return state;
	}
}
