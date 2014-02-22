ApplyTag
========

A simple tool for append data to signed exe file without breaking the signature.
This tools adds data into WIN_CERTIFICATE section in exe file. This code has been ported from source Google Omaha. Works only with signed files.

This program is used on the official Simple Chat website for [change address](https://schat.me/addtag) of the default server.

Dependencies
------------
- Qt >= 4.8

Compilation
-----------
	qmake
	make

Usage
-----------
	ApplyTag.exe <signed file> <output file> <tag file>