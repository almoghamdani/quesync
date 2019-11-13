const INITIAL_STATE = {
	channels: {},
	privateChannels: {}
};

export default function reducer(
	state = INITIAL_STATE,
	action
) {
	switch (action.type) {
		case "GET_PRIVATE_CHANNEL_FULFILLED":
			var channel = { ...action.payload.channel };
			var id = channel.id;

			// Delete the id from the channel
			delete channel.id;

			return { ...state, privateChannels: { ...state.privateChannels, [action.payload.userId]: id }, channels: { ...state.channels, [id]: channel } };

		case "LOGOUT_FULFILLED":
		case "LOGOUT_REJECTED":
		case "CLEAN_CONNECTION":
			return INITIAL_STATE;

		default:
			return state;
	}
}
