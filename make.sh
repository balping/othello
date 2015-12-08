#!/bin/bash

# http://stackoverflow.com/questions/410980/include-a-text-file-in-a-c-program-as-a-char

#xxd -i < o1.glade > o1.xxd
#echo ', 0' >> o1.xxd

# https://developer.gnome.org/gio/2.32/glib-compile-resources.html

glib-compile-resources --target resources.h --generate  resources.xml
glib-compile-resources --target resources.c --generate  resources.xml

rm othello

gcc -o othello othello.c resources.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic