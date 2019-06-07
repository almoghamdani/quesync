const MAX_PINGS_IN_LIST = 100;

export default function reducer(
	state = {
		client: null,
		pings: []
	},
	action
) {
	switch (action.type) {
		case "CLIENT_OBJECT":
			return { ...state, client: action.payload };

		case "ADD_PING_VALUE":
			var pingsCopy = [...state.pings, action.payload];

			return {
				...state,
				pings: pingsCopy.slice(pingsCopy.length > MAX_PINGS_IN_LIST ? pingsCopy.length - MAX_PINGS_IN_LIST : 0)
			};

		default:
			return state;
	}
}
