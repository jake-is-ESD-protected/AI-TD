import serial
from serial.tools import list_ports
import sys
from time import sleep


def getDaisyPort(verbose=False):
    DAISY_HW_IDENTIFIER = "VID:PID=0483:5740"
    ports = list(list_ports.comports())
    for port in ports:
        if DAISY_HW_IDENTIFIER in port.hwid:
            if verbose:
                print(f"Found Daisy Seed ({port.hwid}) on port {port.name}")
            return port.name
    return None


def seedcli():
    cmd = sys.argv[1]
    args = sys.argv[1:]

    # NOTE: maybe move this assertion to the bash script?
    valid_cmds = ["stat", "send", "stop", "echo"]
    assert cmd in valid_cmds

    ser = serial.Serial(getDaisyPort())
    ser.write(cmd.encode())
    sleep(0.01)
    print(ser.readline().decode())


if __name__ == "__main__":
    seedcli()
