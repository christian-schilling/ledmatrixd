
import dbus
import xl.plugins as plugins

PLUGIN_NAME = "LedMatrix Plugin"
PLUGIN_AUTHORS = ['Some Dude <somedude@someplace.com>', ]
PLUGIN_VERSION = '0.1'
PLUGIN_DESCRIPTION = r"""Displays information via ledmatrixd"""
PLUGIN_ENABLED = False
PLUGIN_ICON = None


led_matrix = False
CONNS = plugins.SignalContainer()


def initialize():
    global led_matrix
    conn = dbus.SessionBus()
    obj = conn.get_object("net.initcrash.LedMatrix","/net/initcrash/LedMatrix")
    led_matrix = dbus.Interface(obj,dbus_interface="net.initcrash.LedMatrix")
    led_matrix.Init("192.168.2.10")
    led_matrix.PrintStr("Exaile")
    led_matrix.ScrollLeft(30)
    CONNS.connect(APP.player, 'play-track', play_track)
    return True

def destroy():
    CONNS.disconnect_all()

def play_track(exaile, track):
    led_matrix.PrintStr("\a%s \rby %s \bon %s" % (track.title, track.artist, track.album))
