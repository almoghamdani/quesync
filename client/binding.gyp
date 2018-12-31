{
  "targets": [
    {
      "target_name": "hello",
      "sources": [ 
        "backend/hello.cc", 
        "backend/socket-manager.cc", 
        "backend/voice-chat.cc",
        "backend/client.cc",
        "backend/include/socket-manager.hpp",
        "backend/include/socket-error.hpp",
        "backend/include/voice-chat.hpp",
        "backend/include/client.hpp" ],
      "include_dirs": [ 
        "../include",
        "<!(node -e \"require('nan')\")"
      ],
      "cflags": [
        "-std=c++17",
        "-stdlib=libc++",
      ],
      'link_settings': {
        'libraries': [
          '-lopus',
          '-lbass',
          '-lOpenAL32'
        ],
        'library_dirs': [
          '../lib',
        ],
      },
    }
  ]
}