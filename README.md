Gstopwatch 
=========

**A simple stopwatch, written in GTK3.**

Okay, so apparently there are absolutely zero good stopwatches around for X11!
I spent my time searching but couldn't find one simple enough for me. This
stopwatch will start counting, stop counting and reset.

Usage 
-----

The spacebar does it all! Press space to start timing, space to pause, space again to continue and r to reset the timer when it is paused.

Installation
------------

You'll need the GTK 3.x libraries and headers installed and ready to go. To
build, simply run these commands:

	$ make
	$ make clean install

Bugs
----

For any bug or request [fill an issue][bug] on [GitHub][ghp].

  [bug]: https://github.com/Unia/gstopwatch/issues
  [ghp]: https://github.com/Unia/gstopwatch

ToDo 
----

* Display text like gnome-clocks?
* Font size depend on available space?
* Add about dialog back in?
* Add a timer function too, in GtkNotebook?
* Keyboard shortcut to add/delete laps?
* Clever lap deleting

License
-------

**Gstopwatch** is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**Gstopwatch** is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

**Copyright © 2013** Jente (jthidskes at outlook dot com)
