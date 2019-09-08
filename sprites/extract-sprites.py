#!/usr/bin/env python3

import os
import argparse
import subprocess
import sys

parser = argparse.ArgumentParser(description='Extract sprites listed in sprite_list to png files in out_directory.')
parser.add_argument('sprite_list', help='file containing list of sprites as lines with "file [layer1 layer2 ..] minX minY (maxX|+sizeX) (maxY|+sizeY) (anchorX|+anchorOfsX) (anchorY|+anchorOfsY) name"')
parser.add_argument('out_directory', help='directory in which to place output sprites, named "name_ax_ay.png"')
parser.add_argument('--gimp', help='path to the gimp executable', default='gimp')
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
			split[1] = float(split[1])
			split[2] = float(split[2])
			#if any of maxX/maxY/anchorX/anchorY start with '+' interpret them relative to minX/minY
			if split[3][0] == '+': split[3] = split[1] + float(split[3][1:])
			else: split[3] = float(split[3])

			if split[4][0] == '+': split[4] = split[2] + float(split[4][1:])
			else: split[4] = float(split[4])

			if split[5][0] == '+': split[5] = split[1] + float(split[5][1:])
			else: split[5] = float(split[5])

			if split[6][0] == '+': split[6] = split[2] + float(split[6][1:])
			else: split[6] = float(split[6])
		except:
			raise Exception("Sprite file lines should be a filename, six numbers, and a sprite name; got:\n" + "|".join(split))

		sprites.append(split)

print("Will extract " + str(len(sprites)) + " sprites.")

os.makedirs(args.out_directory, exist_ok=True)


def encode_name(name):
	#used to produce filenames from utf8 strings.
	#for cross-platform reasons, will only use: 0-9, a-z, A-Z, '-', '.', '_'
	#will use '_' for encoding:
	# "_" => "__" <-- underscore to double-underscore
	# "A" => "_a" <-- capital letters to lowercase letters with underscore prefix
	# anything else not [0-9a-z-] => "_0HH" or "_0HHHH" or "_0HHHHHH" or "_0HHHHHHHH" <-- hex encoding of unicode bytes
	out = ""
	for c in name:
		if c in "abcdefghijklmnopqrstuvwxyz0123456789-":
			out += c
		elif c == "_":
			out += "__"
		elif ord(c) >= ord("A") and ord(c) <= ord("Z"):
			out += "_" + chr(ord(c)+(ord("a")-ord("A")))
		else:
			out += "_0" + bytes(c,'utf8').hex()
	return out


scripts = "";
out_files = []


#Because GIMP doesn't read from stdin on windows for some reason, I need to pass all script commands on the command line, which can exceed the character limit of window's CreateProcess.
#Have I mentioned that trying to get work done on windows is quite frustrating sometimes?
#Regardless, this run_scripts() is designed to be used to incrementally run & clear the scripts variable when it starts getting too large.
def run_scripts():
	global scripts
	global out_files

	scripts += "(gimp-quit TRUE)"

	for out_file in out_files:
		if os.path.exists(out_file):
			os.unlink(out_file)

	#DEBUG:
	#with open('script.scheme', 'w') as f:
	#	f.write(scripts)

	print("---------  running GIMP  ----------")
	subprocess.run([
		args.gimp, "-n", "-i", "-b", scripts
	], encoding="utf8", check=True)
	print("\n-----------------------------------")

	for out_file in out_files:
		if not os.path.isfile(out_file):
			sys.exit("ERROR: failed to extract sprite '" + name + "'")

	scripts = ""
	out_files = []



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
	out_file = args.out_directory + "/" + encode_name(name) + "_" + str(anchor_x-min_x) + "_" + str(anchor_y-min_y) + ".png"

	print(f"  {source_file}: {' '.join(map(lambda x:'('+x+')', source_layers))} | \"{name}\" [{str(min_x)},{str(max_x)}]x[{str(min_y)},{str(max_y)}] with anchor {str(anchor_x)}, {str(anchor_y)} => \"{out_file}\"")

	source_file_for_script = os.path.normpath(os.getcwd() + '/' + source_file).replace('\\','\\\\').replace('"','\\"')
	out_file_for_script = os.path.normpath(os.getcwd() + '/' + out_file).replace('\\','\\\\').replace('"','\\"')

	#based on:
	#https://stackoverflow.com/questions/5794640/how-to-convert-xcf-to-png-using-gimp-from-the-command-line
	#https://github.com/pft/script-fu/blob/master/script-fu-save-layers-to-files.scm
	#... and a lot of trial-and-error
	script = f"""
		(let* (
				(file "{source_file_for_script}")
				(outfile "{out_file_for_script}")
				(min_x {min_x})
				(min_y {min_y})
				(max_x {max_x})
				(max_y {max_y})
				(image 0)
			)
			(define (forItems image fn)
				(let* (
						(layers (vector->list (cadr (gimp-image-get-layers image) ) ) )
						(layer 0)
						(children 0)
					)
					(while (pair? layers)
						(set! layer (car layers))
						(set! layers (cdr layers))
						(if (= TRUE (car (gimp-item-is-group layer)))
							(begin
								(set! children (cadr (gimp-item-get-children layer)))
								(set! layers (append (vector->list children) layers))
								;DEBUG: (write layers) (newline)
							)
						)
						(begin
							(fn layer)
						)
					)
				)
			)
			(write outfile) (newline)
			(set! image (car (gimp-file-load RUN-NONINTERACTIVE file file) ) )
			(gimp-image-crop image (- max_x min_x) (- max_y min_y) min_x min_y)
	"""
	if len(source_layers) != 0:
		script += f"""
			; set only the target layers visible:
			(forItems image (lambda (item) (begin
				(gimp-item-set-visible item FALSE)
			) ) )
		"""
		for layer_name in source_layers:
			script += f"""
			(let *
				(
					(layer (car (gimp-image-get-layer-by-name image "{layer_name}") ) )
				)
				(write "set visible {layer_name}") (write layer) (newline)
				(gimp-item-set-visible layer TRUE) ;doing this in order to cause an error when layer name not defined
				(while (not (= layer -1))
					(gimp-item-set-visible layer TRUE)
					(set! layer (car (gimp-item-get-parent layer)))
					;DEBUG: (write layer) (newline)
				)
			)
		"""
	
	script += """
			(gimp-image-merge-visible-layers image CLIP-TO-IMAGE)
			(forItems image (lambda (item) (begin
				;(write "layer: ") (write item) (newline)
				;(write "  visible: ") (write (car (gimp-item-get-visible item))) (newline)
				;(write "  name: ") (write (car (gimp-item-get-name item))) (newline)
				(if (= TRUE (car (gimp-item-is-layer item))) (begin
					(if (= TRUE (car (gimp-item-get-visible item))) ;if layer is visible
						(begin
							(write "   writing") (newline)
							(file-png-save2 RUN-NONINTERACTIVE image item outfile outfile FALSE 9 FALSE FALSE FALSE FALSE FALSE FALSE TRUE)
						)
					)
				))
			)))
			(gimp-image-delete image)
		)
	"""
	if os.name == "nt" and len(scripts + script) > 30000:
		run_scripts();
	elif len(scripts + script) > 130000:
		run_scripts();
	
	scripts += script
	out_files.append(out_file)

if scripts != "":
	run_scripts()

print("It appears that the sprite extraction was successful.")
