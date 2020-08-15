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

![Search](https://uc658b2938afaf1eb4646fca615a.previews.dropboxusercontent.com/p/thumb/AA6f-E4-EEPbZUuJL6f_ef2xLRP-YejTi5kK_bbIypxWoZUGYAl8iym2CHYki2aAreUxc0YSfTYpoRnZ7VKVxs4C0l8HbNGH-IfunG8I0NFjzIgbzd4-O2KAV-q-vqP3y6T7GpWpFh_zxGJqVdp8mxYsHgD1N5UOc9Apo9pCb4ALSx6VnTmgak9DxpvcrAph7nJjVhfPzvIw9c77UQEqaXXyJi11m3d-Vm5jOu-h9H78-9UwhVxW2q2yeAua4TLhOjum-YtCab5we39k4YhZdxNp_9MggM9nPuoy2A3isqPtuFq6EGo3pd3ger5GY4r_LFe9_wvcdfGe1nzXi6XGYTHQVVGFeje86NWMTQC3nfXlSQ/p.png?fv_content=true&size_mode=5)

![Strongs](https://uce3c6a92ec61ce9e995cdd2f859.previews.dropboxusercontent.com/p/thumb/AA6Fmo1ve87DjGW5-1j5rp01DN4JAiK6NbkThtRZ2oLAsGhMS7LM-7NY23HHtZB2uNF5lrF3eB0utJ26kbUbY7w7k_AOdol1ajgDnG7xatwp-vBvVj0_9C5xXlpmUIL_if9vMavZTwvj8znVGE9awz9ak5Rc2jc7KWd82TqoUmnRebTAflwImvmh3cRtHRsnmH1PJ1ARbn9GKa19UgHl-jtMTihxMYkTnADRQsWyBHZroXFiSA3RfljYV7MfsUm_7B-jCfz-NAz-NBxUnlSjs4Y10-VwQS7PyCNZ4nPGeLkGTvNFqO1pYzfvS4PPK1Cw9AHn_w8S1oi7jJpX1ggz3mrCBxokmVzMPszXLpXfK6FGyw/p.png?fv_content=true&size_mode=5)

![Today](https://uc537ade563bd3a478e89567a8c0.previews.dropboxusercontent.com/p/thumb/AA7IUGupgo3e0UAiwRQWU5tDJo3y8HYwvjNgV9X46LRmuqdNZwgV1sgaSrcBog13Ha5QcIsQ8KX-dR7vGfvzGerqSp4u7S47UOlle3Fg6JU7AwprUhQgcp5GZjYSb8SsbLjfVssShHjUAEM9kKqVNcOnxEf9sBp3_sWBfaxDjZFLU6nAy-prGUBjUGT-WMHtjVBvjtnotWu7eVPJdbXd7bRvSd2g4BwjI8ghrLQbQujVkI4fcw8PISbmsu9imymKcujVHInCNpyJdMnVfLD-v252SePOBRrc0iHiGvlHVqrkdOgq2XnouUUAo7LB_SlvzELlM5u0zWsXzMD6f7Xpn6wHzzRtLOVUwuLRtyFtxO5byA/p.png?fv_content=true&size_mode=5)

![Parallel](https://ucb63ca5fc21435550d2266d72a3.previews.dropboxusercontent.com/p/thumb/AA7Xq6YO1r32zcB0jGetO26L7W-sxL6BaL1T2qMM-PgZlu7xRJghDjFMohYLOu0NMuftzPMH0D1KKcAecA51lkSTwaDSi86OpbYGhVX9tiHJAxRixbKK7z3vyhqtz6pigjqi28suJC885zwxKPkiOturDwpBYKPL75MKsVw3YR2TiiKNZYvsU9KN1IMvGHwJ9TFl55-hNc_0TS0v-WvzaL1h8y9VDP9l82PldcCpi9q23vK8t3ZmprUXYWGMKkTViHczFjwn15ATPuPWiI9jDHGwVaa44UsETMkJpXn-vpLlZlg_Bxoi4e2moelEmsnNwVIIxLISyXEPJoW9m0kyy_jTpO_UUBI0oaI_hB9aMcnEvw/p.png?fv_content=true&size_mode=5)

