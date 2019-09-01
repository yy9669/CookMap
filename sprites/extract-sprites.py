#!/usr/bin/env python3

import os
import argparse
import subprocess
import sys

parser = argparse.ArgumentParser(description='Extract sprites listed in sprite_list to png files in out_directory.')
parser.add_argument('sprite_list', help='file containing list of sprites as lines with "file [layer1 layer2 ..] minX minY maxX maxY anchorX anchorY name"')
parser.add_argument('out_directory', help='directory in which to place output sprites, named "name_ax_ay.png"')
args = parser.parse_args()

print("Reading sprite list from '" + args.sprite_list + "', writing results to '" + args.out_directory + "'.")

sprites = []

#Load sprite descriptions from sprite_list:
with open(args.sprite_list, 'r', encoding='utf8') as sprite_list:
	for line in sprite_list:
		split = []
		in_quotes = False
		start_new = True

		#parse line into chunks separated by whitespace (with the exception that whitespace in quotes is ignored):
		i = 0
		while i < len(line):
			if in_quotes:
				if line[i] == '"':
					in_quotes = False
				elif line[i] == '\\':
					if i + 1 >= len(line) or not (line[i+1] == '"' or line[i+1] == '\\'):
						raise Exception("Can only escape '\"' or '\\'.")
					split[-1] = split[-1] + line[i+1]
					i += 1
				else:
					split[-1] = split[-1] + line[i]
			else:
				if line[i].isspace():
					start_new = True
				else:
					if start_new:
						start_new = False
						split.append("")
					assert(len(split) > 0)
					if line[i] == '"':
						in_quotes = True
					else:
						split[-1] = split[-1] + line[i]
			i += 1

		#ignore empty lines:
		if len(split) == 0:
			continue

		if len(split) != 8:
			raise Exception("Sprite file lines should contain exactly eight values; got:\n" + "|".join(split))
		try:
			for i in range(1,7):
				split[i] = float(split[i])
		except:
			raise Exception("Sprite file lines should be a filename, six numbers, and a sprite name; got:\n" + "|".join(split))

		sprites.append(split)

print("Will extract " + str(len(sprites)) + " sprites.")

os.makedirs(args.out_directory, exist_ok=True)

for sprite in sprites:
	source, min_x, min_y, max_x, max_y, anchor_x, anchor_y, name = sprite

	#Source might be something.xcf?layer&layer2&layer3:
	source_file = source
	source_layers = []
	i = source_file.find('?')
	if i != -1:
		source_file = source[:i]
		source_layers = source[i+1:].split('&')

	#compute output filename:
	out_file = args.out_directory + "/" + name + "_" + str(anchor_x-min_x) + "_" + str(anchor_y-min_y) + ".png"

	print(f"\n\n------- {source_file}: {' '.join(map(lambda x:'('+x+')', source_layers))} | [{str(min_x)},{str(max_x)}]x[{str(min_y)},{str(max_y)}] with anchor {str(anchor_x)}, {str(anchor_y)} => \"{out_file}\" -------")

	if os.path.exists(out_file):
		os.unlink(out_file)

	#based on:
	#https://stackoverflow.com/questions/5794640/how-to-convert-xcf-to-png-using-gimp-from-the-command-line
	#https://github.com/pft/script-fu/blob/master/script-fu-save-layers-to-files.scm
	script = f"""
		(let* (
				(file "{source_file}")
				(outfile "{out_file}")
				(min_x {min_x})
				(min_y {min_y})
				(max_x {max_x})
				(max_y {max_y})
				(image 0)
				(layers 0)
				(layer 0)
			)
			(set! image (car (gimp-file-load RUN-NONINTERACTIVE file file) ) )
			(gimp-image-crop image (- max_x min_x) (- max_y min_y) min_x min_y)
	"""
	if len(source_layers) != 0:
		script += f"""
			; set only the target layers visible:
			(set! layers (vector->list (cadr (gimp-image-get-layers image) ) ) )
			(while (pair? layers)
				(gimp-item-set-visible (car layers) FALSE)
				(set! layers (cdr layers))
			)
		"""
		for layer_name in source_layers:
			script += f"""
			(set! layer (car (gimp-image-get-layer-by-name image "{layer_name}") ) )
			(gimp-item-set-visible layer TRUE)
		"""
	
	script += """
			(gimp-image-merge-visible-layers image CLIP-TO-IMAGE)
			(set! layers (vector->list (cadr (gimp-image-get-layers image) ) ) )
			;(write "layers: ") (write layers) (newline)
			(while (pair? layers)
				(set! layer (car layers))
				;(write "layer: ") (write layer) (newline)
				;(write "  visible: ") (write (car (gimp-item-get-visible layer))) (newline)
				;(write "  name: ") (write (car (gimp-item-get-name layer))) (newline)
				(if (= TRUE (car (gimp-item-get-visible layer))) ;if layer is visible
					(begin
						;(write "   writing!") (newline)
						(file-png-save2 RUN-NONINTERACTIVE image layer outfile outfile FALSE 9 FALSE FALSE FALSE FALSE FALSE FALSE TRUE)
					)
				)
				(set! layers (cdr layers))
			)
		)
		(gimp-quit TRUE)
	"""
		#(file-png-save2 RUN-NONINTERACTIVE image ?? "temp.png" "temp.png" FALSE 9 FALSE FALSE FALSE FALSE FALSE FALSE TRUE)
	#print(script)
	subprocess.run([
		"gimp", "-n", "-i", "-b", "-"
	], input=script, encoding="utf8", check=True)

	if not os.path.isfile(out_file):
		sys.exit("ERROR: failed to extract sprite '" + name + "'")




