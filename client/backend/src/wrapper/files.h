#pragma once

#include "client.h"
#include "executer.h"
#include "module.h"

namespace quesync {
namespace client {
namespace wrapper {
class files : public Napi::ObjectWrap<files>, public module<files> {
   public:
    static Napi::Function create_constructor_function(Napi::Env env) {
        return DefineClass(env, "Files",
                           {InstanceMethod("startUpload", &files::start_upload),
                            InstanceMethod("startDownload", &files::start_download),
                            InstanceMethod("stopFileTransmission", &files::stop_file_transmission),
                            InstanceMethod("getFileInfo", &files::get_file_info)});
    }

    files(const Napi::CallbackInfo &info) : Napi::ObjectWrap<files>(info), module(info) {}

    Napi::Value start_upload(const Napi::CallbackInfo &info) {
        std::string file_path = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, file_path]() {
            std::shared_ptr<quesync::file> file;

            // Start upload the file
            file = _client->core()->files()->start_upload(file_path);

            return nlohmann::json{{"file", *file}};
        });
    }

    Napi::Value start_download(const Napi::CallbackInfo &info) {
        std::string file_id = info[0].As<Napi::String>(),
                    download_path = info[1].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, file_id, download_path]() {
            // Start download the file
            _client->core()->files()->start_download(file_id, download_path);

            return nlohmann::json{{"fileId", file_id}};
        });
    }

    Napi::Value stop_file_transmission(const Napi::CallbackInfo &info) {
        std::string file_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, file_id]() {
            // Stop the file's transmission
            _client->core()->files()->stop_file_transmission(file_id);

            return nlohmann::json{{"fileId", file_id}};
        });
    }

    Napi::Value get_file_info(const Napi::CallbackInfo &info) {
        std::string file_id = info[0].As<Napi::String>();

        return executer::create_executer(info.Env(), [this, file_id]() {
            std::shared_ptr<quesync::file> file;

            // Get the file's info
            file = _client->core()->files()->get_file_info(file_id);

            return nlohmann::json{{"file", *file}};
        });
    }

    inline static Napi::FunctionReference constructor;
};
};  // namespace wrapper
};  // namespace client
};  // namespace quesync