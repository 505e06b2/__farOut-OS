#!/usr/bin/env python3

import sys

if sys.argv[1] == "stdlib":
	base_address = 0x60000000 #this is a far pointer address, NOT absolute
	symbol_map_file = "stdlib_symbols.map"
	stdlib_files = "src/standard_library/include/*.h"
	out_path = "src/generated_stdlib_includes/"
	replace = "obj/lib_"

elif sys.argv[1] == "kernel":
	base_address = 0x70000000 #this is a far pointer address, NOT absolute
	symbol_map_file = "kernel_symbols.map"
	stdlib_files = "src/kernel/include/*.h"
	out_path = "src/generated_kernel_includes/"
	replace = "obj/kernel_"

else:
	raise sys.argv[1] + ": Not a valid argument"

known_symbols = {} #structure to be: Root -> Base name -> symbol

class Symbol:
	def __init__(self, function_signature, offset=0):
		self.function_signature = function_signature
		self.offset = offset

	def __repr__(self):
		return "<Symbol offset:%d signature:\"%s\">" % (self.offset, self.function_signature)

#Parse source files for far functions
import glob, re, os

for file_name in glob.glob(stdlib_files):
	basename = os.path.basename(file_name)
	known_symbols[basename] = {}

	with open(file_name) as f:
		file_contents = f.read()

		#pre-process
		file_contents = re.sub(r"\/\*.*?\*\/", "", file_contents, flags=re.S) #remove multi-line comments
		file_contents = re.sub(r"\/\/.*$", "", file_contents, flags=re.M) #remove single-line comments
		file_contents = re.sub(r"#.*$", "", file_contents, flags=re.M) #remove pre-processor commands

		#parse header file(s) input for __far functions
		for y in re.findall(r"^\s*?(.+?__far\s+(.+?)\(.+\))\s*?;$", file_contents, flags=re.M):
			known_symbols[basename][y[1]] = Symbol( y[0].replace(y[1], "(* const %s)" % (y[1])))


#parse map file
check = False
with open(symbol_map_file, "r") as f:
	for x in f.readlines():
		if x.startswith(" .text"):
			check = True
			current_file = x.split()[-1].replace(".o", ".h").replace(replace,"") #replace with more if needed
			continue
		elif x.startswith(" *(.data)"):
			break

		if check:
			o = x.split()
			try:
				if len(o) == 2 and o[1] in known_symbols[current_file].keys():
					known_symbols[current_file][o[1]].offset = int(o[0], base=16)
			except KeyError:
				pass

for basename in known_symbols:
	if len(known_symbols[basename]) < 1:
		continue
	with open(os.path.join(out_path, basename), "w") as f:
		guard = basename.upper().replace(".", "_")
		f.write("#ifndef _%s\n#define _%s\n\n#include <stdint.h>\n#include <stddef.h>\n\n" % (guard, guard))
		for x in known_symbols[basename]:
			symbol = known_symbols[basename][x]
			f.write("%s = (void __far *)0x%x;\n" % (symbol.function_signature, base_address + symbol.offset))
		f.write("\n#endif\n")

