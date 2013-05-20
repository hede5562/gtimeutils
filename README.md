GTimeUtils
=========

**A simple stopwatch and timer, written in GTK.**

Usage
-----

A small word about the usage of, for now, Gtimer. Gtimer can take four (five if you count the help message) command line arguments:

	-s, --seconds     Specify seconds to count down from
	-m, --minutes     Specify minutes to count down from
	-u, --hours       Specify hours to count down from
	-t, --text        Set an alternative notification text
	-r, --run         Immediately start the countdown

Installation
------------

GTimeUtils supports both GTK 2.x and GTK 3.x. You can choose which version to built at compile time. By default GTK 3.x will be used.
You'll need your chosen GTK version's headers with the libnotify and libcanberra headers installed and ready to go. To
build, simply run these commands:

	$ make -GTK3
	  or
	$ make -GTK2
	# make clean install

ToDo
----

* Gtimer: minimize to tray.

Bugs
----

For any bug or request [fill an issue][bug] on [GitHub][ghp].

  [bug]: https://github.com/Unia/gtimeutils/issues
  [ghp]: https://github.com/Unia/gtimeutils

License
-------

**GTimeUtils** is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**GTimeUtils** is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

**Copyright © 2013** Jente (jthidskes at outlook dot com)
