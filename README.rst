:author: Federico Vaga <federico.vaga@vaga.pv.it>

LibUinput
=========

This is a very simple library for the Linux's uinput char device.

I found others libraries for ``uinput`` but I do not like them because of their
java oriented solution. I think that a library must be simple to use, easy
to understand and it must hide some complexity. The uinput interface is pretty
simple, so this library does not simplify nothing; the aim of this library is
to hide the ``ioctl()`` interface to the user of this library and provide some
common functions.
