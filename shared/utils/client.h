#pragma once

#ifdef QUESYNC_CLIENT
#include <napi.h>
#include <uv.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace quesync {
namespace utils {
class client {
   public:
    static Napi::Object json_to_object(Napi::Env env, nlohmann::json json);

    template <typename T, typename R>
    static std::vector<T> array_to_native(Napi::Array arr) {
        std::vector<T> items;

        for (int i = 0; i < arr.Length(); i++) {
            items.push_back(arr.Get(i).As<R>());
        }

        return items;
    };
};
};  // namespace utils
};  // namespace quesync
#endif