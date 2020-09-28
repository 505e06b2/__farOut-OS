#!/usr/bin/env python3

base_address = 0x60000000 #this is a far pointer address, NOT absolute
symbol_map_file = "stdlib_symbols.map"
out_path = "src/stdlib_farptrs.c"

class Symbol:
	def __init__(self, function_signature, offset=0):
		self.function_signature = function_signature
		self.offset = offset

known_symbols = { #Able to automate this and check for __far on function with pycparser??
	"printf": Symbol("void __far (*%s)(const char *, ...)"),
	"puts": Symbol("void __far (*%s)(const char *)")
}

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
	f.write("#ifndef _STDLIB_FARPTRS_C\n#define _STDLIB_FARPTRS_C\n\n")
	for i in known_symbols.keys():
		f.write("%s = (void __far *)0x%x;\n" % (known_symbols[i].function_signature % i, base_address + known_symbols[i].offset))
	f.write("\n#endif\n")
