const messagesSort = (a, b) => {
	if (a.sentAt < b.sentAt)
		return -1;
	else if (a.sentAt > b.sentAt)
		return 1;
	else
		return 0;
}

const INITIAL_STATE = {
	messages: {},
	newMessages: {},
	error: null
};

export default function reducer(
	state = INITIAL_STATE,
	action
) {
	switch (action.type) {
		case "SEND_MESSAGE_PENDING":
		case "GET_CHANNEL_MESSAGES_PENDING":
			return { ...state, error: null };

		case "SEND_MESSAGE_REJECTED":
		case "GET_CHANNEL_MESSAGES_REJECTED":
			return { ...state, error: action.payload.error };

		case "SEND_MESSAGE_FULFILLED":
			var message = { ...action.payload.message };
			const channelId = message.channelId;

			const oldChannelMessages = state.messages[channelId] ? state.messages[channelId] : [];
			let channelMessages = [...oldChannelMessages];

			// Delete the channel id from the message
			delete message.channelId;

			// Add the message to the array
			channelMessages.push(message);

			// Sort the channel messages array
			channelMessages.sort(messagesSort);

			return { ...state, messages: { ...state.messages, [channelId]: channelMessages }, newMessages: { ...state.newMessages, [channelId]: null } };

		case "GET_CHANNEL_MESSAGES_FULFILLED":
			{
				let messages = [...action.payload.messages];

				const oldChannelMessages = state.messages[action.payload.channelId] ? state.messages[action.payload.channelId] : [];
				let channelMessages = [...oldChannelMessages];

				// For each message
				messages.forEach(message => {
					let messageCopy = { ...message }

					// Remove the channel id from the message
					delete messageCopy.channelId;

					// Add the message to the array of messages
					channelMessages.push(messageCopy);
				})

				// Sort the channel messages array
				channelMessages.sort(messagesSort);

				return { ...state, messages: { ...state.messages, [action.payload.channelId]: channelMessages } };
			}
		case "ADD_MESSAGE_TO_CHANNEL":
			{
				const oldChannelMessages = state.messages[action.payload.channelId] ? state.messages[action.payload.channelId] : [];
				let channelMessages = [...oldChannelMessages];

				// Add the message to the array
				channelMessages.push(action.payload.message);

				// Sort the channel messages array
				channelMessages.sort(messagesSort);

				return { ...state, messages: { ...state.messages, [action.payload.channelId]: channelMessages } };
			}

		case "SET_NEW_MESSAGE_FOR_CHANNEL":
			return { ...state, newMessages: { ...state.newMessages, [action.payload.channelId]: action.payload.messageContent } }

		case "LOGOUT_FULFILLED":
		case "LOGOUT_REJECTED":
			return INITIAL_STATE;

		default:
			return state;
	}
}
