gstopwatch:
	gcc gstopwatch.c `pkg-config --cflags --libs gtk+-2.0` -o gstopwatch

clean:
	rm -vf gstopwatch	
