
#from https://stackoverflow.com/questions/714100/os-detecting-makefile
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OPUSENC = ../../nest-libs/linux/opus-tools/bin/opusenc
else
	OPUSENC = ../../nest-libs/macos/opus-tools/bin/opusenc
endif
../dist/cold-dunes.opus : cold-dunes.wav
	$(OPUSENC) --vbr --bitrate 128 cold-dunes.wav ../dist/cold-dunes.opus
