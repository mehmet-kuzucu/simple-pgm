### Simple-PGM


This is a C++ program for compressing and decompressing PGM images. The program provides three types of image compression techniques: mean compression, binary run-length encoding, and color shifting. The program also supports image decompression to their original format.


## Requirements
This program requires the following libraries to be installed:

* iostream
* ctime
* string
* fstream


## How to use the program

The program is written in C++ and can be compiled and executed in any C++ compiler. Here are the steps to use the program:

1. Clone the repository to your local machine.
2. Compile the program using a C++ compiler.
3. To compress an image using mean compression technique, call the function mean_compression(string file_name, int kernel_rate = 3) with the image file name and kernel rate as input parameters. The kernel rate is optional and its default value is 3.
4. To decompress an image that was compressed using the mean compression technique, call the function mean_decoder(string file_name, string file_name_to_write) with the compressed file name and the name to write the decompressed image as input parameters.
5. To compress an image using binary run-length encoding technique, call the function binary_run_lenght_encoding(string file_name, int rate = 4) with the image file name and rate as input parameters. The rate is optional and its default value is 4.
6. To decompress an image that was compressed using the binary run-length encoding technique, call the function run_lenght_decoder(string file_name, string file_name_to_write) with the compressed file name and the name to write the decompressed image as input parameters.

## Example Usage
To compress an image using mean compression, call the mean_compression() function with the following arguments:

* file_name: The name of the input image file.
* kernel_rate (optional): The size of the kernel to be used for compression (default is 3).

```
Image img(file_name);
img.mean_compression(file_name, kernel_rate);
```

## License
This program is licensed under the MIT License.
