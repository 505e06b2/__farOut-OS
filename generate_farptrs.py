#!/usr/bin/env python3

base_address = 0x60000000 #this is a far pointer address, NOT absolute
symbol_map_file = "stdlib_symbols.map"
stdlib_files = "src/standard_library/include/*.h"
out_path = "src/stdlib_farptrs.c"
known_symbols = {} #filled below

class Symbol:
	def __init__(self, function_signature, offset=0):
		self.function_signature = function_signature
		self.offset = offset

	def __repr__(self):
		return "<Symbol offset:%d signature:\"%s\">" % (self.offset, self.function_signature)

#Parse source files for far functions
import glob, re

file_contents = ""
for x in glob.glob(stdlib_files):
	with open(x) as f:
		file_contents += f.read()

#pre-process
file_contents = re.sub(r"\/\*.*?\*\/", "", file_contents, flags=re.S) #remove multi-line comments
file_contents = re.sub(r"\/\/.*$", "", file_contents, flags=re.M) #remove single-line comments
file_contents = re.sub(r"#.*$", "", file_contents, flags=re.M) #remove pre-processor commands

for x in re.findall(r"^\s*?(.+?__far\s+(.+?)\(.+\))\s*?;$", file_contents, flags=re.M):
	known_symbols[x[1]] = Symbol( x[0].replace(x[1], "(*%s)" % x[1]) )


#parse map file
check = False
with open(symbol_map_file, "r") as f:
	for x in f.readlines():
		if x.startswith(" *(.text)"):
			check = True
			continue
		elif x.startswith(" *(.data)"):
			break

		if check:
			o = x.split()
			if len(o) == 2 and o[1] in known_symbols.keys():
				known_symbols[o[1]].offset = int(o[0], base=16)

with open(out_path, "w") as f:
	f.write("#ifndef _STDLIB_FARPTRS_C\n#define _STDLIB_FARPTRS_C\n\n#include \"stdint.h\"\n\n")
	for i in known_symbols.keys():
		f.write("%s = (void __far *)0x%x;\n" % (known_symbols[i].function_signature, base_address + known_symbols[i].offset))
	f.write("\n#endif\n")
