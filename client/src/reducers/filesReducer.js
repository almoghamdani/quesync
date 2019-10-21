const INITIAL_STATE = {
	files: {},
	filesProgress: {}
};

export default function reducer(
	state = INITIAL_STATE,
	action
) {
	switch (action.type) {
		case "FILE_START_UPLOAD_FULFILLED":
			{
				const file = { ...action.payload.file };
				const fileId = file.id;

				// Remove the id from the file object
				delete file.id;

				return {
					...state,
					files: { ...state.files, [fileId]: file },
					filesProgress: {
						...state.filesProgress,
						[fileId]: state.filesProgress[fileId] ? state.filesProgress[fileId] : 0
					}
				};
			}


		case "FILE_TRANSMISSION_PROGRESS_UPDATE":
			{
				const { fileId, progress } = action.payload;

				return {
					...state,
					filesProgress: { ...state.filesProgress, [fileId]: progress }
				};
			}

		case "LOGOUT_FULFILLED":
		case "LOGOUT_REJECTED":
		case "CLEAN_CONNECTION":
			return INITIAL_STATE;

		default:
			return state;
	}
}
