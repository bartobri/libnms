LibNMS
======

This project is a compliment to my [no-more-secrets project](https://github.com/bartobri/no-more-secrets)
which provides a comamnd line tool for reproducing the famouse
"decrypting text" effect shown in the 1992 movie [Sneakers](https://www.youtube.com/watch?v=F5bAa6gFvLs&t=35).

This project provides a dynamically linkable library that provides a
simple interface for reproducing this effect in other projects.

This library relies on ANSI/VT100 escape sequences. Most modern terminal
programs support these sequences. If yours does not, you can install and
run this through an alternate terminal program like xTerm.

Effect example:

SCREEN SHOT HERE

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

License
-------

This program is free software; you can redistribute it and/or modify it under the terms of the the
MIT License (MIT). See [LICENSE](LICENSE) for more details.
