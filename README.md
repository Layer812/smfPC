# smfPC 0.3
SMF (Standard MIDI File) Player for Cardputer and Cardputer-Adv<br>
<img width="50%" src ="https://github.com/user-attachments/assets/43961a28-a456-451b-917d-3326e87eaf99"><br>
Work with M5Stack Midi module [Unit-Synth](https://www.switch-science.com/products/9510?_pos=12&_sid=2a4ee9417&_ss=r).<br>
It works like [this](https://x.com/layer812/status/1890690659733921866)<br>
It can play [SMF files](https://en.wikipedia.org/wiki/MIDI) from a microSD card on the [Cardputer](https://shop.m5stack.com/products/m5stack-cardputer-kit-w-m5stamps3) or **Cardputer-Adv**.<br>

**v0.3 adds Cardputer-Adv compatibility**, preserving the v0.2 GUI, keyboard controls, MIDI output and SD file browser. The same source sketch supports both models via the M5Cardputer library's board auto-detection. MIDI playback has been confirmed on both Cardputer and Cardputer-Adv.

## Install
1.Install [M5burner](https://docs.m5stack.com/en/uiflow/m5burner/intro)<br>
2.Install **v0.3** from M5Burner using share code **`ytA202BfCmxyVRkZ`** (Cardputer / Cardputer-Adv). The previous share code `9NxQL96EOvAzWNNk` remains for **v0.2**.<br>
3.After flush the image, put SMF (.mid) files to your SD card (FAT32 formatted) and power on.<br>
## Usage
1.Push 'M' Button, you can see help.<br>
2.Select file by 'Cursor key'(Up/Down/Left/Right) without Fn. <br>
3.Hit 'Space Key' to Start/Stop playing.<br>
## Compile
1.Download all of codes into "smfPC" folder on ArduinoIDE as sketch.<br>
2.Select board type **M5Cardputer** (both Cardputer and Cardputer-Adv). Use the current M5Stack Arduino board package.<br>
3.Install **M5Cardputer 1.1.1 or newer** (with current M5Unified/M5GFX dependencies), **M5UnitSynth**, **MD_MIDIFile**, and **SdFat**.<br>
4.Compile and upload to either Cardputer or Cardputer-Adv. No GUI or key remapping is required.<br>
5.Connect Unit Synth to the **HY2.0-4P Grove port** (5V, GND, G1 RX, G2 TX). The sketch uses GPIO1 (UART RX) and GPIO2 (UART TX).<br>
6.On the original Cardputer, ensure its Grove **5V switch is set to output (5V OUT)** so the external Unit Synth is powered. The Cardputer's built-in speaker is not used for MIDI output.<br>

Both models use microSD SPI pins SCK=40, MISO=39, MOSI=14, CS=12. v0.3 initializes these explicitly, instead of assuming the generic `SS` pin.

**Arduino core 3.2.2 / GCC 14 note:** The upstream `MD_MIDIFile 2.6.0` library has a template type mismatch in `src/MD_MIDITrack.cpp` (`min(ARRAY_SIZE(mev.data), mLen)`). If your Arduino IDE reports this error, change that expression **inside the installed library** to `min((uint32_t)ARRAY_SIZE(mev.data), mLen)`. The CI workflow applies this single compatibility correction automatically; no application playback or GUI code is modified.

## Limitations
This software provided as No warranty.<br>
Files in the directory < 255, directory depth < 10. Path name length(include path name) < 255.<br>
File extention be needed as ".mid"<br>
Remain some bugs...<br>
## Version history
| Version  | Share Code | Change |
|:----------:|:-----------:|:-------------|
| 0.3       | ytA202BfCmxyVRkZ | Cardputer-Adv support; original Cardputer retained; hardware playback verified on both; GUI unchanged |
| 0.2       | 9NxQL96EOvAzWNNk      | Initial Commit     |

## License
- Please see the LICENSE file for more information.

## Thanks
Thanks to [@EuPhobos](https://github.com/Layer812/smfPC/issues/1) and the contributor of [issues #2](https://github.com/Layer812/smfPC/issues/2) and [#3](https://github.com/Layer812/smfPC/issues/3) (account now displayed as `ghost`) for their bug reports.<br>
Thanks for super work! [Zoltraak by Cosmic PIANO♪](https://www.youtube.com/watch?v=XzpTOdl-akg)<br>
