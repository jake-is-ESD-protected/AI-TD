import json
import os
from os.path import join
import numpy as np

def json2dict(path:str):
    with open(path, "r") as f:
        d = json.load(f)
    return d

def getLayerShapes(d:dict):
    ls = dict()
    for i, layer in enumerate(d["layers"]):
        ls[f"#define SHAPE_L_{i+1}"] = layer["shape"][1]
    return ls

def getInitFunc():
    s = """
void ai_init(){

}
"""

def writeHFile(path:str, d:dict):
    header = os.path.basename(path).upper().replace('.', '_')
    shapes_out = getLayerShapes(d)
    temp = shapes_out.copy()
    shapes_in = {"#define IN_SHAPE": d["in_shape"][1]}
    temp.pop(list(temp.keys())[-1])
    shapes_in = {**shapes_in, **temp}
    s = f"#ifndef _{header}_\n#define _{header}_\n\n"
    s += f"#define IN_SHAPE {d['in_shape'][1]}\n\n"
    defines = ""
    vs = "\n"
    for layer, array_name, shape_name, value in zip(d["layers"], shapes_out.keys(), shapes_in.keys(), shapes_out.values()):
        weights = layer['weights'][0]
        biases = layer['weights'][1]

        defines += f"{array_name} {value}\n"
        vs += f"\nconst float weights_l{array_name.replace('#define SHAPE_L_', '')}[][{shape_name.replace('#define ', '')}]"+ "{\n"
        
        weights = [list(x) for x in zip(*weights)] # thx to Kristof, we know that RTNeural expects transposed weights
        for weight in weights:
            vs += f"\t{str(weight).replace('[', '{').replace(']', '}')},\n"
        vs += "};"

        vs += f"\nconst float bias_l{array_name.replace('#define SHAPE_L_', '')}[] =" + "\n"
        vs += "\t" + (f"{str(biases).replace('[', '{').replace(']', '}')}".replace('\t', ',')) + ";\n"
    s += defines + vs
    s += "\n#endif"
    
    with open(path, "w") as f:
        f.write(s)

if __name__ == "__main__":
    newest = os.listdir(join("TauNet", "logs"))[-1]
    d = json2dict(join("TauNet", "logs", newest, "taunet.json"))
    ls = getLayerShapes(d)
    writeHFile(join("src", "ai", "model.h"), d)
    writeHFile(join("src", "qspiFlashLoader", "modelData.h"), d)