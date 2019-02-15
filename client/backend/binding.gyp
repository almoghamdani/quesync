{
  "targets": [
    {
      "target_name": "quesync",
      "sources": [
        "<!@(node -p \"require('fs').readdirSync('./src').map(f=>'src/'+f).join(' ')\")",
        "<!@(node -p \"require('fs').readdirSync('../../shared').map(f=>'../../shared/'+f).join(' ')\")"
      ],
      "include_dirs": [ 
        "../../include",
        "<!(node -e \"require('nan')\")"
      ],
      "cflags": [
        "-std=c++17",
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
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1
        }
      },
      "defines": [
          'ASIO_STANDALONE'
      ]
    }
  ]
}