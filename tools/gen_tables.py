#!/usr/bin/env python

import math

if __name__ == "__main__":
    sint=[ int(round(math.sin(math.radians(i*1.40625))*64)) for i in range(256) ]
    cost=[ int(round(math.cos(math.radians(i*1.40625))*64)) for i in range(256) ]
    #atant=[int(math.degrees(math.atan(float(i)/32))) for i in range(33)]
    atant=[int(math.degrees(math.atan(float(i)/16.0))/1.40625) for i in range(17)]
    sqrts = [ int(round(math.sqrt(i)*16)) for i in range(256) ]

    print('#include "blast.h"')
    print("const s8 sin_table[%s] = {" % len(sint))
    for i in range(0, len(sint), 16):
        print("    %s," % ",".join([str(j) for j in sint[i:i+16]]))
    print("};\n\n")

    print("const s8 cos_table[%s] = {" % len(cost))
    for i in range(0, len(cost), 16):
        print("    %s," % ",".join([str(j) for j in cost[i:i+16]]))
    print("};\n\n")

    print("const u8 atan_table[%s] = {" % len(atant))
    for i in range(0, len(atant), 16):
        print("    %s," % ",".join([str(j) for j in atant[i:i+16]]))
    print("};\n\n")

    print("const u8 sqrt_table[%s] = {" % len(sqrts))
    for i in range(0, len(sqrts), 16):
        print("    %s," % ",".join([str(j) for j in sqrts[i:i+16]]))
    print("};\n\n")

