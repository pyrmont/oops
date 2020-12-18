(declare-project
  :name "Oops"
  :description "Object-oriented programming constructs for Janet"
  :author "Michael Camilleri"
  :license "MIT"
  :url "https://github.com/pyrmont/oops"
  :repo "git+https://github.com/pyrmont/oops"
  :dependencies ["https://github.com/pyrmont/testament"])


(def cflags
  ["-Iutf8.h" "-std=c99" "-Wall" "-Wextra" "-O3"])


(def lflags
  [])


(declare-native
  :name "oops"
  :cflags cflags
  :lflags lflags
  :headers ["src/oops.h"]
  :source ["src/oops/instance.c"
           "src/oops/protocol.c"
           "src/oops/type.c"
           "src/oops.c"])
