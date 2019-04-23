export default function reducer(
	state = {
		client: null
	},
	action
) {
	switch (action.type) {
		case "CLIENT_OBJECT":
			return { ...state, client: action.payload };

		default:
			return state;
	}
}
