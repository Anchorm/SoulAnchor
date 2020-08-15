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

![Contents](https://uc7f3fa1a6d0fd33a18c6739f8c2.previews.dropboxusercontent.com/p/thumb/AA5-jmoTPa290maFsSX9xgfgAy35UMT3d9yNIyrMhbzZnJpxCvFIJz6jp_kHULVwwxLpti2gfspYLx_4hH43bnfHCOIMP5QU5x7wNm572Y48jCu4BZZLbCjLQbgt4DJ3Fuz4x8Gla0VkoQ93XALVXFOwiFlYpjQGWVka-QjlDoUXH749RZ6YY6XOD9bI7HZS-ZDsM2Sz5YcZF1S9qyrJ4p_q7jrPXvN5iQGA3JJfCj5KRCc5L3HmmSRYRSrtg3RR-CQdjEJ122Vyf1Ay3OLFP0E0nZTHBN0HDpASCQcXaWnaAmbP7NGEtrlNOikytwl8HX8R_of2l55pnVsCjcZ14V6IER8lzTOmAgw6nNkP9pnohw/p.png?fv_content=true&size_mode=5)
![Search](https://www.dropbox.com/sh/jhuutnchr7pxru3/AADgsPEchOyjxPsTPfaKJ2ipa/SoulAnchor-screenshot-2.png)
![Strongs](https://www.dropbox.com/sh/jhuutnchr7pxru3/AACpFv4aEzwkYt2y7byfD4VGa/SoulAnchor-screenshot-3.png)
![Today](https://www.dropbox.com/sh/jhuutnchr7pxru3/AAB-jDJvLUN_IAPFlx5LwqWpa/SoulAnchor-screenshot-4.png)
![Parallel](https://www.dropbox.com/sh/jhuutnchr7pxru3/AADwpOxHMfJRZOrX6yD3_FFQa/SoulAnchor-screenshot-5.png)

