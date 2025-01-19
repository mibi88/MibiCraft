.TL 1
Compiling MibiCraft on Windows XP

.LP
MibiCraft works correctly with zlib 1.3, libpng 1.6.40 and freeglut 1.8.1 (at
the time I'm writing the document, the 17/01/2025, with the latest commit being
commit 85ab576 "Add threading support for windows back" from the 15/01/2025)

.LP
Compiling MibiCraft is quite difficult on Windows XP. I recommend you to
compile it with OpenWatcom 1.6 (OpenWatcom 1.9 was quite buggy for me, so I
downgraded to OpenWatcom 1.6 which worked slightly better). MibiCraft can also
be built with gcc, but I wasn't able to compile my image conversion tool
imgconv with it.

.LP
First, you'll need to compile the imgconv tool.

.NH 1
Compiling imgconv

.LP
imgconv uses libpng to read the assets, which was quite challenging to build.

.NH 2
Compiling libpng on Windows XP

.LP
libpng provides files to compile it with the OpenWatcom IDE, but they require a
build of zlib which I wasn't able to build correcly.

.LP
That's why I created a static library project. I Added the following source
files to it :

.LP
After building the library by pressing F5, copy the generated .lib file to
C:\\WATCOM\\lib386\\nt and the libpng and zlib (I don't know if they are
required, but I also copied them) header files to C:\\WATCOM\\h\\nt (if you
installed the OpenWatcom C compiler at  C:\\WATCOM).

.NH 1
Compile MibiCraft

.NH 2
Compile freeglut

.LP
Compiling freeglut is straightforward. Compile freeglut 2.8.1 (or later if you
manage to). Copy the generated .lib file to C:\\WATCOM\\lib386\\nt and the
header files to C:\\WATCOM\\h\\nt (if you installed the OpenWatcom C compiler
at C:\\WATCOM).

.NH 2
Compile the imgconv tool and MibiCraft

.LP
Now just build imgconv. The compilation should succeed. Check that you've
followed the previous steps properly if it fails.

.LP
Now you should be able to compile MibiCraft easily.

.NH 1
Running MibiCraft

.LP
Copy freeglut.dll in the same folder and you should be able to run MibiCraft.


