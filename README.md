# smfPC 0.2
SMF (standard midi file) Player for Cardputer
Work with M5Stack Midi module [Unit-Synth](https://www.switch-science.com/products/9510?_pos=12&_sid=2a4ee9417&_ss=r).
It works like [this](https://x.com/layer812/status/1890690659733921866)<br>
It can play [SMF files](https://en.wikipedia.org/wiki/MIDI) from SDcard in your [Cardputer](https://shop.m5stack.com/products/m5stack-cardputer-kit-w-m5stamps3).<br>
## Install
1.Install [M5burner](https://docs.m5stack.com/en/uiflow/m5burner/intro)<br>
2.You can find the firmware as "smfPC" on M5Burner or put share code '9NxQL96EOvAzWNNk ' in User Custom/Share on M5Burner<br>
3.After flush the image, put SMF (.mid) files to your SD card (FAT32 formatted) and power on.<br>
## Usage
1.Push 'M' Button, you can see help.<br>
<img width="50%" src ="https://github.com/user-attachments/assets/43961a28-a456-451b-917d-3326e87eaf99"><br>
2.Select file by 'Cursor key'(Up/Down/Left/Right) without Fn. Hit 'Space Key' to Start/Stop playing.<br>
## Compile
1.Download all of codes into "smfPC" folder on ArduinoIDE as sketch.<br>
2.Change board type to Cardputer<br>
3.Install librarys M5UnitSynth and MD_MIDIFile and SDFat.<br>
4.Compile and Upload to Cardputer<br>
## Limitations (things to do)
This software provided as No warranty.<br>
Limit numbers.<br>
- Files in the directory < 255, directory depth < 10. Path name length(include path name) < 255.
File extention be needed as ".mid"<br>
## Version history
| Version  | Share Code | Change |
|:----------:|:-----------:|:-------------|
| 0.2       | 9NxQL96EOvAzWNNk      | Initial Commit     |

## License
- Please see the LICENSE file for more information.
