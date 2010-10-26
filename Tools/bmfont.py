from __future__ import with_statement

def read_bmfont(source):
	import os
	import shlex
	ret = {}

	def parse(line):
		parts = shlex.split(line)
		section = parts[0]

		table = []
		for part in parts[1:]:
			attrib, value = part.split('=')
			value = int(value) if value.lstrip('-').isdigit() else value.strip('"')		
			table.append((attrib, value))
		
		table = dict(table)
		
		return (section, table)

	# Create dictionary of all font data
	with open(source, "r") as src:
		for line in src:
			try:
				type, table = parse(line)
				ret.setdefault(type, []).append(table)
			except:
				return (None, "Encountered unknown line, bailed! (='%s')" % line)
				
	# Check for all required sections. Kerning is optional.
	required_sections = ['info', 'common', 'page', 'chars', 'char']
	if not all([x in ret for x in required_sections]):
		return (None, "Missing sections in file")
		
	# Does not support multiple pages.
	if len(ret['page']) > 1:
		return (None, "Multiple pages are not supported.")
		
	# Read image data
	image_path = os.path.join(os.path.dirname(source), ret['page'][0]['file'])
	with open(image_path, "rb") as image:
		ret['image_data'] = image.read()
	
	return (ret, None)
	
def write_pfnt(font_data, target):
	import struct

	def to_uint32(v): return struct.pack("<L", int(v))
	def to_int16(v):  return struct.pack("<h", int(v))
	def to_float(v):  return struct.pack("<f", float(v))
	def to_string(s): return struct.pack("<%ds" % (len(s)+1), s)
	def arr_to_int16vec(a): return struct.pack("<%dh"%len(a), *a)
	def arr_to_floatvec(a):  return struct.pack("<%df"%len(a), *a)
	def to_texcoords(x, y, w, h, sw, sh):
		if h == 0 or w == 0:
			return (0., 0., 0., 0.)
			
		tx1 = float(x)/float(sw)
		tx2 = float(x+w)/float(sw)
		ty1 = float(y)/float(sh)
		ty2 = float(y+h)/float(sh)
		return (tx1, ty1, tx2, ty2)

	# Set some constant values
	header = to_uint32(0xBADC0FFE)
	section_info = to_uint32(0x53C70001)	# Font name, font size
	section_common = to_uint32(0x53C70002)	# Line height, ...
	section_chars = to_uint32(0x53C70003)	# Info for all 256 characters
	section_kerning = to_uint32(0x53C70004)	# Optional kerning information
	section_image = to_uint32(0x53C70005)	# Image data (assumed to be PNG)
	
	# info section data
	fontname = to_string(font_data['info'][0]['face'])
	fontname_len = to_uint32(len(fontname))

	# common info data
	scaleW = font_data['common'][0]['scaleW']
	scaleH = font_data['common'][0]['scaleH']
	width = to_int16(scaleW)
	height = to_int16(scaleH)
	lineheight = to_int16(font_data['common'][0]['lineHeight'])
	
	# chars data (add tx, ty, th, tw)
	chars = bytearray()
	chars_count = len(font_data['char'])
	
	fields_int16 = ['id','width', 'height', 'xoffset', 'yoffset', 'xadvance']
	for c in font_data['char']:
		if c['id'] in range(0, 255):
			chars += arr_to_int16vec([c[x] for x in fields_int16])
			chars += arr_to_floatvec(to_texcoords(c['x'], c['y'], c['width'], c['height'], scaleW, scaleH))
		else:
			chars_count -= 1
	chars_count = to_int16(chars_count)
			
	# optional kerning info
	kerning_count = to_int16(0)
	kernings = bytearray()
	if 'kernings' in font_data and 'kerning' in font_data:
		kerning_count = to_int16(font_data['kernings'][0]['count'])
		for k in font_data['kerning']:
			fields = ['first', 'second', 'amount']
			kernings += arr_to_int16vec([k[f] for f in fields])

	# image size
	image_size = to_uint32(len(font_data['image_data']))

	# Write data
	with open(target, "wb") as dst:
		dst.write(header) # 4b
		
		dst.write(section_info) # 4b
		dst.write(fontname_len) # 4b
		dst.write(fontname)     # fontname_len * 1b

		dst.write(section_common) # 4b
		dst.write(width) # 2b
		dst.write(height) # 2b
		dst.write(lineheight) # 2b
		
		dst.write(section_chars) # 4b
		dst.write(chars_count) # 2b
		dst.write(chars) # 28b * chars_count
		
		dst.write(section_kerning) # 4b
		dst.write(kerning_count) # 2b
		if struct.unpack("<h", kerning_count)[0] > 0:
			dst.write(kernings) # 6b * kerning_count
		
		dst.write(section_image) # 4b
		dst.write(image_size) # 4b
		dst.write(font_data['image_data'])

def convert(source, target):
	(font_data, info) = read_bmfont(source)
	
	if font_data:
		write_pfnt(font_data, target)
		return (True, None)
	return (False, info)

def main(argv):
	import os, glob, re
	
	if len(argv) < 2:
		print("usage: %s path/to/font.fnt ..." % argv[0])
		return -1
		
	# Expand directories in args
	sources = []
	for src in argv[1:]:
		if os.path.isdir(src):
			sources += map(os.path.normpath, glob.glob(src + os.path.sep + '*.fnt'))
		else:
			sources.append(src)
		
	# Create target filenames
	targets = [re.sub("fnt$", "pfnt", source) for source in sources]
	filenames = zip(sources, targets)
		
	# Convert
	for source, target in filenames:

		# Check for source files and warn if we are overwriting existing files
		if not os.path.exists(source):
			print("error: source file (='%s') does not exist" % source)
			return -1
			
		if os.path.exists(target):
			print("warning: overwriting file (='%s')." % target)
	
		(res, info) = convert(source, target)
		print("Converted '%s'... %s" % (os.path.basename(source),"OK" if res else "Failed"))
		if not res:
			print("- %s" % info)
			
	print("Done.")
	
	return 0
	
	

if __name__ == '__main__':
	import sys
	ret = main(sys.argv)
	sys.exit(ret)