# Jpeg to RGB565 converter

Simple jpeg to 16bit rgb565 array converter tool for use in mcu projects.  Writen by <a href="https://github.com/gimbas">Rafael Silva</a>

This app reads a jpeg image file, and converts it to a 16bit rgb565 array, exports it in a file ready for inclusion in your mcu projects.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

- You will need root access to a Linux machine.
- You will need to install libjpeg
```
sudo apt-get install libjpeg9*
```

### Compiling

A step by step series of examples that tell you how to get the app running

- Clone this repository to your working directory

```
sudo git clone https://github.com/gimbas/cp2130
```

- Go to the source directory

```
cd jpeg-to-rgb565/
```

- Compile

```
make
```

See the [Makefile](Makefile) for more information on compile flags etc.

### Installing

- you may download the app from the releases if you don't want/need to compile it

- If you want to run the app from anywhere on your system you can put it(the binary) on /usr/local/bin for example
```
cp ./bin/jpegtorgb565 /usr/local/bin
```

- Depending on your user permissions, you might need to copy the compilled binary with root access (sudo)

## Running

- You will need a .jpeg image to convert.

```
jpegtorgb565 exampleImg.jpeg -o exampleImgArray
```
or if you want the output files in a given directory
```
jpegtorgb565 exampleImg.jpeg -o exampleImgArray -d destinationDirectory
```
- Depending on your user permissions, you might need to run the compilled binary with root access (sudo)

## Authors

* **Rafael Silva** - [gimbas](https://github.com/crying-face-emoji)

See also the list of [contributors](https://github.com/gimbas/cp2130/graphs/contributors) who participated in this project.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments
