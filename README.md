# simpleBMP
 * open and save 8bit(24bit RBG) .bmp
 * there is no so much abstraction,you need to know how bmp file orgnized.
 * I just save every bmpinfo into different variables.
 * source code is very short,read code is more easier than readme.

 > 1.you can reach every byte in bmpfile by using defined name
 
 eg. img.bmpFileHeaderData.biWidth
 
 > 2.pixal data are ranged like how they store in disk.

# Installation: 
 * 1.just include "simplebmp.h"
 * 2.Need c++ "vector" supported.

# Useage:
 * ClImgBMP imgvar; -- create a image var
 * imgvar.LoadImage("Path") -- load a bmpfile
 * imgvar.SaveImage("Path") -- save to disk


# details:
## every ClImgBMP variable consited of four part,as same as bmpfile
 * bmpFileHeaderData
 * bmpInfoHeaderData
 * colorMap[256]//only when bmpInfoHeaderData.biBitCount == 8,will load or save this part
 * imgData

 when you use LoadImage(),bmpInfoHeaderData.biWidth and bmpInfoHeaderData.biHeight decide how much pixal to load
 
 when you use SaveImage(),function just save all four atrributes
 
 you have to make sure all value are matched,or you might fail to load next time.especially when you change the width or height of image.
 
