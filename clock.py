import dbus
import sys
import time


conn = dbus.SessionBus()
obj = conn.get_object("net.initcrash.LedMatrix","/net/initcrash/LedMatrix")
led_matrix = dbus.Interface(obj,dbus_interface="net.initcrash.LedMatrix")

led_matrix.Init("192.168.2.10")

while(True):
    led_matrix.ClearScreen()
    led_matrix.PrintStr(time.strftime("  %H:%M:%S"))
    time.sleep(1)
