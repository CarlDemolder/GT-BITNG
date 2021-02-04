import sys


# noinspection PyGlobalUndefined
def init(conn_ic_id):
    global BLEDriver, Flasher
    from pc_ble_driver_py import config

    config.__conn_ic_id__ = conn_ic_id
    # noinspection PyUnresolvedReferences
    from pc_ble_driver_py.ble_driver import BLEDriver, Flasher


def main(selected_port):
    descs = BLEDriver.enum_serial_ports()
    print("enum_serial_ports: {} serial ports found".format(len(descs)))
    for i, d in enumerate(descs):
        print("\nSerial port #{}:".format(i))
        print("|")
        print('|-  Port: "{}"'.format(d.port))
        print('|-  Manufacturer: "{}"'.format(d.manufacturer))
        print('|-  Serial Number: "{}"'.format(d.serial_number))
        print('|-  PnP ID: "{}"'.format(d.pnp_id))
        print('|-  Location ID: "{}"'.format(d.location_id))
        print('|-  Vendor ID: "{}"'.format(d.vendor_id))
        print('|_  Product ID: "{}"'.format(d.product_id))
    if selected_port is not None:
        flasher = Flasher(serial_port=selected_port)
        if flasher.fw_check():
            print(
                'Port "{}" already flashed with connectivity firmware'.format(
                    selected_port
                )
            )
        else:
            print('Flashing Port "{}"'.format(selected_port))
            flasher.fw_flash()
            print("Firmware flashed")


if __name__ == "__main__":
    port = None
    if len(sys.argv) < 2:
        print("Please specify connectivity IC identifier (NRF51, NRF52)")
        exit(1)
    init(sys.argv[1])
    if len(sys.argv) == 3:
        port = sys.argv[2]
    main(port)
    quit()