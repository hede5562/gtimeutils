gstopwatch:
	gcc gstopwatch.c `pkg-config --cflags --libs gtk+-3.0` -o gstopwatch

clean:
	rm -f gstopwatch	
