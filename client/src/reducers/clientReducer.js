const MAX_PINGS_IN_LIST = 10;

export default function reducer(
	state = {
		client: null,
		connected: false,
		pings: []
	},
	action
) {
	switch (action.type) {
		case "CLIENT_OBJECT":
			return { ...state, client: action.payload };

		case "ADD_PING_VALUE":
			var pingsCopy = [...state.pings, action.payload];

			// If reached max pings, reset list
			if (pingsCopy.length > MAX_PINGS_IN_LIST) {
				return {
					...state,
					pings: [state.pings.slice(-1)[0], action.payload]
				};
			}

			return {
				...state,
				pings: pingsCopy
			};

		case "CONNECTED":
			return { ...state, connected: true };

		case "DISCONNECTED":
			return { ...state, connected: false };

		default:
			return state;
	}
}
