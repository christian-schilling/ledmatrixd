using GLib;
using led_matrix;
using Thread;

[DBus (name = "net.initcrash.LedMatrix")]
public class LedMatrix : Object {

    bool initialized;
    uint scroll_id;
    led_matrix.line ledLine;
    string cur_message;
    int pos_x;

    public LedMatrix() {
        initialized = false;
        pos_x = 0;
        cur_message = "";
    }

    public void Init (string matrix_ip) {
        if(!initialized) {
            led_matrix.allocate_line(&ledLine);
            led_matrix.init(matrix_ip);
            initialized = true;
        }
    }

    public void PrintStr (string msg) {
        if(!initialized) return;
        cur_message = msg;
        led_matrix.clear_screen(&ledLine);
        led_matrix.print(cur_message,&ledLine);
        led_matrix.update(&ledLine);
    }

    public void ScrollLeft(int speed) {
        if(!initialized) return;

        if(scroll_id != 0)
            GLib.Source->remove(scroll_id);

        scroll_id = GLib.Timeout->add(speed,() => {
            ledLine.x--;
            if (ledLine.x < -512) ledLine.x += 512;
            led_matrix.clear_screen(&ledLine);
            led_matrix.print(cur_message,&ledLine);
            led_matrix.update(&ledLine);
        });
    }

    public void ScrollStop() {
        if(!initialized || scroll_id == 0) return;
        GLib.Source->remove(scroll_id);
        scroll_id = 0;
    }

    public void ClearScreen() {
        cur_message = "";
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
