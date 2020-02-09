#!/usr/bin/env python
import argparse
import json
import xmltodict
import base64
import zlib
import struct
import pprint

class TiledData(object):

    def __init__(self, filename):
        with open(filename) as h:
           data = h.read()

        tile_data = xmltodict.parse(data)
        
        self.parsed_data = {k.lstrip('@'):v for k,v in tile_data.get('map').items()}
        self.parsed_data['tilesets'] = []
        self.parsed_data['layers'] = []

        self.parse_tilesets(tile_data.get('map').get('tileset'))
        self.parse_layers(tile_data.get('map').get('layer'))


    def parse_tilesets(self, tileset_data):
        tempset = {k.lstrip('@'):v for k,v in tileset_data.items()}
        tempset['image'] = {k.lstrip('@'):v for k,v in tileset_data.get('image', {}).items()}
        tileprops = {
            x.get('@id'):{x.get('properties').get('property').get('@name'):x.get('properties').get('property').get('@value')}
            for x in tileset_data.get('tile',{})
        }
        tempset['tileproperties'] = tileprops
        self.parsed_data['tilesets'].append(tempset)


    def parse_layers(self, layer_data):
        tempset = {k.lstrip('@'):v for k,v in layer_data.items()}
        layerdata = layer_data.get('data')
        rawdata = layerdata.get('#text')
        encoding = layerdata.get('@encoding')
        compression = layerdata.get('@compression')

        if encoding == 'base64':
            decoded = base64.b64decode(rawdata)
        elif encoding == 'csv':
            decoded = rawdata.split(',')
        else:
            decoded = [x.get('@gid') for x in layerdata.get('tile')]

        if compression == 'zlib':
            decompressed = zlib.decompress(decoded)
        else:
            decompressed = decoded

        if encoding == 'base64':
            tilesize = struct.calcsize('<I')
            tiledata = struct.unpack('<%sI' % ( int(len(decompressed)/tilesize) ), decompressed)
        else:
            tiledata = decompressed

        tempset['data'] = tiledata
        self.parsed_data['layers'].append(tempset)


    def get_dict(self):
        return self.parsed_data


class Tile2C(object):
    def __init__(self, infile, outfile):
        self.inputf = infile
        self.outputf = outfile

    def run(self):
        #f = open(self.inputf)
        #try:
        #    data = f.read()
        #finally:
        #    f.close()

        #tiled_data = json.loads(data)
        
        tf = TiledData(self.inputf)
        tiled_data = tf.get_dict()


        print(dir(tiled_data))
        print(tiled_data.keys())

        print("Orientation: ", tiled_data.get("orientation"))
        print("Layer Count: ", len(tiled_data.get("layers", [])))
        print("Height:      ", tiled_data.get("height"))
        print("Width:       ", tiled_data.get("width"))
        print("Tile Height: ", tiled_data.get("tileheight"))
        print("Tile Width:  ", tiled_data.get("tilewidth"))
        print("Properties:  ", tiled_data.get("properties"))
        print("Version:     ", tiled_data.get("version"))
        print("Tile Sets:   ", tiled_data.get("tilesets"))
        print("Tile Image:  ", tiled_data.get("tilesets",[{}])[0].get("image"))
        print("Tile props:  ", tiled_data.get("tilesets",[{}])[0].get("tileproperties"))

        tiles_name = tiled_data.get("tilesets",[{}])[0].get("name")
        tile_props = tiled_data.get("tilesets",[{}])[0].get("tileproperties")

        layer0 = tiled_data.get("layers",[{}])[0]
        print(layer0.keys())
       
        tiles = layer0.get("data")
        width = int(layer0.get("width"))
        height = int(layer0.get("height"))

        f = open(self.outputf, "w")
        try:
            f.write("const unsigned char %s_map_data[%s] = {\n" % (tiles_name, width*height))
            for i in range(height):
                f.write(",".join(["%s" % hex(int(x)) for x in tiles[width*i:width*(i+1)]]))
                f.write(",\n")
            f.write("};\n")

            f.write("const unsigned char %s_map_coll[%s] = {" % (tiles_name,len(tile_props)))
            f.write(",".join(
                [hex(int(tile_props.get(str(y),{}).get('collision','0'))) for y in range(len(tile_props))]
            ))
            f.write("};\n");

            f.write("const tilemap %s_map = {\n" % tiles_name)
            f.write("%s,\n" % width)
            f.write("%s,\n" % height)
            f.write("(unsigned char *) %s_map_data,\n" % tiles_name)
            f.write("(unsigned char *) %s_map_coll\n};\n" % tiles_name) 
        finally:
            f.close()

        print("LEN:", len(["%s" % hex(int(x)) for x in tiles[width*i:width*(i+1)]]))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
            description="Turn Tiled json files into .h files for SGDK."
    )

    parser.add_argument(
            "infile",
            type=str,
            help="Input json file that should be read."
    )
    parser.add_argument(
            "outfile",
            type=str,
            help="Output file."
    )

    args = parser.parse_args()

    t = Tile2C(args.infile, args.outfile)
    t.run()
