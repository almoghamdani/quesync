{
    "targets": [
        {
            "target_name": "quesync",
            "sources": [
                "<!@(node -p \"require('glob').sync('./src/**/*.cc').join(' ')\")",
                "<!@(node -p \"require('glob').sync('../../shared/**/*.cpp').join(' ')\")",
                "<!@(node -p \"require('glob').sync('../../include/**/*.cpp').join(' ')\")"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "../../include"
            ],
            "cflags": [
                "-std=c++17",
            ],
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            'conditions': [
                ["OS=='win'", {
                    'include_dirs': [
                        'C:/OpenSSL-Win64/include'
                    ],
                    'library_dirs': [
                        'C:/OpenSSL-Win64/lib',
                        '../../lib'
                    ],
                    'link_settings': {
                        'libraries': [
                            '-lVC/libeay32MT',
                            '-lVC/ssleay32MT',
                            '-lopus',
                            '-lOpenAL32',
                            '-lbass'
                        ]
                    },
                    'defines': [
                        '_WIN32_WINNT=0x0501'
                    ],
                    "msvs_settings": {
                        "VCCLCompilerTool": {
                            "ExceptionHandling": 1
                        }
                    },
                }], ["OS=='mac'", {
                    'include_dirs': [
                        '/usr/local/opt/openssl/include'
                    ],
                    'link_settings': {
                        'libraries': [
                            '-lopus',
                            '-lopenal'
                        ],
                        'library_dirs': [
                            '../../../lib',
                            '/usr/local/opt/openal-soft/lib'
                        ],
                    },
                    'xcode_settings': {
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                        'GCC_ENABLE_CPP_RTTI': 'YES',
                        "CLANG_CXX_LIBRARY": "libc++",
                        "MACOSX_DEPLOYMENT_TARGET": "10.7",
                        'OTHER_CFLAGS': [
                            '-std=c++17'
                        ]
                    },
                }]
            ],
            "defines": [
                'ASIO_STANDALONE',
                'QUESYNC_CLIENT'
            ]
        }
    ]
}
