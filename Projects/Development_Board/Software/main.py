import matplotlib
matplotlib.use("TkAgg")

import serial_tools as serial
import serial_slave_constants as ssc

from openpyxl import Workbook
import pandas as pd
import numpy as np
import sys
import time

import logging
from queue import Queue, Empty
from pc_ble_driver_py.observers import *


# def get_temp():

def ind_led_blink_short(ser):
    blink_array = [ssc.USB_COMMAND_HEADER, ssc.NRF52_MODULE, ssc.NRF52_COMMON_COMMAND, ssc.NRF52_LED_IND_BLINK, ssc.NRF52_LED_IND_SHORT_BLINK, ssc.USB_COMMAND_FOOTER]
    ser.write_array(blink_array)


def ind_led_blink_medium(ser):
    blink_array = [ssc.USB_COMMAND_HEADER, ssc.NRF52_MODULE, ssc.NRF52_COMMON_COMMAND, ssc.NRF52_LED_IND_BLINK, ssc.NRF52_LED_IND_MEDIUM_BLINK, ssc.USB_COMMAND_FOOTER]
    ser.write_array(blink_array)


def ind_led_blink_long(ser):
    blink_array = [ssc.USB_COMMAND_HEADER, ssc.NRF52_MODULE, ssc.NRF52_COMMON_COMMAND, ssc.NRF52_LED_IND_BLINK, ssc.NRF52_LED_IND_LONG_BLINK, ssc.USB_COMMAND_FOOTER]
    ser.write_array(blink_array)


def ind_led_blink_custom(ser, led_on_ms, led_off_ms):
    led_on_ms_hb = int(led_on_ms >> 8)
    led_on_ms_lb = int(led_on_ms % 256)
    led_off_ms_hb = int(led_off_ms >> 8)
    led_off_ms_lb = int(led_off_ms % 256)

    blink_array = [ssc.USB_COMMAND_HEADER, ssc.NRF52_MODULE, ssc.NRF52_COMMON_COMMAND, ssc.NRF52_LED_IND_BLINK, ssc.NRF52_LED_IND_CUSTOM_BLINK, led_on_ms_hb, led_on_ms_lb, led_off_ms_hb,
                   led_off_ms_lb, ssc.USB_COMMAND_FOOTER]
    ser.write_array(blink_array)


def ble_led_blink_custom(ser, led_on_ms, led_off_ms):
    led_on_ms_hb = int(led_on_ms >> 8)
    led_on_ms_lb = int(led_on_ms % 256)
    led_off_ms_hb = int(led_off_ms >> 8)
    led_off_ms_lb = int(led_off_ms % 256)

    blink_array = [ssc.USB_COMMAND_HEADER, ssc.NRF52_MODULE, ssc.NRF52_COMMON_COMMAND, ssc.NRF52_LED_BLE_BLINK, led_on_ms_hb, led_on_ms_lb, led_off_ms_hb, led_off_ms_lb, ssc.USB_COMMAND_FOOTER]
    ser.write_array(blink_array)


def main():
    ser = serial.SerialPort()
    baud_rate = 9600
    com_port = 'COM3'
    ser.open_serial_port(com_port, baud_rate)

    # time.sleep(1)
    # ind_led_blink_short(ser)
    # time.sleep(1)
    # ind_led_blink_medium(ser)
    # time.sleep(1)
    # ind_led_blink_long(ser)
    time.sleep(1)
    ind_led_blink_custom(ser, 1000, 1000)
    time.sleep(1)
    ble_led_blink_custom(ser, 1000, 1000)


if __name__ == "__main__":
    main()
