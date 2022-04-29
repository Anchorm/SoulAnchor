# SoulAnchor - X11 Bible Tool

*this hope we have as an anchor of the soul, a hope both sure and steadfast and one which enters within the veil*

&nbsp;

SoulAnchor is a Bible reading tool for the Linux desktop build with the Qt framework. A parallel window is available to compare translations. There's an option to display Strong's numbers and definitions. Also included are a few Bible dictionaries, cross references and topical links. 

* Basic but handy tool for daily use
* Bible reader first, light study tool second
* Open formats, open to modify, add/remove translations
* Multiple ways to contemplate the Word, text, audio, image

&nbsp;

### Installation

Arch Linux: use the PKGBUILD from the [AUR](https://aur.archlinux.org/packages/soulanchor "archlinux user depository")

Manually: `qmake6` , `make`, `make install`  
Note: if you keep the build directory you can later use `make uninstall DEL_DIR='rmdir -p'`  
Note2: the default install prefix is `/usr`. This can be changed with `qmake6 PREFIX=/dir`  
Note3: qmake6 is using qt6 which is required  

&nbsp;

![soulanchor screenshot](/../screenshots/soulanchor-01.png?raw=true "screenshot")
