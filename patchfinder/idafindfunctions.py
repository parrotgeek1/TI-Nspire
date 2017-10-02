import idc
import struct
import idautils
 
def find_all( opcode_str ):
    ret = []
    ea = idc.FindBinary(0, 1, opcode_str)
    while ea != idc.BADADDR:
        ret.append(ea)
        ea = idc.FindBinary(ea + 4, 1, opcode_str)
    return ret
     
def define_functions():
    # The function first searches for all user defined functions, reads
    # the opcodes and searches for that opcodes in the rest of the file.
    #
    # You can extend this by adding more disassembled instructions that
    # make you believe are function prologues.
    #
    # Obviously not any PUSH is a function start, this is only a filter
    # against erroneously defined functions. So if you define a function
    # that starts with other instruction (and you think there could be
    # other functions that start with that instruction), just add it here.
    prologues = ["STMFD", "push", "PUSH", "mov", "MOV"]
     
    print "Finding all signatures"
    #ea= 0
    #ea = 0x11800000
    #ea = 0x10000000

    opcodes = set()
    if idc.SegStart(ea) == 0xFFFFFFFF:
        print "Wrong segment address set!"
        return
    
    for funcea in idautils.Functions(idc.SegStart(ea), idc.SegEnd(ea)):
        # Get the opcode
        start_opcode = idc.Dword(funcea)
         
        # Get the disassembled text
        dis_text = idc.GetDisasm(funcea)
        we_like_it = False
         
        # Filter possible errors on manually defined functions
        for prologue in prologues:
            if prologue in dis_text:
                we_like_it = True
         
        # If it passes the filter, add the opcode to the search list.
        if we_like_it:
            opcodes.add(start_opcode)
         
    print "# different opcodes: %x" % (len(opcodes))
    while len(opcodes) > 0:
        # Search for this opcode in the rest of the file
        opcode_bin = opcodes.pop()
        opcode_str = " ".join(x.encode("hex") for x in struct.pack("<L", opcode_bin))
        print "Searching for " + opcode_str
        matches = find_all( opcode_str )
        for matchea in matches:
            # If the opcode is found in a non-function
            if not idc.GetFunctionName(matchea):
                # Try to make code and function
                print "Function at " + hex(matchea)
                idc.MakeCode(matchea)
                idc.MakeFunction(matchea)
 
    print "We're done!"
     
define_functions()
