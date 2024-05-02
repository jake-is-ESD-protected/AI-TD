import json
import os
from os.path import join

def json2dict(path=str):
    with open(path, "r") as f:
        d = json.load(f)
    return d

def getLayerShapes(d=dict):
    ls = dict()
    for i, layer in enumerate(d["layers"]):
        ls[f"#define SHAPE_L_{i+1}"] = layer["shape"][1]
    return ls

def getInitFunc():
    s = """
void ai_init(){

}
"""

def writeHFile(path=str, d=dict):
    header = os.path.basename(path).upper().replace('.', '_')
    shapes = getLayerShapes(d)
    s = f"#ifndef _{header}_\n#define _{header}_\n\n"
    s += f"#define IN_SHAPE {d['in_shape'][1]}\n\n"
    defines = ""
    vs = "\n"
    
    for layer, name, value in zip(d["layers"], shapes.keys(), shapes.values()):
        weights = layer['weights'][0]
        biases = layer['weights'][1]

        defines += f"{name} {value}\n"
        vs += f"\nconst std::vector<std::vector<float>> weights_l{name.replace('#define SHAPE_L_', '')}"+ "{\n"
        
        for weight in weights:
            vs += f"\t{str(weight).replace('[', '{').replace(']', '}')},\n"
        vs += "};"

        vs += f"\nconst float bias_l{name.replace('#define SHAPE_L_', '')}[] =" + "\n"
        vs += "\t" + (f"{str(biases).replace('[', '{').replace(']', '}')}".replace('\t', ',')) + ";\n"
    s += defines + vs
    s += "\n#endif"
    
    with open(path, "w") as f:
        f.write(s)

if __name__ == "__main__":
    d = json2dict(join("TauNet", "logs", "20240429-125746", "taunet.json"))
    print(d["in_shape"])
    ls = getLayerShapes(d)
    writeHFile(join("src", "ai", "model.h"), d)