 {
    "targets": [
      {
        "target_name": "osx-keylogger",
        "cflags": ["-Wall", "-std=c++11", "-framework", "coreFoundation", "-framework", "IOKit"],
        "conditions": [
            ["OS=='mac'", {
              "sources": [ "osx-keylogger.cc" ]
            }]
          ],
        "include_dirs" : [
          "<!(node -e \"require('nan')\")"
        ]
      }
    ]
  }
