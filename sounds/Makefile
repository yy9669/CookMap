
#from https://stackoverflow.com/questions/714100/os-detecting-makefile
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OPUSENC = ../../nest-libs/linux/opus-tools/bin/opusenc
else
	OPUSENC = ../../nest-libs/macos/opus-tools/bin/opusenc
endif

../dist/oh_no.opus : oh_no.wav
	$(OPUSENC) --vbr --bitrate 128 collision.wav ../dist/collision.opus
	$(OPUSENC) --vbr --bitrate 128 eat.wav ../dist/eat.opus
	$(OPUSENC) --vbr --bitrate 128 jump.wav ../dist/jump.opus
	$(OPUSENC) --vbr --bitrate 128 oh_no.wav ../dist/oh_no.opus
	$(OPUSENC) --vbr --bitrate 128 pickup.wav ../dist/pickup.opus
	$(OPUSENC) --vbr --bitrate 128 pleasure.wav ../dist/pleasure.opus
	$(OPUSENC) --vbr --bitrate 128 pot_cook.wav ../dist/pot_cook.opus
	$(OPUSENC) --vbr --bitrate 128 pot_finish.wav ../dist/pot_finish.opus
	$(OPUSENC) --vbr --bitrate 128 pot_put.wav ../dist/pot_put.opus
	$(OPUSENC) --vbr --bitrate 128 scene1_bgm.wav ../dist/scene1_bgm.opus
	$(OPUSENC) --vbr --bitrate 128 scene2_bgm.wav ../dist/scene2_bgm.opus
	$(OPUSENC) --vbr --bitrate 128 scene3_bgm.wav ../dist/scene3_bgm.opus
	$(OPUSENC) --vbr --bitrate 128 trash.wav ../dist/trash.opus
	$(OPUSENC) --vbr --bitrate 128 win.wav ../dist/win.opus
