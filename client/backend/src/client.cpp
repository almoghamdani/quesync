#include "client.h"

#include "socket-manager.h"
#include "executer.h"
#include "event-worker.h"

#include "../../../shared/utils.h"
#include "../../../shared/packets/login_packet.h"
#include "../../../shared/packets/register_packet.h"
#include "../../../shared/packets/error_packet.h"
#include "../../../shared/packets/search_packet.h"
#include "../../../shared/packets/friend_request_packet.h"
#include "../../../shared/packets/ping_packet.h"
#include "../../../shared/packets/profile_packet.h"
#include "../../../shared/packets/get_private_channel_packet.h"
#include "../../../shared/packets/send_message_packet.h"
#include "../../../shared/packets/get_channel_messages_packet.h"
#include "../../../shared/packets/friendship_status_packet.h"
#include "../../../shared/packets/session_auth_packet.h"
#include "../../../shared/packets/call_request_packet.h"
#include "../../../shared/packets/join_call_request_packet.h"
#include "../../../shared/packets/leave_call_packet.h"

Napi::FunctionReference Client::constructor;

Client::Client(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Client>(info),
												 _communicator(std::bind(&Client::clean, this)),
												 _user(nullptr),
												 _voice_chat(nullptr)
{
	Napi::Env env = info.Env();
}

Client::~Client()
{
	clean();
}

Communicator &Client::communicator()
{
	return _communicator;
}

void Client::clean()
{
	// Clean user object
	if (_user)
	{
		delete _user;
		_user = nullptr;
	}
}

Napi::Value Client::connect(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert first parameter to string
	std::string ip = info[0].As<Napi::String>();

	// Create a new executer worker that will make the connection to the server
	Executer *e = new Executer([this, ip]() {
		try
		{
			// Try to connect to the server
			_communicator.connect(ip);
		}
		catch (QuesyncException &ex)
		{
			return nlohmann::json{{"error", ex.getErrorCode()}};
		}
		catch (...)
		{
			return nlohmann::json{{"error", UNKNOWN_ERROR}};
		}

		// Initialize voice chat
		_voice_chat = std::make_shared<VoiceChat>(this, ip.c_str());
		_voice_chat->init();

		return nlohmann::json{{"error", SUCCESS}};
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	// Return the promise
	return deferred.Promise();
}

Napi::Value Client::login(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to satring
	std::string username = info[0].As<Napi::String>(), password = info[1].As<Napi::String>();

	Executer *e = new Executer([this, username, password]() {
		QuesyncError error = SUCCESS;

		std::shared_ptr<ResponsePacket> response_packet;

		nlohmann::json server_res;
		nlohmann::json res;

		// Create a login packet from the username and password
		LoginPacket login_packet(username, password);

		// If already authenticated, return error
		if (_user)
		{
			res["error"] = ALREADY_AUTHENTICATED;

			return res;
		}

		// Send the login packet
		try
		{
			response_packet = _communicator.send(&login_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		else if (response_packet && response_packet->type() == AUTHENTICATED_PACKET) // If the response packet is a valid authentication response, get the user from it
		{
			// Parse the server response
			server_res = nlohmann::json::parse(response_packet->data());

			// Create a new user
			_user = new User();

			// Set the user data
			_user->set(server_res["user"]);

			// Set success error code
			res["error"] = SUCCESS;

			// Return the user serialized
			res["user"] = _user->json();

			// Return the session id
			res["sessionId"] = server_res["sessionId"];
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::signup(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to string
	std::string username = info[0].As<Napi::String>(), password = info[1].As<Napi::String>(),
				email = info[2].As<Napi::String>();

	Executer *e = new Executer([this, username, password, email]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;

		nlohmann::json server_res;
		nlohmann::json res;

		// Create a login packet from the username and password
		RegisterPacket register_packet(username, password, email, username);

		// If already authenticated, return error
		if (_user)
		{
			res["error"] = ALREADY_AUTHENTICATED;

			return res;
		}

		// Send the register packet
		try
		{
			response_packet = _communicator.send(&register_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		else if (response_packet && response_packet->type() == AUTHENTICATED_PACKET) // If the response packet is a valid authentication response, get the user from it
		{
			// Parse the server response
			server_res = nlohmann::json::parse(response_packet->data());

			// Create a new user
			_user = new User();

			// Set the user data
			_user->set(server_res["user"]);

			// Set success error code
			res["error"] = SUCCESS;

			// Return the user serialized
			res["user"] = _user->json();

			// Return the session id
			res["sessionId"] = server_res["sessionId"];
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::sessionAuth(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to satring
	std::string session_id = info[0].As<Napi::String>();

	Executer *e = new Executer([this, session_id]() {
		QuesyncError error = SUCCESS;

		std::shared_ptr<ResponsePacket> response_packet;

		nlohmann::json server_res;
		nlohmann::json res;

		// Create a session auth packet from the session
		SessionAuthPacket session_auth_packet(session_id);

		// If already authenticated, return error
		if (_user)
		{
			res["error"] = ALREADY_AUTHENTICATED;

			return res;
		}

		// Send the session auth packet
		try
		{
			response_packet = _communicator.send(&session_auth_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		else if (response_packet && response_packet->type() == AUTHENTICATED_PACKET) // If the response packet is a valid authentication response, get the user from it
		{
			// Parse the server response
			server_res = nlohmann::json::parse(response_packet->data());

			// Create a new user
			_user = new User();

			// Set the user data
			_user->set(server_res["user"]);

			// Set success error code
			res["error"] = SUCCESS;

			// Return the user serialized
			res["user"] = _user->json();
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::getUserProfile(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to satring
	std::string user_id = info[0].As<Napi::String>();

	Executer *e = new Executer([this, user_id]() {
		QuesyncError error = SUCCESS;

		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		// Create a profile packet from the user id
		ProfilePacket profile_packet(user_id);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the profile packet
		try
		{
			response_packet = _communicator.send(&profile_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a valid user's profile packet
		else if (response_packet && response_packet->type() == PROFILE_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Return the profile serialized
			res["profile"] = nlohmann::json::parse(response_packet->data());
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::search(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to string
	std::string nickname = info[0].As<Napi::String>();
	int tag = info[1].IsUndefined() ? -1 : info[1].As<Napi::Number>();

	Executer *e = new Executer([this, nickname, tag]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		SearchPacket search_packet(nickname, tag);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the search packet
		try
		{
			response_packet = _communicator.send(&search_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a valid search results response, get the search results from it
		else if (response_packet && response_packet->type() == SEARCH_RESULTS_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Return the search results in an object
			res["searchResults"] = nlohmann::json::parse(response_packet->data());
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::sendFriendRequest(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to string
	std::string friend_id = info[0].As<Napi::String>();

	Executer *e = new Executer([this, friend_id]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		FriendRequestPacket friend_request_packet(friend_id);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the friend request packet
		try
		{
			response_packet = _communicator.send(&friend_request_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a friend request confirmation response
		else if (response_packet && response_packet->type() == FRIEND_REQUEST_SENT_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			res["friendId"] = friend_id;
			res["sentAt"] = std::time(nullptr);
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::getUser(const Napi::CallbackInfo &info)
{
	return _user ? Utils::JsonToObject(info.Env(), _user->json()) : info.Env().Null();
}

Napi::Value Client::registerEventHandler(const Napi::CallbackInfo &info)
{
	std::string event_name = info[0].As<Napi::String>();
	Napi::Function event_handle_function = info[1].As<Napi::Function>();

	// Create an event worker with the event handle function
	std::shared_ptr<EventWorker> eventWorker(new EventWorker(event_handle_function));
	eventWorker->SuppressDestruct(); // Prevent from the event worker to destruct when calling OnOK

	// Register an event handler that will create an event worker
	_communicator
		.eventHandler()
		.registerEventHandler(event_name, [eventWorker](nlohmann::json &event_data) {
			// Set the event data
			eventWorker->setEventData(event_data);

			// Queue the event worker
			eventWorker->Queue();
		});

	return info.Env().Null();
}

Napi::Value Client::getPrivateChannel(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to string
	std::string user_id = info[0].As<Napi::String>();

	Executer *e = new Executer([this, user_id]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		GetPrivateChannelPacket get_private_channel_packet(user_id);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the get private channel packet
		try
		{
			response_packet = _communicator.send(&get_private_channel_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a private channel response, get the channel from it
		else if (response_packet && response_packet->type() == PRIVATE_CHANNEL_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Get the channel from the response's data
			res["channel"] = nlohmann::json::parse(response_packet->data());
			res["userId"] = user_id;
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::sendMessage(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to string
	std::string content = info[0].As<Napi::String>();
	std::string channel_id = info[1].As<Napi::String>();

	Executer *e = new Executer([this, content, channel_id]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		SendMessagePacket send_message_packet(content, channel_id);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the send message packet
		try
		{
			response_packet = _communicator.send(&send_message_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a message id response, get the message id from it
		else if (response_packet && response_packet->type() == MESSAGE_ID_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Get the message id from the resposne's data and assemble a message
			res["message"] = nlohmann::json{
				{"id", nlohmann::json::parse(response_packet->data())["messageId"]},
				{"channelId", channel_id},
				{"senderId", _user->id()},
				{"content", content},
				{"sentAt", std::time(nullptr)}};
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::getChannelMessages(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to string
	std::string channel_id = info[0].As<Napi::String>();
	unsigned int amount = info[1].As<Napi::Number>();
	unsigned int offset = info[2].As<Napi::Number>();

	Executer *e = new Executer([this, channel_id, amount, offset]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		GetChannelMessagesPacket get_channel_messages_packet(channel_id, amount, offset);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the get channel messages packet
		try
		{
			response_packet = _communicator.send(&get_channel_messages_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a channel messages packet, get the messages from it
		else if (response_packet && response_packet->type() == CHANNEL_MESSAGES_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Get the messages from the resposne's data
			res["messages"] = nlohmann::json::parse(response_packet->data());
			res["channelId"] = channel_id;
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::call(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to c++ objects
	std::string channel_id = info[0].As<Napi::String>();

	Executer *e = new Executer([this, channel_id]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res, voice_res;

		CallRequestPacket call_request_packet(channel_id);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the call request packet
		try
		{
			response_packet = _communicator.send(&call_request_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a call started packet, init voice chat
		else if (response_packet && response_packet->type() == CALL_STARTED_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Get the voice info
			voice_res = nlohmann::json::parse(response_packet->data());

			// Enable voice chat
			_voice_chat->enable(_user->id(), voice_res["voiceSessionId"], channel_id);

			// Get the voice states in the voice channel and the channel id
			res["voiceStates"] = voice_res["voiceStates"];
			res["channelId"] = channel_id;
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::joinCall(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to c++ objects
	std::string channel_id = info[0].As<Napi::String>();

	Executer *e = new Executer([this, channel_id]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res, voice_res;

		JoinCallRequestPacket join_call_request_packet(channel_id);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the join call request packet
		try
		{
			response_packet = _communicator.send(&join_call_request_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a join call approved packet, init voice chat
		else if (response_packet && response_packet->type() == JOIN_CALL_APPROVED_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Get the voice info
			voice_res = nlohmann::json::parse(response_packet->data());

			// Enable voice chat
			_voice_chat->enable(_user->id(), voice_res["voiceSessionId"], channel_id);

			// Get the voice states in the voice channel and the channel id
			res["voiceStates"] = voice_res["voiceStates"];
			res["channelId"] = channel_id;
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::leaveCall(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	Executer *e = new Executer([this]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		LeaveCallPacket leave_call_packet;

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the leave call packet
		try
		{
			response_packet = _communicator.send(&leave_call_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a join call approved packet, init voice chat
		else if (response_packet && response_packet->type() == CALL_LEFT_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			// Disable the voice chat
			_voice_chat->disable();
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Value Client::setFriendshipStatus(const Napi::CallbackInfo &info)
{

	Napi::Env env = info.Env();
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	// Convert parameters to string
	std::string friend_id = info[0].As<Napi::String>();
	bool status = info[1].As<Napi::Boolean>();

	Executer *e = new Executer([this, friend_id, status]() {
		QuesyncError error = SUCCESS;
		std::shared_ptr<ResponsePacket> response_packet;
		nlohmann::json res;

		FriendshipStatusPacket friendship_status_packet(friend_id, status);

		// If not authenticated, return error
		if (!_user)
		{
			res["error"] = NOT_AUTHENTICATED;

			return res;
		}

		// Send the friendship status packet
		try
		{
			response_packet = _communicator.send(&friendship_status_packet);
		}
		catch (QuesyncException &ex)
		{
			res["error"] = ex.getErrorCode();

			return res;
		}
		catch (...)
		{
			res["error"] = UNKNOWN_ERROR;

			return res;
		}

		// If the response is an error, handle the error
		if (response_packet && response_packet->type() == ERROR_PACKET)
		{
			// Set the error code from the response packet
			res["error"] = std::static_pointer_cast<ErrorPacket>(response_packet)->error();
		}
		// If the response packet is a friendship status set packet, return success
		else if (response_packet && response_packet->type() == FRIENDSHIP_STATUS_SET_PACKET)
		{
			// Set success error code
			res["error"] = SUCCESS;

			res["friendId"] = friend_id;
		}
		else if (error)
		{
			// If an error occurred, return it
			res["error"] = error;
		}
		else
		{
			// We shouldn't get here
			res["error"] = UNKNOWN_ERROR;
		}

		return res;
	},
							   deferred);

	// Queue the executer worker
	e->Queue();

	return deferred.Promise();
}

Napi::Object Client::Init(Napi::Env env, Napi::Object exports)
{
	// Create scope for Client object
	Napi::HandleScope scope(env);

	// This method is used to hook the accessor and method callbacks
	Napi::Function func = DefineClass(env,
									  "Client",
									  {InstanceMethod("connect", &Client::connect),
									   InstanceMethod("login", &Client::login),
									   InstanceMethod("sessionAuth", &Client::sessionAuth),
									   InstanceMethod("register", &Client::signup),
									   InstanceMethod("getUserProfile", &Client::getUserProfile),
									   InstanceMethod("search", &Client::search),
									   InstanceMethod("sendFriendRequest", &Client::sendFriendRequest),
									   InstanceMethod("registerEventHandler", &Client::registerEventHandler),
									   InstanceMethod("getUser", &Client::getUser),
									   InstanceMethod("getPrivateChannel", &Client::getPrivateChannel),
									   InstanceMethod("sendMessage", &Client::sendMessage),
									   InstanceMethod("getChannelMessages", &Client::getChannelMessages),
									   InstanceMethod("setFriendshipStatus", &Client::setFriendshipStatus),
									   InstanceMethod("call", &Client::call),
									   InstanceMethod("joinCall", &Client::joinCall),
									   InstanceMethod("leaveCall", &Client::leaveCall)});

	// Create a peristent reference to the class constructor. This will allow
	// a function called on a class prototype and a function
	// called on instance of a class to be distinguished from each other.
	constructor = Napi::Persistent(func);

	// Call the SuppressDestruct() method on the static data prevent the calling
	// to this destructor to reset the reference when the environment is no longer
	// available.
	constructor.SuppressDestruct();

	exports.Set("Client", func);
	return exports;
}