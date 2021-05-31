# SoulAnchor - X11 Bible Tool

*this hope we have as an anchor of the soul, a hope both sure and steadfast and one which enters within the veil*

&nbsp;

SoulAnchor is a Bible reading tool for the Linux desktop build with the Qt framework. For reading, searching and comparing the Scriptures. A parallel window is available to compare translations. There's an option to display Strong's numbers and definitions. Also included are a few Bible dictionaries. And it's possible to use cross references and topical links. 

The vision i have for SoulAnchor:
* Basic but handy tool for daily use
* Bible reader first, light study tool second
* Open formats, easy to modify, add/remove translations
* Multiple ways to contemplate the Word, text, audio, image

&nbsp;

### Installation

For Arch Linux you can use the PKGBUILD from the AUR.
Dependencies: qt5-base, qt5-multimedia.

Manually: After running `qmake` and `make`, use `sudo make install`. If you keep the build directory you can later use `sudo make uninstall DEL_DIR='rmdir -p'`.  
Note: the default install prefix is `/usr`. This can be changed with `qmake PREFIX=/dir`  
Note2: you might want to check soulanchor.pro to see if the install directories match your distro's default locations (the .path lines)  

&nbsp;

![soulanchor 01](/../screenshots/soulanchor-01.png?raw=true "Content tab, parallel window")
![soulanchor 02](/../screenshots/soulanchor-02.png?raw=true "Search tab")
![soulanchor 03](/../screenshots/soulanchor-03.png?raw=true "Today, Settings")
![soulanchor 04](/../screenshots/soulanchor-04.png?raw=true "Cross references")
![soulanchor 05](/../screenshots/soulanchor-05.png?raw=true "Strongs")
