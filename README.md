# YAVGMP
Yet Another [VGM](https://en.wikipedia.org/wiki/VGM_(file_format)) Parser/Player

## Installation

To compile and use YAVGMP, you'll need CMake, GCC and ZLib. On Ubuntu, they (plus Git too) can be installed using this command:
```
sudo apt-get install build-essential zlib1g-dev cmake git
```

This repository and its submodules can then be cloned using the following command:
```
git clone --recursive https://github.com/itsmevjnk/yavgmp.git
```

Once that's done, create a build output directory, run CMake, then Make the project:
```
mkdir -p yavgmp/build && cd yavgmp/build
cmake ..
make
```

## Usage
This project's "backend" consists of the following directories:
* `vgmcore`: The main code of the backend, including file parsing and emulator interface.
* `vgmchips`: An assortment of sound chip emulators. Currently supported chips are: SN76489, YM2413, and YM2149/AY-3-8910.

A couple of tools are also available from this project, which utilises the backend:
* `vgmcdump`: Dump a VGM file's audio output into files for each chip's channel.
* `vgminfo`: Dump information on a VGM file, and also parse its data section.
* `psgbench`: Benchmark comparison between two available SN76489 PSG emulator implementations (`emu76489` wrapper versus C++ port).
* `rconvtest`: Test for the `vgm_chip_rateconv` class, which provides a rate converter implementation based on that of `emu2149` that emulator wrappers can utilise.

### Demos
[Sega Master System V2.1 BIOS](https://youtu.be/gmXzw8NRtsA)

## Contributing
Pull requests (especially additions of tools and chip emulators) are welcome. If you have a suggestion for the project, or have an issue with any part of it, feel free to open an issue on this repository.

## License
[MIT](https://choosealicense.com/licenses/mit/)

Additionally, portions of the project's codebase may be subject to their respective licenses and restrictions; refer to their `LICENSE` files for details.