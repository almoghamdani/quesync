{
  "targets": [
    {
      "target_name": "quesync",
      "sources": [ 
        "src/quesync-backend.cc", 
        "src/socket-manager.cc", 
        "src/voice-chat.cc",
        "src/client.cc",
        "src/include/socket-manager.hpp",
        "src/include/socket-error.hpp",
        "src/include/voice-chat.hpp",
        "src/include/client.hpp" ],
      "include_dirs": [ 
        "../../include",
        "<!(node -e \"require('nan')\")"
      ],
      "cflags": [
        "-std=c++17",
        "-stdlib=libc++",
      ],
      "cflags_cc": [
        '-fexceptions'
      ],
      'xcode_settings': {
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'        # -fno-exceptions
      },
      'link_settings': {
        'libraries': [
          '-lopus',
          '-lbass',
          '-lOpenAL32'
        ],
        'library_dirs': [
          '../../lib',
        ],
      },
    }
  ]
}