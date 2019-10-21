const INITIAL_STATE = {
	files: {},
	filesProgress: {},
	downloading: []
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

		case "FILE_START_DOWNLOAD_FULFILLED":
			{
				const { fileId } = action.payload;

				return {
					...state,
					downloading: [...state.downloading, fileId],
					filesProgress: {
						...state.filesProgress,
						[fileId]: state.filesProgress[fileId] ? state.filesProgress[fileId] : 0
					}
				}
			}

		case "STOP_FILE_TRANSMISSION_FULFILLED": {
			const { fileId } = action.payload;
			
			let downloading = [...state.downloading];

			// If the file is currently downloading, remove it from the downloads's list
			if (downloading.includes(fileId)) {
				downloading = downloading.filter(id => id !== fileId);
			}

			return {
				...state,
				downloading
			}
		}

		case "FILE_TRANSMISSION_PROGRESS_UPDATE":
			{
				const { fileId, progress } = action.payload;
				const file = state.files[fileId];

				let downloading = [...state.downloading];

				// If the file is being downloaded and the download is finished remove it as been downloaded
				if (downloading.includes(fileId) && file && file.size === progress) {
					downloading = downloading.filter(id => id !== fileId);
				}

				return {
					...state,
					downloading,
					filesProgress: { ...state.filesProgress, [fileId]: progress }
				};
			}

		case "GET_FILE_INFO_FULFILLED":
			{
				const file = { ...action.payload.file };
				const fileId = file.id;

				// Delete the file's id
				delete file.id;

				return {
					...state,
					files: { ...state.files, [fileId]: file }
				}

			}

		case "LOGOUT_FULFILLED":
		case "LOGOUT_REJECTED":
		case "CLEAN_CONNECTION":
			return INITIAL_STATE;

		default:
			return state;
	}
}
