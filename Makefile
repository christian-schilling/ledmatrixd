CFLAGS=`pkg-config --cflags glib-2.0` `pkg-config --cflags gobject-2.0` `pkg-config --cflags dbus-glib-1` -I.
LDFLAGS=`pkg-config --libs glib-2.0` `pkg-config --libs gobject-2.0` `pkg-config --libs dbus-glib-1`
VALAC=valac

main : main.o led_matrix.o

main.o: main.c

main.c : main.vala led_matrix.vapi
	$(VALAC) -C main.vala led_matrix.vapi --pkg dbus-glib-1

led_matrix.o : led_matrix.c led_matrix.h

.PHONY: clean

clean:
	$(RM) *.o main main.c
