import os
import scipy as sp
import numpy as np
from ctypes import *


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

    lib.AFInCAppend.argtypes = [c_double]
    lib.AFInCAppend.restype = None

    lib.AFInCProcess.argtypes = []
    lib.AFInCProcess.restype = None

    lib.afGetT1A.argtypes = []
    lib.afGetT1A.restype = c_double

    lib.afGetT2A.argtypes = []
    lib.afGetT2A.restype = c_double

    lib.afGetTempo.argtypes = []
    lib.afGetTempo.restype = c_double

    lib.afGetSpectralCentroid.argtypes = []
    lib.afGetSpectralCentroid.restype = c_double

    lib.afGetSpectralFlatness.argtypes = []
    lib.afGetSpectralFlatness.restype = c_double

    lib.afGetPBandL.argtypes = []
    lib.afGetPBandL.restype = c_double

    lib.afGetPBandML.argtypes = []
    lib.afGetPBandML.restype = c_double

    lib.afGetPBandMH.argtypes = []
    lib.afGetPBandMH.restype = c_double

    lib.afGetPBandH.argtypes = []
    lib.afGetPBandH.restype = c_double

    return lib