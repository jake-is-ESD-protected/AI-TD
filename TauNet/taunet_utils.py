import os
import scipy as sp
import numpy as np
import pandas as pd
from ctypes import *
import json


def read_audio_file(path, truncate_at=10):
    if path.endswith(".wav"):  # Check for .wav files
        sample_rate, audio_data = sp.io.wavfile.read(path)
        try:
            audio_data = audio_data[:, 0]
        except:
            pass
        n_bits = 32  # Assuming 32-bit audio
        audio_data = audio_data / (2**(n_bits - 1))  # Adjust range to -1 to 1 
        audio_data /= np.abs(np.max(audio_data))  # Safer normalization
        audio_data = audio_data[: sample_rate * truncate_at]
        label = os.path.splitext(os.path.basename(path))[0]  # Extract filename without extension
        return (audio_data, label, sample_rate)


def read_audio_files(directory, truncate_at=10):
    """
    Reads all audio files from a directory and returns their data with file name labels.

    Args:
        directory: The directory path (string).
        truncate_at: Amount of seconds after the audio will be cut.

    Returns:
        A list of tuples, where each tuple contains:
            - The audio data as a NumPy array.
            - The filename (without extension).
    """
    audio_data_list = []
    for filename in os.listdir(directory):
        audio = read_audio_file(os.path.join(directory, filename))
        if audio:
            audio_data_list.append(audio)
    return audio_data_list


def af_dsp_init(path_to_dll):
    lib = CDLL(path_to_dll)

    lib.resetBuffer.argtypes = []
    lib.resetBuffer.restype = None

    lib.initAf.argtypes = []
    lib.initAf.restype = None

    lib.AFInCAppend.argtypes = [c_float]
    lib.AFInCAppend.restype = None

    lib.processBTTAndAFInC.argtypes = []
    lib.processBTTAndAFInC.restype = None

    lib.afGetT1A.argtypes = []
    lib.afGetT1A.restype = c_float

    lib.afGetT2A.argtypes = []
    lib.afGetT2A.restype = c_float

    lib.afGetTempo.argtypes = []
    lib.afGetTempo.restype = c_float

    lib.afGetSpectralCentroid.argtypes = []
    lib.afGetSpectralCentroid.restype = c_float

    lib.afGetPBandL.argtypes = []
    lib.afGetPBandL.restype = c_float

    lib.afGetPBandML.argtypes = []
    lib.afGetPBandML.restype = c_float

    lib.afGetPBandMH.argtypes = []
    lib.afGetPBandMH.restype = c_float

    lib.afGetPBandH.argtypes = []
    lib.afGetPBandH.restype = c_float

    lib.afGetCrestFactor.argtypes = []
    lib.afGetCrestFactor.restype = c_float

    lib.afGetSpectralFlux.argtypes = []
    lib.afGetSpectralFlux.restype = c_float

    return lib


def create_dataset(audio_dir, human_input_csv, dsp_dll_path, save=None):
    lib = af_dsp_init(dsp_dll_path)
    audio_data = read_audio_files(audio_dir)
    audio_data = sorted(audio_data, key=lambda x: int(x[1].split()[0]))
    human_data = pd.read_csv(human_input_csv)

    input_data = dict()
    output_data = dict()
    
    for audio, label, fs in audio_data:
        lib.initAf()
        lib.resetBuffer()
        label = label.split()[0] + " " + label[-16:]
        for sample in audio:
            lib.AFInCAppend(sample)
        try:
            lib.processBTTAndAFInC()
        except OSError as e:
            print(f"AFinC died at {label}: {e}")
            print("skipping...")
            continue

        human_input = human_data[human_data['MEASUREMENT_ID'] == int(label.split()[0])].iloc[0].to_dict()
        human_input.pop("MEASUREMENT_ID")
        human_input.pop("SONG_ID")
        human_input.pop("ATTACK_T2")    # not yet needed
        human_input.pop("SUSTAIN_T2")   # not yet needed

        attack = human_input.pop("ATTACK_GAIN")
        if attack < 0.5:
            human_input["ATTACK_CUT"] = 1 - (attack * 2)
            human_input["ATTACK_BOOST"] = 0
        else:
            human_input["ATTACK_CUT"] = 0
            human_input["ATTACK_BOOST"] = (attack * 2) - 1
        
        sustain = human_input.pop("SUSTAIN_GAIN")
        if sustain < 0.5:
            human_input["SUSTAIN_CUT"] = 1 - (sustain * 2)
            human_input["SUSTAIN_BOOST"] = 0
        else:
            human_input["SUSTAIN_CUT"] = 0
            human_input["SUSTAIN_BOOST"] = (sustain * 2) - 1

        human_output = dict()
        human_output["ATTACK_T1"] = human_input.pop("ATTACK_T1") / 500 # force-normalize, see transientDSP.cpp
        human_output["SUSTAIN_T1"] = human_input.pop("SUSTAIN_T1") / 3000 # see above ^
        output_data[label] = tuple(human_output.values())
        
        input_data[label] = tuple(round(val, 4) for val in (
            lib.afGetTempo(),
            lib.afGetT1A(),# / fs,
            lib.afGetT2A(),# / fs,
            lib.afGetSpectralCentroid(),
            lib.afGetPBandL(),
            lib.afGetPBandML(),
            lib.afGetPBandMH(),
            lib.afGetPBandH(),
            lib.afGetCrestFactor(),
            lib.afGetSpectralFlux()
        ))
        input_data[label] += tuple(human_input.values())
        print(f"processed {label}...")
    if save:
        with open(save[:-5] + "in.json", "w") as json_file:
            json.dump(input_data, json_file)
        with open(save[:-5] + "out.json", "w") as json_file:
            json.dump(output_data, json_file)
    return input_data, output_data