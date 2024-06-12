# Chip emulators (and their wrappers) for `vgmcore`

The following chips are supported as of now:
* SN76489 PSG (`psg`, using [emu76489](https://github.com/digital-sound-antiques/emu76489))
* YM2413 (OPLL) (`opll`, using [emu2413](https://github.com/digital-sound-antiques/emu2413))
* YM3526 (OPL) / YM3812 (OPL2) / Y8950 (MSX-Audio) (`opl`, using [emu8950](https://github.com/digital-sound-antiques/emu8950))
* AY-3-8910 / YM2149 (SSG) (`ay8910`, using [emu2149](https://github.com/digital-sound-antiques/emu2149))
* Konami K051649 (SCC) / K052539 (SCC+) (`scc`, using [emu2212](https://github.com/digital-sound-antiques/emu2212))
* YM2612 (OPN2) / YM3438 (OPN2C) (`opn2`, using [NukedOPN2](https://github.com/nukeykt/Nuked-OPN2) (or more specifically [Wohlstand's mods for libOPNMIDI](https://github.com/Wohlstand/libOPNMIDI/blob/master/src/chips/nuked/ym3438.c)))

Refer to the repository root's README file for more details.
