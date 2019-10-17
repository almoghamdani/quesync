const callsSort = (a, b) => {
	if (a.startDate < b.startDate)
		return -1;
	else if (a.startDate > b.startDate)
		return 1;
	else
		return 0;
}


export default function reducer(
	state = {
		channelId: null,
		callJoinDate: null,
		calls: {},
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
		case "GET_CHANNEL_CALLS_PENDING":
			return { ...state, error: 0 };

		case "CALL_REJECTED":
		case "JOIN_CALL_REJECTED":
		case "LEAVE_CALL_REJECTED":
		case "SET_VOICE_STATE_REJECTED":
		case "GET_CHANNEL_CALLS_REJECTED":
			return { ...state, error: action.payload.error };

		case "CALL_FULFILLED":
			{
				const callDetails = action.payload.callDetails;

				var call = { ...callDetails.call };
				const channelId = call.channelId;

				const oldChannelCalls = state.calls[channelId] ? state.calls[channelId] : [];
				let channelCalls = [...oldChannelCalls];

				// Delete the channel id from the call
				delete call.channelId;

				// Add the call to the array
				channelCalls.push(call)

				// Sort the channel calls array
				channelCalls.sort(callsSort)

				return {
					...state,
					calls: { ...state.calls, [channelId]: channelCalls },
					activeCalls: [...state.activeCalls, action.payload.channelId],
					voiceStates: callDetails.voiceStates,
					channelId: action.payload.channelId,
					callJoinDate: Math.floor(Date.now() / 1000) - 1
				};
			}


		case "JOIN_CALL_FULFILLED":
			{
				const channelId = action.payload.channelId;

				const oldChannelCalls = state.calls[channelId] ? state.calls[channelId] : [];
				let channelCalls = [...oldChannelCalls];

				// Set the call as joined
				channelCalls[channelCalls.length - 1].joined = true;

				return {
					...state,
					calls: { ...state.calls, [channelId]: channelCalls },
					voiceStates: action.payload.voiceStates,
					channelId,
					callJoinDate: Math.floor(Date.now() / 1000) - 1
				};
			}


		case "LEAVE_CALL_FULFILLED":
			const channelId = state.channelId;

			const oldChannelCalls = state.calls[channelId] ? state.calls[channelId] : [];
			let channelCalls = [...oldChannelCalls];

			let activeCalls = state.activeCalls;
			const connectedUsers = Object.keys(state.voiceStates).filter(
				user => state.voiceStates[user].state === 1
			);

			// If no one is connected to the channel, remove the channel as an active channel
			if (!connectedUsers.length) {
				// Set the end date for the call
				channelCalls[channelCalls.length - 1].endDate = Math.floor(Date.now() / 1000);

				// Remove the channel as an active call
				activeCalls = [...state.activeCalls].filter(
					channel => channel !== state.channelId
				);
			}

			return {
				...state,
				calls: { ...state.calls, [channelId]: channelCalls },
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
			{
				const channelId = action.payload;

				return {
					...state,
					activeCalls: [...state.activeCalls, channelId]
				};
			}

		case "REMOVE_ACTIVE_CALL": {
			const activeCalls = [...state.activeCalls].filter(
				channel => channel !== action.payload
			);

			const channelId = action.payload;

			const oldChannelCalls = state.calls[channelId] ? state.calls[channelId] : [];
			let channelCalls = [...oldChannelCalls];

			// Set the end date of the call
			channelCalls[channelCalls.length - 1].endDate = Math.floor(Date.now() / 1000);

			return {
				...state,
				calls: { ...state.calls, [channelId]: channelCalls },
				activeCalls: activeCalls
			};
		}

		case "GET_CHANNEL_CALLS_FULFILLED":
			{
				const channelId = action.payload.channelId;

				return {
					...state,
					calls: { ...state.calls, [channelId]: action.payload.calls }
				}
			}

		default:
			return state;
	}
}
