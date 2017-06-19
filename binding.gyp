 {
    "targets": [
      {
        "target_name": "osx-keylogger",
        "cflags": ["-Wall", "-std=c++11"],
        "sources": [ "osx-keylogger.cc" ],
        "conditions": [
            ["OS=='mac'", {
              "sources": [ "osx-keylogger.cc" ],
              "cflags": ["-Wall", "-std=c++11", "-framework", "coreFoundation", "-framework", "IOKit"]
            }]
          ],
        "include_dirs" : [
          "<!(node -e \"require('nan')\")"
        ]
      }
    ]
  }
