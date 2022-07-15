# simpleBMP

A bare metal C++ header only library, support 8bit & 24bit BMP image reading/writing.

## Features
* Open && Save BMP file (Only 8bit and 32bit)
* There is no so much abstraction, this library just read every bytes of BMP file into a structure.
    * You can reach every byte in bmp-file by using defined name.
    * You need to know the meaning of the defined names, the docstring in header may help you.
    * If you want to do more, read the specification of BMP from the link in the header file, and do as you want.
* Source code is very short and well doc-ed, read the code is more easier than readme.

# Installation: 
`#include "simpleBMP.h"`

# Usage:

```cpp
simpleBMP::BMPImg  img; // create a image
img.LoadImage("/path/to/img.bmp"); // load file
uint8_t * a = img.At(0,0) // Get pixel addr by index
uint8_t * a = img.AtLine(0) // Get pixel addr by line number
std::vector<uint8_t> c0 = img.GetChannel(0) // Get entire channel data by channel number
img.SaveImage("/path/to/img2.bmp"); // save to disk
```

> See [Otsu method sample](sample/otsu_method.cpp) for more

# Details:

* Every BMPImg instance consisted of four data member, as same as bmp-file
    * `file_header`: contains the basic info of bmp-file
    * `info_header`: contains the data info of bmp-file
    * `color_palette[256]`: only when info_header.biBitCount <= 8, will load or save this part
    * `pixel_data`: contains the pixel data of bmp-file
* Note
    * When using LoadImage(), info_header.biWidth and info_header.biHeight decide how much pixel to load
    * When using SaveImage(), you need to make sure `file_header` and `info_header` are both correct, you may
      build the debug version to let the assert help you checking.

> Silly word:

> It is really interesting that such a toy library get so "many" stars. It is good to see that it helps you, hope you enjoy it.