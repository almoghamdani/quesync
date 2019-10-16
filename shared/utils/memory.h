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

    template <typename T>
    static std::string merge_buffers(std::vector<std::shared_ptr<T>> vec, int buf_size) {
        std::shared_ptr<unsigned char> buf(new unsigned char[vec.size() * buf_size]);

        // Copy the buffers to the buffer
        for (int i = 0; i < vec.size(); i++) 
        {
            memcpy(buf.get() + i * buf_size, vec[i].get(), buf_size);
        }

        return std::string((char *)buf.get(), vec.size() * buf_size);
    }
};
};  // namespace utils
};  // namespace quesync