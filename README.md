# Utah Teapot for 16 Bit Real Mode MSDOS

3D rendering of a Utah Teapot for MSDOS.

Based on [Rebecca Cran's Utah Teapot for SDL1.2](https://github.com/bcran/teapot)

<img src='https://github.com/astriiddev/teapot-MSDOS/assets/98296288/d30b3117-ba72-41d0-95c9-54cabd76b101' width='600'>  <br />

https://github.com/astriiddev/teapot-MSDOS/assets/98296288/bef4b896-c138-409c-a600-b2c9eae0b86e

Fun excerize in low-level 3D graphics :)

# Building
This has been tested in DOSBox-X as well as on my own 66MHz 486. 
Builds tested with Borland Turbo C++, DJGPP 2.02, and Watcom C/C++ 9.5.

To build, rename either the `GMF` (GNU Makefile), `TMF` (Turbo C Makefile), or `WMF` (Watcom Makefile) to `MAKEFILE` and run `make` (`wmake` for Watcom).  

Alternatively, run:
```
make -f gmf
```
```
make -f tmf
```
or
```
wmake -f wmf
```

If you have more than one compiler on your DOS system, you will have to set up your own way to differentiate between the different versions of MAKE.

Recommended to use DOSBox-X rather than the standard version of DOSBox for building and running.
