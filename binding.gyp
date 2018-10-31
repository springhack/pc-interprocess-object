{
  'targets': [
    {
      'target_name': 'interprocess-object',
      'cflags_cc!':[
        '-fno-exceptions',
        '-fno-rtti',
      ],
      'sources': [
        'include/xpcpp/sync/sync_mutex.cpp',
        'include/xpcpp/sync/sync_util.cpp',
        'src/tlsf.c',
        'src/md5.cpp',
        'src/main.cc'
      ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")',
        './include'
      ],
      'conditions': [
        ['OS=="win"', {
          'defines': [
            '_HAS_EXCEPTIONS=1'
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1
            }
          }
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'GCC_ENABLE_CPP_RTTI': 'YES',
            'OTHER_CFLAGS': [
              '-Wall',
              '-Wno-missing-braces',
              '-Wno-format-security',
              '-std=c++11',
              '-ObjC++',
              '-mmacosx-version-min=10.7'
            ]
          },
          'libraries': [
              '-lobjc'
          ]
        }]
      ]
    }
  ]
}
