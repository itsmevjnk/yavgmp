# vgmcdump
Dump channel and chip outputs from VGM files.

## Note
When mixing channels/chips' outputs using an audio editor software (eg. Audacity), set the gain setting for each chip as follows to ensure the outputs are balanced:

| Chip   | Volume factor (from `vgm_parser::chip_volumes`) | Volume factor (normalised; `0x100` = 1.0) | Volume (dB) | Notes |
|--------|----------------------------|-----------------------------------------|-------------|------|
|`psg`|`0x080`|0.5000|-6.02||
|`opll`|`0x100`|1.0000|0.00|Originally `0x200`|
|`opn2`|`0x100`|1.0000|0.00||
|`opm`|`0x100`|1.0000|0.00||
|`spcm`|`0x180`|1.5000|3.52||
|`rf68`|`0x0B0`|0.6875|-3.25||
|`opn`|`0x100`|1.0000|0.00||
|`opna`|`0x080`|0.5000|-6.02||
|`opnb`|`0x080`|0.5000|-6.02||
|`opl2`|`0x100`|1.0000|0.00||
|`opl`|`0x100`|1.0000|0.00||
|`msx`|`0x100`|1.0000|0.00||
|`opl3`|`0x100`|1.0000|0.00||
|`opl4`|`0x100`|1.0000|0.00||
|`opx`|`0x100`|1.0000|0.00||
|`pcmd8`|`0x098`|0.5938|-4.53||
|`rf164`|`0x080`|0.5000|-6.02||
|`pwm`|`0x0E0`|0.8750|-1.16||
|`ay8910`|`0x100`|1.0000|0.00||
|`dmg`|`0x0C0`|0.7500|-2.50||
|`apu`|`0x100`|1.0000|0.00||
|`mpcm`|`0x040`|0.2500|-12.04||
|`pd59`|`0x11E`|1.1172|0.96||
|`ok58`|`0x1C0`|1.7500|4.86||
|`ok95`|`0x100`|1.0000|0.00||
|`scc`|`0x0A0`|0.6250|-4.08||
|`k39`|`0x100`|1.0000|0.00||
|`hu`|`0x100`|1.0000|0.00||
|`c140`|`0x100`|1.0000|0.00||
|`k60`|`0x0B3`|0.6992|-3.11||
|`pokey`|`0x100`|1.0000|0.00||
|`qsound`|`0x100`|1.0000|0.00||
|`scsp`|`0x020`|0.1250|-18.06||
|`wswan`|`0x100`|1.0000|0.00||
|`vsu`|`0x100`|1.0000|0.00||
|`saa`|`0x100`|1.0000|0.00||
|`doc`|`0x040`|0.2500|-12.04||
|`otto`|`0x020`|0.1250|-18.06||
|`c352`|`0x100`|1.0000|0.00||
|`x1`|`0x040`|0.2500|-12.04||
|`ga20`|`0x280`|2.5000|7.96||
|`mikey`|`0x100`|1.0000|0.00||