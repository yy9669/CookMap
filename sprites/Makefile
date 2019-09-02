

../dist/the-planet.png ../dist/the-planet.atlas : the-planet.xcf the-planet.list extract-sprites.py pack-sprites
	rm -rf the-planet
	./extract-sprites.py the-planet.list the-planet
	./pack-sprites ../dist/the-planet the-planet/*
