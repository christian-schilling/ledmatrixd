import dbus
import sys
import time

conn = dbus.SessionBus()
obj = conn.get_object("net.initcrash.LedMatrix","/net/initcrash/LedMatrix")
led_matrix = dbus.Interface(obj,dbus_interface="net.initcrash.LedMatrix")

led_matrix.Init("192.168.2.10")
led_matrix.PrintStr(sys.argv[1])
