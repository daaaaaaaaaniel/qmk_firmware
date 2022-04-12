/* Copyright 2015-2021 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#pragma once

/* USB Device descriptor parameter */
#define DEVICE_VER 0x0003
#define PRODUCT_ID 0xA649
#undef MANUFACTURER
#define MANUFACTURER Drop

#undef MATRIX_ROWS
#undef MATRIX_COLS
/* key matrix size */
#define MATRIX_ROWS 10
#define MATRIX_COLS 6

#undef MATRIX_ROW_PINS
#undef MATRIX_COL_PINS
#define MATRIX_ROW_PINS { A10, A9, A8, B15, C13, C14, C15, A2, A3, A6 }
#define MATRIX_COL_PINS { B11, B10, B2, B1, A7, B0 }
#define UNUSED_PINS

// #define ENCODERS_PAD_A { B12 }
// #define ENCODERS_PAD_B { B13 }
// 
// #define DIP_SWITCH_PINS { B14, A15, A0, B9 }

#define MUSIC_MAP
#undef AUDIO_VOICES
#undef AUDIO_PIN
#define AUDIO_PIN A5
#define AUDIO_PIN_ALT A4
#define AUDIO_PIN_ALT_AS_NEGATIVE

// /*
//  * WS2812 Underglow Matrix options
//  */
// #define RGB_DI_PIN A1
// #define RGBLED_NUM 9
// #define RGBLIGHT_ANIMATIONS
// // RGB Matrix support
// #define DRIVER_LED_TOTAL RGBLED_NUM
// 
// #define WS2812_PWM_DRIVER PWMD2
// #define WS2812_PWM_CHANNEL 2
// #define WS2812_PWM_PAL_MODE 1
// #define WS2812_DMA_STREAM STM32_DMA1_STREAM2
// #define WS2812_DMA_CHANNEL 2


////// All preceding code cloned from qmk_firmware/keyboards/preonic/rev3_drop/config.h //////


#ifdef AUDIO_ENABLE
#    define STARTUP_SONG SONG(PREONIC_SOUND)
// #define STARTUP_SONG SONG(NO_SOUND)

#    define DEFAULT_LAYER_SONGS \
        { SONG(QWERTY_SOUND), SONG(COLEMAK_SOUND), SONG(DVORAK_SOUND) }
#endif

// #define MUSIC_MASK (keycode != KC_NO)

/*
 * MIDI options
 */

/* enable basic MIDI features:
   - MIDI notes can be sent when in Music mode is on
*/
// #define MIDI_BASIC

/* enable advanced MIDI features:
   - MIDI notes can be added to the keymap
   - Octave shift and transpose
   - Virtual sustain, portamento, and modulation wheel
   - etc.
*/
#define MIDI_ADVANCED


/* override number of MIDI tone keycodes (each octave adds 12 keycodes and allocates 12 bytes) */
//#define MIDI_TONE_KEYCODE_OCTAVES 2


// reduce advertised power consumption for use with iOS/iPadOS devices
#define USB_MAX_POWER_CONSUMPTION 100

// prevent acccidentally triggered modifer keys when interrupting a mod tap key.
#define IGNORE_MOD_TAP_INTERRUPT

// for rapidly typing sequences like MT(SHIFT) -> i -> release i -> release MT(SHIFT), which should ideally output a capital I. This option fixes it so that it won't trigger the mod key's tap functionality in this scenario.
#define PERMISSIVE_HOLD


/* //disables macOS FN key to allow compatibility with Vial etc. If using Karabiner, remap Caps Locks to Apple FN, or adjust the Modifier Keys in Systems Preferences so that Caps Locks triggers FN.
// enables macOS FN key
#undef PRODUCT_ID
#define PRODUCT_ID      0x029c
#undef VENDOR_ID
#define VENDOR_ID       0x05ac
#undef PRODUCT
#define PRODUCT         Preonic Rev3
#define APPLE_FN_ENABLE
*/


// How many combos?
#define COMBO_COUNT 1


// Mouse controls
#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MAX_SPEED 8
#define MOUSEKEY_MOVE_DELTA 3
#define MOUSEKEY_TIME_TO_MAX 60
