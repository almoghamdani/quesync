{
  "targets": [
    {
      "target_name": "quesync",
      "sources": [
        "<!@(node -p \"require('fs').readdirSync('./src').map(f=>'src/'+f).join(' ')\")",
        "<!@(node -p \"require('fs').readdirSync('../../shared').map(f=>'../../shared/'+f).join(' ')\")"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")", 
        "../../include",
      ],
      "cflags": [
        "-std=c++17",
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      'xcode_settings': {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7",
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
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1
        }
      },
      "defines": [
          'ASIO_STANDALONE',
          'QUESYNC_CLIENT'
      ]
    }
  ]
}