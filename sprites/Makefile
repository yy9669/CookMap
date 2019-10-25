
OS := $(shell uname)

ifeq ($(OS), Linux)
	GIMP=gimp
else
	GIMP=/Applications/GIMP.app/Contents/MacOS/gimp
endif


../dist/cookmap.png ../dist/cookmap.atlas : cookmap.xcf cookmap.list trade.xcf trade-font.list extract-sprites.py pack-sprites
	./extract-sprites.py cookmap.list cookmap --gimp='$(GIMP)'
	./extract-sprites.py trade-font.list cookmap --gimp='$(GIMP)'
	./pack-sprites ../dist/cookmap cookmap/*
	rm -rf cookmap
