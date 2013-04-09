pkgname=gstopwatch-git
pkgbase=gstopwatch
pkgver=2013.03.28
pkgrel=1
pkgdesc="A simple stopwatch, written in GTK3."
arch=(any)
url="https://github.com/Unia/gstopwatch"
license=(GPL)
depends=('gtk3')

_gitroot="https://github.com/Unia/$pkgbase"
_gitname="$pkgbase"

pkgver() {
    cd "$srcdir/$_gitname"
    git log -1 --format="%cd" --date=short | sed 's|-|.|g'
}

build() {
	cd "$srcdir"
	msg "Connecting to GIT server..."

	if [ -d ${_gitname} ] ; then
		cd ${_gitname}/
		git pull
		msg "The local files are updated."
	else
		git clone ${_gitroot} ${_gitname}
		cd ${_gitname}/
	fi
	msg "GIT checkout done or server timeout"

	make
}

package() {
	cd "$srcdir/$pkgbase/"

	make DESTDIR="$pkgdir" install
}
