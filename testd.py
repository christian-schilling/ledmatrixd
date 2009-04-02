import dbus
import sys
import time

conn = dbus.SessionBus()
obj = conn.get_object("net.initcrash.LedMatrix","/net/initcrash/LedMatrix")
led_matrix = dbus.Interface(obj,dbus_interface="net.initcrash.LedMatrix")

led_matrix.Init("192.168.0.93")
led_matrix.SelectFont(int(sys.argv[2]))
led_matrix.PrintStrDirect(sys.argv[1])
