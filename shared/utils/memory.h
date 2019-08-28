#pragma once

#include <memory>
#include <string>

namespace quesync {
namespace utils {
class memory {
   public:
    template <typename T>
    static std::shared_ptr<T> convert_to_buffer(std::string data) {
        std::shared_ptr<T> buf = std::shared_ptr<T>(new T[data.length()]);

        // Copy the data to the buffer
        memcpy(buf.get(), data.data(), data.length());

        return buf;
    }
};
};  // namespace utils
};  // namespace quesync