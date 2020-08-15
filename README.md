# SoulAnchor - X11 Bible Tool

*this hope we have as an anchor of the soul, a hope both sure and steadfast and one which enters within the veil - Hebrews 6:19*

&nbsp;

SoulAnchor is a basic and free X11 Bible reading tool with some extra's. And available under the GPLv3.

The vision i have for SoulAnchor:
* Basic but handy tool for daily use
* Bible reader first, light study tool second
* Avoid distractions, keep it focused
* Open / easy to modify, add/remove translations
* Multiple ways to contemplate the Word, text, audio, image

&nbsp;

### Installation

For Arch Linux you can use the PKGBUILD from the AUR.
Dependencies: qt5-base, qt5-multimedia.

Details: since this is a Qt project, `qmake` is used. The soulanchor.pro file has a DESTDIR variable with the value `install/usr/bin`. After running `qmake` and `make`, the executable will be in that location. The install dir has the directories and files that need to be copied to the filesystem. There is no --prefix of make install for now.

SoulAnchor checks the XDG locations for application and user data & configuration directories. This would be for the data directories:
``"~/.local/share/<APPNAME>", "/usr/local/share/<APPNAME>", "/usr/share/<APPNAME>".`` And for the config: ``"~/.config/<APPNAME>", "/etc/xdg/<APPNAME>".``

&nbsp;

Although i examined the source material and have corrected those errors that i became aware of, i cannot guarantee the correctness of the translations and dictionaries. If you notice an error, please let me know. 

Anchorman - in the year of our Lord 2020

&nbsp;

![Contents](https://www.dropbox.com/sh/jhuutnchr7pxru3/AAA7rtTic8LdeUFTKEtsW2hPa?dl=0&preview=SoulAnchor-screenshot-1.png "Contents")
![Search](https://www.dropbox.com/sh/jhuutnchr7pxru3/AAA7rtTic8LdeUFTKEtsW2hPa?dl=0&preview=SoulAnchor-screenshot-2.png "Search")
![Strongs](https://www.dropbox.com/sh/jhuutnchr7pxru3/AAA7rtTic8LdeUFTKEtsW2hPa?dl=0&preview=SoulAnchor-screenshot-3.png "Strongs")
![Today](https://www.dropbox.com/sh/jhuutnchr7pxru3/AAA7rtTic8LdeUFTKEtsW2hPa?dl=0&preview=SoulAnchor-screenshot-4.png "Today")
![Parallel](https://www.dropbox.com/sh/jhuutnchr7pxru3/AAA7rtTic8LdeUFTKEtsW2hPa?dl=0&preview=SoulAnchor-screenshot-5.png "Parallel")

