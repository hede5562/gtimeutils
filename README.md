GTimeUtils
=========

**A simple stopwatch and timer, written in GTK3.**

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

GTimeUtils is written in GTK 3.x, so you need GTK3's headers with the libnotify and libcanberra headers installed and ready to go. To
build, simply run these commands:

	$ ./autogen.sh
	$ make
	# make clean install

Translations
-----------

You can help translate GTimeUtils in your language!
To do so, simply follow these steps:

	$ cd po
	$ intltool-update --pot
	$ mv gtimeutils.pot <language code>.po

Where `<language code>` is, obviously, the code of your language (e.g. `nl` for Dutch, `fr` for French, `en` for English...)
Edit the `LINGUAS` file and add your language code. Please keep the list alphabetically.
Lastly, open the .po file you just generated and translate all the strings. Don't forget to fill in the information in the header!

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

**Copyright © 2013** Jente Hidskes <jthidskes@outlook.com>
