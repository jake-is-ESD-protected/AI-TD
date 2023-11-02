import serial
import platform
import sys

opsys = platform.platform()
print(f"Starting script from a {opsys} system")


def getPorts(verbose=True):
    ser = serial.Serial()
    print("discovering devices...")
    for ns in range(20):
        try:
            ser.port = "COM" + str(ns)
            ser.open()
            print(ser.port + " available")
            print(ser.get_settings())
            ser.close()

        except serial.SerialException:
            pass


def seedcli():
    cmd = sys.argv[1]
    args = sys.argv[1:]

    # NOTE: maybe move this assertion to the bash script?
    valid_cmds = ["stat", "send", "stop"]
    assert cmd in valid_cmds

    ser = serial.Serial()
