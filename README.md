LibNMS
======

This project provides a dynamically linked library that contains the
necessary functionality to recreate the famous data decryption effect
shown in the 1992 film [Sneakers](https://www.youtube.com/watch?v=F5bAa6gFvLs&t=35).

It's intended purpose is to be used for any software project whose author
may wish to use this effect when presenting data to the user. This library
also provides capabilities for getting user input in the case of a menu
or set of selections is presented to the user. See the [usage](#usage) section
for details.

This library has no dependencies, but it does rely on ANSI/VT100 escape
sequences to recreate the effect. Most modern terminal programs support
these sequences so this should not be an issue for most users. If yours
does not, you may need to use an alternate terminal program like xTerm.

If you wish to get an idea of how this looks or if it will work on your
system, without needing to implement this library inside a separate program,
you can download and install my other project, [no-more-secrets](https://github.com/bartobri/no-more-secrets),
which implements this same code in a command line tool that applies the
effect to piped data.

Screen Cap:

![Screenshot](http://www.brianbarto.info/extern/images/nms/nms.gif)

Table of Contents
-----------------

1. [Download and Install](#download-and-install)
2. [Usage](#usage)
3. [License](#license)

Download and Install
--------------------

In order to download and build this library, you will need to have `git`,
`gcc`, and `make` installed. Install them from your package manager if not
already installed.

```
$ which make
/usr/bin/make

$ which gcc
/usr/bin/gcc

$ which git
/usr/bin/git
```

Download and Install:

```
$ git clone https://github.com/bartobri/libnms.git
$ cd libnms
$ make
$ sudo make install
```

Uninstall:

```
$ sudo make uninstall
```

Usage
-----

**Synopsys**
```
include "nms.h"

int main(void) {

    // Apply the effect to the string "setec astronomy"
    nms_exec("setec astronomy");
    
    // Make the foreground color of the decrypted characters "red"
    nms_set_foreground_color("red");
    nms_exec("setec astronomy");
    
    // Initiate decryption sequence without requiring a key press
    nms_set_auto_decrypt(1);
    nms_exec("setec astronomy");
    
    // Clear screen prior to displaying any output
    nms_set_clear_scr(1);
    nms_exec("setec astronomy");
    
    // Do not use color (for terminals that don't support color)
    nms_use_color(0);
    nms_exec("setec astronomy");
    
    // Require the user to choose 1, 2, or 3 before returning execution to main()
    nms_set_return_opts("123");
    char c = nms_exec("Choose: [1] apples [2] oranges [3] pears");
    printf("User chose %c", c);
    
    // Set the cursor position to 0/2 (x/y) when getting user selection
    nms_set_input_position(0, 2);
    nms_set_return_opts("123");
    char c = nms_exec("Choose: [1] apples [2] oranges [3] pears");
    printf("User chose %c", c);
    
    return 0;
}
```

**Functions**

`char nms_exec(char *)`

The nms_exec() is the primary function for this library. It applies the
movie effect to a given string of characters. If nms_set_return_opts() is
used, this function will return the user selection. Otherwise it returns
zero.

This is the only function that must be called to generate the movie effect.
All other functions are used to taylor its behavior. Read below for more.

`void nms_set_foreground_color(char *)`

The nms_set_foreground_color() function sets the desired foreground color
of the decrypted characters displayed by nms_exec(). Valid arguments are
"white", "yellow", "black", "magenta", "blue", "green", "red" and "cyan".
Blue is the default. No value is returned.

`void nms_set_auto_decrypt(int)`

The nms_set_auto_decrypt() function sets a flag that nms_exec() uses to
initiate the decryption sequence without the need for the user to press a
key. If the integer argument is evaluated as true, the flag is turned on.
If false, it is turned off.

`void nms_set_clear_scr(int)`

The nms_set_clear_scr() function sets a flag that nms_exec() checks to
determine if it will clear the screen prior to displaying any output. If
the integer argument is evaluated as true, the flag is turned on.
If false, it is turned off. The flag is off by default. Note that the
screen contents prior to clearing are saved and restored once the effect
has completed.

`void nms_use_color(int)`

The nms_use_color() function sets a flag that nms_exec() checks to determine
if it will use color escape sequences when displaying unencrypted characters.
If the integer argument is evaluated as true, the flag is turned on.
If false, it is turned off.  The flag is on by default. This function exists
to support terminals that do not have color capabilities.

`void nms_set_return_opts(char *)`

The nms_set_return_opts() function sets the character options that nms_exec()
requires the user to choose from before it returns execution to the calling
function. This is intended to be used for cases where the string passed
to nms_exec() contains a menu with a set of selections. Note that nms_exec()
returns the selection to the calling function.

`void nms_set_input_position(int, int)`

The nms_set_input_position() function is meant to be used in conjunction with
the nms_set_return_opts() function. It sets the cursor position to the given x/y
coordinates when nms_exec() requires a menu selection from the user.

**Compiling**

You must tell the compiler to include the nms library.

```
gcc myprogram.c -lnms
```

License
-------

This program is free software; you can redistribute it and/or modify it under the terms of the the
MIT License (MIT). See [LICENSE](LICENSE) for more details.
