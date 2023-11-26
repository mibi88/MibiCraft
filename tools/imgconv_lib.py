"""
    Imgconv - converts image data in a header file.
    Copyright (C) 2022  Mibi88

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see https://www.gnu.org/licenses/.
"""

from PIL import Image

import os

def conv(infile, outheader, outsource, prefix = ""):
    # Get the name and the extension of the input file.
    name, ext = os.path.splitext(os.path.basename(infile))
    # If the input file is a png, we convert it.
    if ext == ".png":
        # We open the input file with Pillow
        img = Image.open(infile).convert("RGB")
        w, h = img.size
        # We create an array for the RGBA values of each pixel of the image.
        pixels = []
        # We loop through the image.
        for y in range(h):
            for x in range(w):
                # We get the pixel.
                pixel = img.getpixel((x, y))
                # We add his r, g and b color values to the pixel array.
                pixels.append(pixel[0])
                pixels.append(pixel[1])
                pixels.append(pixel[2])

        image_name = name

        # We generate the part of the header that is before the array of colors.
        out = f"""#include <{outheader}>

/* Image {name} converted with tools/imgconv_dir.py */

const unsigned char {image_name}_data[{w*h*3}] = {{
    """
        # We put the array of colors in out.
        for i in pixels:
            out += hex(i) + ", "
        out = out[:-2] # We remove the last ", " because nothing follows it.
        # We generate the end of the header.
        out += f"""
}};

const int {image_name}_width = {w};
const int {image_name}_height = {h};
"""
        # We write the header to the output file.
        with open(outsource, "w") as file:
            file.write(out)
            file.close()
        with open(outheader, "w") as file:
            file.write(f"""#ifndef {image_name.upper()}_H
#define {image_name.upper()}_H

extern const unsigned char {image_name}_data[{w*h*3}];

extern const int {image_name}_width;
extern const int {image_name}_height;

#endif
""")
            file.close()
    else:
        # We print an error message if the image is not a png because we are not
        # sure that we can open it.
        print("bad ext")
