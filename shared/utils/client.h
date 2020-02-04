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
    /**
     * Converts nlohmann::json to Napi::Object.
     *
     * @param env The current environment.
     * @param json The json to be converted.
     * @return An object with the data from the json.
     */
    static Napi::Object json_to_object(Napi::Env env, nlohmann::json json);

    /**
     * Converts Napi::Array to native vector.
     *
     * @tparam T The native type of the elements.
     * @tparam R The Napi type of elements.
     * @param arr The array to be converted.
     * @return The vector with the native elements.
     */
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