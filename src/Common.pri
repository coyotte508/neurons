# Common shadow build directory for all builds
CONFIG(shadow)|!equals($${_PRO_FILE_PWD_}, $${OUT_PWD}) {
   CONFIG(debug, debug|release) {
      OBJECTS_DIR=$$PWD/../build/debug/$$dirprefix$$basename(_PRO_FILE_PWD_)
   }
   CONFIG(release, debug|release) {
      OBJECTS_DIR=$$PWD/../build/release/$$dirprefix$$basename(_PRO_FILE_PWD_)
   }
   message("Shadow build enabled. Obj dir" $$OBJECTS_DIR)
} else {
   message("No shadow build")
}

bin = $$PWD/../bin

DESTDIR=$$bin
LIBS += -L$$bin
