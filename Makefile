
main : main.c led_matrix.o
	gcc -o main main.c led_matrix.o -I. `pkg-config --libs --cflags glib-2.0` `pkg-config --libs --cflags gobject-2.0` `pkg-config --libs --cflags dbus-glib-1`



main.c : main.vala led_matrix.vapi
	valac -C main.vala led_matrix.vapi --pkg dbus-glib-1

led_matrix.o : led_matrix.c led_matrix.h
	gcc -c led_matrix.c
