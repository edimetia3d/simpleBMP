# simpleBMP

* Open and save 8bit(24bit RBG) `.bmp`
* There is no so much abstraction, you need to know how bmp file orgnized.
    * I just save every bmpinfo into different variables.
    * You can reach every byte in bmpfile by using defined name
    * Pixal data are ranged as how they stored in disk.
* Source code is very short,read code is more easier than readme.

# Installation: 

* Just include "simplebmp.h"
* Need c++ `<vector>` supported.

# Useage:

```
char * path=....;
ClImgBMP imgvar; // create a image var
imgvar.LoadImage("Path"); // load a bmpfile
uint8_t a = imgvar.imgData[0]; // acces value
imgvar.SaveImage("Path"); // save to disk
```

> You can see more in OtsuMethod sample


# Details:

* Every ClImgBMP variable consited of four part,as same as bmpfile
    * `bmpFileHeaderData`
    * `bmpInfoHeaderData`
    * `colorMap[256]`,only when bmpInfoHeaderData.biBitCount == 8,will load or save this part
    * `imgData`
* Note
    * When you use LoadImage(),bmpInfoHeaderData.biWidth and bmpInfoHeaderData.biHeight decide how much pixal to load
    * When you use SaveImage(),function just save all four atrributes,So you have to make sure all value are matched,or you might fail to load next time.especially when you change the width or height of image.