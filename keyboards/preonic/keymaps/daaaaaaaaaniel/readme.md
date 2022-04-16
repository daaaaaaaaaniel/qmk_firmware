# Daniel's daily driver
An Apple-centric keyboard
 
### Features

* Two space bars!
	* Send ENTER by hitting both at the same time.
	* Hold one or the other space bar to change layers.

* Ergonomically send BACKSPACE and DELETE using right thumb keys.

* Navigate and select text faster with special whole-word arrow keys (located next to the regular arrow keys).

- [x] Improved iPad compatibility
- [x] Toggle 4-octave MIDI keyboard layer
- [x] Support for macOS consumer keys
- [x] Add mouse key support
- [x] Navigate through browser tabs without chording

**NOTE:** (Recommended) Use either Karabiner (macOS) and/or System Preferences (macOS/iPadOS) to map Caps Lock to Fn (Globe key).


QWERTY (Default Layer)
----------------------
* Raise and Lower keys each act as Space when tapped. Tapping them together sends Enter.
* Left Shift acts as Escape when tapped.
* Enter acts as Right Shift when held.
* Right-hand modifiers: Bksp acts as Right Command when held; Del acts as Right Option when held; [ acts as Right Control when held.
* When holding the Tab key, tapping each of the space bars switches to the previous or next tab in the active window.[^1]

[^1]: Tab navigation uses its own layer with all transparent keys aside from the two space bars, which move to the next/previous tab.

```
/* Qwerty (Default Layer)
 * ,-----------------------------------------------------------------------------------.
 * |  Esc |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   `  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  | Ctrl | Opt  | Cmd  |     Lower   |    Raise    | Bksp |  Del |   [  |   ]  |
 * `-----------------------------------------------------------------------------------'
 */
```

Lower (Symbols)
---------------
* Toggle on the Raise layer by pressing the Raise (Lock) key.
* Tap the MIDI key to toggle on the MIDI layer.

```
/* Lower (Symbols) - holding Left Space
 * ,-----------------------------------------------------------------------------------.
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |   ~  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |AppSwi|   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |   {  |   }  |   [  |   ]  |   \  |  |   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | MIDI |      |      |      |             | Raise (Lock)|      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
```
 
Raise (Navigation) 
------------------
* WASD controls the mouse.
* WrdFwd and WrdBck move the insertion point forward and back by one whole word (effectively a combo of Option+Left/Right arrow keys).
* LinSrt and LinEnd move the insertion point to the beginning and end of the line (effectively a combo of Command+Left/Right arrow keys).
* Sel →,Sel ←, WoSel→ and WoSel← expand/contract the text selection by one character or one full word in either direction (effectively a combo of (Option+)Shift+Left/Right arrow keys).
* SwapCh swaps the position of the characters to the left and right of the insertion point.
* Toggle on the Lower layer by pressing the Lower (Lock) key.

```
/* Raise (Navigation) - holding Right Space
 * ,-----------------------------------------------------------------------------------.
 * |LockSc|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute | Ctrl |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |  MUp | Click|RClick|SwapCh|WoSel←| Sel ←|  Up  | Sel →|WoSel→| PgUp |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      | MLeft| MDown|MRight| Click|      |WrdBck| Left | Shift|Right |WrdFwd| PgDwn|
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      | Cut  | Copy | Paste|      |LinSrt| Desel| Down |      |LinEnd| Shift|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      | Lower (Lock)|             |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
```

Extra (Media/Functions)
-----------------------
* Tap and hold both spacebars together to momentarily turn on the Extra layer. 

```
/* Extra Layer (Functions) - holding Lower + Raise
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |AppSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      | Next | Vol- | Vol+ |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |             |             |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
```
 
MIDI
----
* Tap the Qwerty key to exit the MIDI layer.

```
,-----------------------------------------------------------------------------------.
|  C0  |  D♭0 |  D0  |  E♭0 |  E0  |  F0  |  G♭0 |  G0  |  A♭0 |  A0  |  B♭0 |  B0  |
|------+------+------+------+------+------+------+------+------+------+------+------|
|  C1  |  D♭1 |  D1  |  E♭1 |  E1  |  F1  |  G♭1 |  G1  |  A♭1 |  A1  |  B♭1 |  B1  |
|------+------+------+------+------+-------------+------+------+------+------+------|
|  C2  |  D♭2 |  D2  |  E♭2 |  E2  |  F2  |  G♭2 |  G2  |  A♭2 |  A2  |  B♭2 |  B2  |
|------+------+------+------+------+------+------+------+------+------+------+------|
|  C3  |  D♭3 |  D3  |  E♭3 |  E3  |  F3  |  G♭3 |  G3  |  A♭3 |  A3  |  B♭3 |  B3  |
|------+------+------+------+------+------+------+------+------+------+------+------|
|Qwerty|      | Vel-1| Vel+1| Pitch Bend+ | Pitch Bend- |Oct-1 |Oct+1 |Note-1|Note+1|
`-----------------------------------------------------------------------------------'
```
