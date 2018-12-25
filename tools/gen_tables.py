#!/usr/bin/env python

import math

if __name__ == "__main__":
    sint=[ int(round(math.sin(math.radians(i*1.40625))*64)) for i in range(256) ]
    cost=[ int(round(math.cos(math.radians(i*1.40625))*64)) for i in range(256) ]

    print("const s8 sin_table[%s] = {" % len(sint))
    for i in range(0, len(sint), 16):
        print("    %s," % ",".join([str(j) for j in sint[i:i+16]]))
    print("};\n\n")

    print("const s8 cos_table[%s] = {" % len(cost))
    for i in range(0, len(cost), 16):
        print("    %s," % ",".join([str(j) for j in cost[i:i+16]]))
    print("};\n\n")
