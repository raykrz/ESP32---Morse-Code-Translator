# ESP32 Morse Code Translator

Tap Morse code on an ESP32's built-in **Boot** button and read the decoded text in the serial monitor. An LED lights up while the button is held, so you can see your dots and dashes.

```
Morse translator ready.
Short press = dot, long press = dash.
Pause about 1 second between letters, 2 seconds between words.
  .
  ..
  ...
... = S   Message: S
  -
  --
  ---
--- = O   Message: SO
  .
  ..
  ...
... = S   Message: SOS
```

## What you need

| Item | Notes |
| --- | --- |
| ESP32 dev board | Written for the classic ESP32-WROOM-32D DevKit (`esp32dev`). |
| USB data cable | Must carry data, not just power. |
| 1 LED | Any colour. |
| 1 resistor, about 200 Ω | Anything from 200 Ω to 330 Ω works. |
| Breadboard and 2 jumper wires | |

No external button is needed: the board's Boot button is already connected to GPIO 0.

## Wiring

```
GPIO 23 ──[ 200 Ω ]──►|── GND
                     LED
```

1. Connect **GPIO 23** to one end of the resistor.
2. Connect the other end of the resistor to the LED's **long leg** (anode).
3. Connect the LED's **short leg** (cathode) to **GND**.

## Software setup

You need [PlatformIO](https://platformio.org/). Pick one of the two options.

**Option A: VS Code (easiest)**

1. Install [VS Code](https://code.visualstudio.com/).
2. Open the Extensions panel and install **PlatformIO IDE**.

**Option B: command line**

```bash
pip install platformio
```

If your computer does not detect the board when you plug it in, install the [CP210x USB driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) (boards with a CH340 chip need the CH340 driver instead).

## Build and upload

1. Get the code:

   ```bash
   git clone https://github.com/raykrz/ESP32---Morse-Code-Translator.git
   ```

   ```bash
   cd ESP32---Morse-Code-Translator
   ```

2. Plug the ESP32 into your computer over USB.

3. Build, upload and open the serial monitor:

   ```bash
   pio run -t upload -t monitor
   ```

   In VS Code, open the folder and use the PlatformIO toolbar at the bottom instead: **→** (Upload), then the **plug** icon (Serial Monitor).

The first build downloads the ESP32 toolchain, which takes a few minutes. Later builds are fast.

When the upload finishes you should see `Morse translator ready.` in the monitor. If the monitor shows garbage characters, check that its speed is **115200** baud.

> **Upload stuck on `Connecting....`?** Hold the Boot button until the upload starts, then let go.

## How to use it

| Action | Result |
| --- | --- |
| Press shorter than 0.3 s | Dot (`.`) |
| Press of 0.3 s or longer | Dash (`-`) |
| Pause of 0.8 s | Ends the letter and prints it |
| Pause of 2 s | Ends the word and adds a space |

- The monitor prints the dots and dashes as you tap, then the decoded letter and the full message so far.
- A code that is not in the table is shown as `?`.
- The message holds up to 80 characters. Press the board's **EN** (reset) button to clear it and start again.

### Morse table

| | | | | | |
| --- | --- | --- | --- | --- | --- |
| A `.-` | B `-...` | C `-.-.` | D `-..` | E `.` | F `..-.` |
| G `--.` | H `....` | I `..` | J `.---` | K `-.-` | L `.-..` |
| M `--` | N `-.` | O `---` | P `.--.` | Q `--.-` | R `.-.` |
| S `...` | T `-` | U `..-` | V `...-` | W `.--` | X `-..-` |
| Y `-.--` | Z `--..` | 0 `-----` | 1 `.----` | 2 `..---` | 3 `...--` |
| 4 `....-` | 5 `.....` | 6 `-....` | 7 `--...` | 8 `---..` | 9 `----.` |

## Changing the settings

All settings are at the top of [`src/main.cpp`](src/main.cpp). Edit them and upload again.

| Setting | Default | Meaning |
| --- | --- | --- |
| `BUTTON_PIN` | `0` | The Boot button. Set to another pin (for example `19`) to use an external button wired between that pin and GND. |
| `LED_PIN` | `23` | Pin driving the LED. |
| `DASH_MIN_MS` | `300` | A press this long or longer counts as a dash. |
| `LETTER_GAP_MS` | `800` | A pause this long ends a letter. |
| `WORD_GAP_MS` | `2000` | A pause this long ends a word. |
| `DEBOUNCE_MS` | `20` | Button changes shorter than this are ignored. |

If you find the timing too fast, raise `DASH_MIN_MS`, `LETTER_GAP_MS` and `WORD_GAP_MS` together.

## Troubleshooting

| Problem | Fix |
| --- | --- |
| No serial port appears | Try another USB cable (many are charge-only) and install the USB driver linked above. |
| Upload fails with `Connecting....` | Hold the Boot button while the upload starts. |
| Monitor shows unreadable characters | Set the monitor speed to 115200 baud. |
| LED never lights | Flip the LED around: the long leg goes to the resistor, the short leg to GND. |
| Letters split apart while typing | You are pausing too long inside a letter. Tap faster or raise `LETTER_GAP_MS`. |
| Every press becomes a dash | Press more briefly or raise `DASH_MIN_MS`. |
| Board does not start after a reset | Do not hold the Boot button while pressing EN: that puts the ESP32 into upload mode. |

## Project layout

```
platformio.ini   Board, framework and serial monitor settings
src/main.cpp     The whole program
```
