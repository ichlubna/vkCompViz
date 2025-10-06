# Simple Blending
This program can load two input images and blend them together with a given factor. The result is shown in a window and also stored in a file.
 
## Example usage
[Build](../../README.md) the examples. Then run in the `build` directory:

    cd examples/simpleBlending/; ./simpleBlending -i1 ../../../examples/simpleBlending/data/A.webp -i2 ../../../examples/simpleBlending/data/B.webp -f 0.5 -o ../../result.webp; cd -

or

    cd examples/simpleBlending/; ./simpleBlending -i1 ../../../examples/simpleBlending/data/A.png -i2 ../../../examples/simpleBlending/data/B.png -f 0.5 -o ../../result.png; cd -

or

    cd examples/simpleBlending/; ./simpleBlending -i1 ../../../examples/simpleBlending/data/A.exr -i2 ../../../examples/simpleBlending/data/B.exr -f 0.5 -o ../../result.exr; cd -
 

All images in the data directory were used to test the functionality of the example. The WEBP file stores 8-bit colors, PNG 16-bit, and EXR 32-bit floating point. Mouse wheel can be used to zoom the image.

