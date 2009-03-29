using GLib;
using led_matrix;

[DBus (name = "net.initcrash.LedMatrix")]
public class LedMatrix : Object {

    bool initialized;
    bool scrolling;
    led_matrix.line ledLine;

    public LedMatrix() {
        initialized = false;
    }

    public void Init (string matrix_ip) {
        if(!initialized) {
            led_matrix.allocate_line(&ledLine,512);
            led_matrix.init(matrix_ip);
            initialized = true;
        }
    }

    public void PrintStr (string msg) {
        if(!initialized) return;
        led_matrix.print(msg, &ledLine);
        led_matrix.update(&ledLine);
    }

    public void ScrollLeft(int speed) {
        if(!initialized || scrolling) return;
        scrolling = true;
        GLib.Timeout->add(speed,() => {
            if(!scrolling)
                return false;
            led_matrix.shift_left(&ledLine);
            led_matrix.update(&ledLine);
        });
    }

    public void ScrollStop() {
        if(!initialized || !scrolling) return;
        scrolling = false;
    }
    
    public void ClearScreen() {
        if(!initialized) return;
        led_matrix.clear_screen(&ledLine);
    }
}

void main () {
    var loop = new MainLoop (null, false);

    try {
        var conn = DBus.Bus.get (DBus.BusType. SESSION);

        dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus",
                                                   "/org/freedesktop/DBus",
                                                   "org.freedesktop.DBus");
        // try to register service in session bus
        uint request_name_result = bus.request_name ("net.initcrash.LedMatrix", (uint) 0);
        if (request_name_result == DBus.RequestNameReply.PRIMARY_OWNER) {
            stderr.printf ("good\n");
        } else {
            stderr.printf ("not good\n");
        }

        // start server
        var server = new LedMatrix ();


        conn.register_object ("/net/initcrash/LedMatrix", server);
        loop.run ();

    } catch (Error e) {
        stderr.printf ("Oops: %s\n", e.message);
    }
}
