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

#include QMK_KEYBOARD_H
#include "muse.h"

// defining my custom variables. i use them for toggling on the Raise and Lower layers.
bool is_lower_key_held = false;
bool is_raise_key_held = false;
bool is_layer_toggled_on = false;

/* declare layers */
enum preonic_layers {
  _QWERTY,
  _SYM,
  _TAB,
  _EXTRA,
//   _CMD1,
  _MIDI
};

/* declare special custom key codes */
enum preonic_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  MIDI,
  KC_MISSION_CONTROL, // _AC_SHOW_ALL_WINDOWS // AC Desktop Show All Windows
  KC_SPOTLIGHT,
  KC_DICTATION,
  KC_DO_NOT_DISTURB,
  KC_LOCK_SCREEN,
  KC_LAUNCHPAD, // _AC_SHOW_ALL_APPS // AC Desktop Show All Applications 
//   TEXT_DESELECT,
  TEXT_SELECT_WORD,
//   AA_RCMD,
//   AA_LCMD
};

#define AA_RCMD RCMD_T(KC_BSPC) // command (hold); backspace (tap)
#define AA_ROPT ROPT_T(KC_DEL) // option (hold); delete (tap)
#define AA_RCTL RCTL_T(KC_LBRC)
#define AA_RSFT LM(_QWERTY,MOD_RSFT), // idk this doesn't quite work  /* alt version: // OSM(MOD_RSFT) // one-shot right shift */
#define SX_ESC SFT_T(KC_ESC) // shift (hold); escape (tap)
#define DD_CMD LCMD_T(KC_DEL) // command (hold); delete (tap)
#define DD_BSPC ROPT_T(KC_BSPC) // option (hold); backspace (tap)
#define KX_CUT LCMD(KC_X) // command-x
#define KX_COPY LCMD(KC_C) // command-c
#define KX_PSTE LCMD(KC_V) // command-v
#define SHOW_ALL_APP_WINDOWS LCTL(KC_DOWN) // Control-Down Arrow
#define NEXTTAB SCMD(KC_RBRC) // Command-Shift-] (or perhaps Control-Tab)
#define PREVTAB SCMD(KC_LBRC) // Command-Shift-[ (or perhaps Control-Shift-Tab)
// #define NEXT_WORD_DELETE LOPT(KC_DEL) // Option-Delete
// #define PREVIOUS_WORD_BACKSPACE LOPT(KC_BSPC) // Option-Backspace
#define NEXT_WORD LOPT(KC_RGHT) // Option-Right Arrow. Alternatively, use Opt-Cmd-F (or a custom keybind in DefaultKeyBinding.dict).
#define PREVIOUS_WORD LOPT(KC_LEFT) // Option-Left Arrow. Alternatively, use Opt-Cmd-B (or a custom keybind in DefaultKeyBinding.dict).
#define SELECT_NEXT_CHARACTER LSFT(KC_RGHT) // Shift-Right Arrow
#define SELECT_PREVIOUS_CHARACTER LSFT(KC_LEFT) // Shift-Left Arrow
#define SELECT_NEXT_WORD LSA(KC_RGHT) // Shift-Option-Right Arrow
#define SELECT_PREVIOUS_WORD LSA(KC_LEFT) // Shift-Option-Left Arrow
#define LINE_START G(KC_LEFT) // Command-Left Arrow
#define LINE_END G(KC_RGHT) // Command-Right Arrow
#define KX_SWAP LCTL(KC_T) // swap characters adjacent to insersion point

/* short names / aliases */
// #define KX_PALT LCMD(LSFT(KC_X))
#define KC_MCTL KC_MISSION_CONTROL
#define KC_SPLT KC_SPOTLIGHT
#define KC_DICT KC_DICTATION
#define KC_DOND KC_DO_NOT_DISTURB
#define KC_LOCK KC_LOCK_SCREEN
#define KC_LPAD KC_LAUNCHPAD
#define ALL_APP SHOW_ALL_APP_WINDOWS
// #define WD_DEL NEXT_WORD_DELETE
// #define WD_BSPC PREVIOUS_WORD_BACKSPACE
#define WD_NEXT NEXT_WORD
#define WD_PREV PREVIOUS_WORD
#define SEL_NXT SELECT_NEXT_CHARACTER
#define SEL_PRV SELECT_PREVIOUS_CHARACTER
#define SELWNXT SELECT_NEXT_WORD
#define SELWPRV SELECT_PREVIOUS_WORD
#define LN_END  LINE_END
#define LN_STRT LINE_START
// #define TX_DSEL TEXT_DESELECT
#define TX_SEL  TEXT_SELECT_WORD
/* keycodes for moving between layers */
#define AA_TAB  LT(_TAB, KC_TAB)
#define AA_RSPC  LT(_SYM,KC_SPACE)
#define AA_LSPC  LT(_SYM,KC_ENT)
/* keycodes for Amethyst */
#define AM_MOD1 LM(_QWERTY, MOD_LALT | MOD_LSFT)
#define AM_MOD2 LM(_QWERTY, MOD_MEH) // Control + Option + Shift 
#define AA_GRV  LSA_T(KC_GRV)
#define AA_Q    LCTL_T(KC_Q)


/* Sounds */
#ifdef AUDIO_ENABLE
  float song_preonic_sound[][2] = SONG(PREONIC_SOUND); // song for switching into MIDI layer 
  float song_plover_gb[][2]  = SONG(PLOVER_GOODBYE_SOUND);  // song for switching out of MIDI layer (back to QWERTY)
#endif

/* Combos */
// When both SPACE keys are tapped together, execute ENTER. When both SPACE keys are HELD, activate _EXTRA layer.
enum combos {
  CMD_SELECT_ALL,
};

const uint16_t PROGMEM adj_combo[] = {KC_LCMD, AA_RCMD, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
 [CMD_SELECT_ALL] = COMBO(adj_combo, LT(_EXTRA, KC_CAPS)), // HITTING anf HOLDING both Cmd keys together activates _EXTRA layer (Combo that executes a layer tap). TAPPING both Cmd keys together triggers Caps Lock.
 // HITTING and HOLDING both space bars together activates _EXTRA layer (Combo that executes a layer tap), although concievably it could activate any layer, thus making holding the COMBO different from pressing the keys at different times and holding them. 
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* 
NOTE
I'm chaning the _RAISE and _LOWER layers. In the next `push`, I'm only having QWERTY and one other "special" layer for numbers/symbols. The Nav layer will be accessed through the pinkies, not the thumbs.
*/

 /* QWERTY (Default layer)
 * ,-----------------------------------------------------------------------------------.
 * |  Esc |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   `  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  | Shift|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  | Ctrl | Opt  | Cmd  |     Lower   |    Raise    | Bksp |  Del |   [  |   ]  |
 * `-----------------------------------------------------------------------------------'
 */
[_QWERTY] = LAYOUT_preonic_2x2u(
  KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  AA_GRV,  AA_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  AA_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, LT(_TAB, KC_QUOT),
  SX_ESC,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, AA_RSFT,
  KC_CAPS, KC_LCTL, KC_LOPT, KC_LCMD,     AA_LSPC,          AA_RSPC,      AA_RCMD, AA_ROPT, AA_RCTL, KC_RBRC
),

/* Numbers and Symbols - holding Space
 * ,-----------------------------------------------------------------------------------.
 * |LockSc|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |  F12 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |   ~  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |AppSwi|   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  Esc |      |      |      |      |   |  |   {  |   }  |   [  |   ]  |  Up  |  \   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | MIDI |      |      |      |             |             |  Del | Left | Down |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_SYM] = LAYOUT_preonic_2x2u(
  KC_LOCK, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, KC_F12,
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PLUS,
  ALL_APP, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  _______,TO(_QWERTY),KX_CUT,KX_COPY, KX_PSTE, KC_PIPE, KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC, KC_UP,   KC_BSLS,
  MIDI,   TO(_SYM), _______, _______, LT(_SYM,KC_ESC), LT(_SYM,KC_ESC), RCMD_T(KC_DEL), KC_LEFT, KC_DOWN, KC_RGHT
),

/* Tab (Window Managment layer) - holding Tab
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |AmMod2| Shift|      |      |SwapCh|WrdSel|      |      |PrvTab|NxtTab|      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      | Shift| Opt  | Cmd  |      |      | Left | Down |  Up  |Right | Opt  |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      | Cut  | Copy | Paste|      |      | PgUp | PgDwn|      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty|(Lock)|      |      |             |             |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_TAB] = LAYOUT_preonic_2x2u( // NOTE: add a tap dance routine to LineStart/LineEnd such that tapping once goes to the start of the line and each additional tap goes up/down by a line !! this will make it feel more like clockwise/counterclockwise movement
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, AM_MOD2, KC_LSFT, KC_NO,   KC_NO,   KX_SWAP, TX_SEL,  KC_NO,   KC_NO,   PREVTAB, NEXTTAB, _______,
  MO(_TAB),KC_LSFT, KC_LOPT, KC_LCMD, KC_NO,   KC_NO,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_ROPT, MO(_TAB),
  _______, KC_NO,   KX_CUT,  KX_COPY, KX_PSTE, KC_NO,   KC_NO,   KC_PGDN, KC_PGUP, KC_NO,   KC_NO,   _______,
  TO(_QWERTY),TO(_TAB), _______, DD_CMD,   _______,         _______,      _______, _______, _______, _______
),

/* Extra (Media Functions layer) - holding Left Space + Right Space || or in 1x2u layout, access via layers CMD1+CMD2
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |AppSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      | Next | Vol- | Vol+ |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |(Lock)|      |      |             |             |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_EXTRA] = LAYOUT_preonic_2x2u(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, _______,
  _______, KC_NO,   KX_CUT,  KX_COPY, KX_PSTE, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_MNXT, KC_VOLD, KC_VOLU,
  TO(_QWERTY),TO(_EXTRA), _______, _______,  _______,    _______,      _______, _______, _______, _______
),

// /* CMD1 (Empty - this is just for accessing a tri-layer when CMD1+CMD2 are active simultaneously)
//  */
// [_CMD1] = LAYOUT_preonic_2x2u(
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______,      _______,         _______,      _______, _______, _______, _______
// ),


/* MIDI
 * ,-----------------------------------------------------------------------------------.
 * |  C0  |  D♭0 |  D0  |  E♭0 |  E0  |  F0  |  G♭0 |  G0  |  A♭0 |  A0  |  B♭0 |  B0  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  C1  |  D♭1 |  D1  |  E♭1 |  E1  |  F1  |  G♭1 |  G1  |  A♭1 |  A1  |  B♭1 |  B1  |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |  C2  |  D♭2 |  D2  |  E♭2 |  E2  |  F2  |  G♭2 |  G2  |  A♭2 |  A2  |  B♭2 |  B2  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  C3  |  D♭3 |  D3  |  E♭3 |  E3  |  F3  |  G♭3 |  G3  |  A♭3 |  A3  |  B♭3 |  B3  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty|      | Vel-1| Vel+1| Pitch Bend+ | Pitch Bend- |Oct-1 |Oct+1 |Note-1|Note+1|
 * `-----------------------------------------------------------------------------------'
 */
[_MIDI] = LAYOUT_preonic_2x2u(
  MI_C,    MI_Db,   MI_D,    MI_Eb,   MI_E,    MI_F,    MI_Gb,   MI_G,    MI_Ab,   MI_A,    MI_Bb,   MI_B,
  MI_C_1,  MI_Db_1, MI_D_1,  MI_Eb_1, MI_E_1,  MI_F_1,  MI_Gb_1, MI_G_1,  MI_Ab_1, MI_A_1,  MI_Bb_1, MI_B_1,
  MI_C_2,  MI_Db_2, MI_D_2,  MI_Eb_2, MI_E_2,  MI_F_2,  MI_Gb_2, MI_G_2,  MI_Ab_2, MI_A_2,  MI_Bb_2, MI_B_2,
  MI_C_3,  MI_Db_3, MI_D_3,  MI_Eb_3, MI_E_3,  MI_F_3,  MI_Gb_3, MI_G_3,  MI_Ab_3, MI_A_3,  MI_Bb_3, MI_B_3,
  QWERTY, _______,  MI_VELD, MI_VELU,     MI_BENDD,         MI_BENDU,     MI_OCTD, MI_OCTU, MI_TRNSD, MI_TRNSU
)

};

// // i think this is what made it possible to distingish between the combo and the staggered pressing of both space keys?
// layer_state_t layer_state_set_user(layer_state_t state) {
//   return update_tri_layer_state(state, _LOWER, _RAISE, _EXTRA);
// }


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
        case QWERTY:
          if (record->event.pressed) {
            // set_single_persistent_default_layer(_QWERTY);
            layer_off(_MIDI);
            layer_move(_QWERTY);
            stop_all_notes();
            PLAY_SONG(song_plover_gb);
          }
          return false;
          break;
        case MIDI:
          if (record->event.pressed) {
            layer_on(_MIDI);
            stop_all_notes();
            PLAY_SONG(song_preonic_sound);
          }
          return false;
          break;
//         case LOWER:
//           if (record->event.pressed) {
//             layer_on(_LOWER);
//             layer_state_set_user(_LOWER, _RAISE, _ADJUST);
//           } else {
//             layer_off(_LOWER);
//             update_tri_layer(_LOWER, _RAISE, _ADJUST);
//           }
//           return false;
//           break;
//         case RAISE:
//           if (record->event.pressed) {
//             layer_on(_RAISE);
//             update_tri_layer(_LOWER, _RAISE, _ADJUST);
//           } else {
//             layer_off(_RAISE);
//             update_tri_layer(_LOWER, _RAISE, _ADJUST);
//           }
//           return false;
//           break;
// macOS key codes from /u/Archite - https://gist.github.com/archite/6021f8204f147ff8b756f73a12bc778a
        case KC_MISSION_CONTROL:
          if (record->event.pressed) {
            host_consumer_send(0x29F);
          } else {
            host_consumer_send(0);
          }
          return false;
          break;
        case KC_SPOTLIGHT:
          if (record->event.pressed) {
            host_consumer_send(0x221);
          } else {
            host_consumer_send(0);
          }
          return false;
          break;
        case KC_DICTATION:
          if (record->event.pressed) {
            host_consumer_send(0xCF);
          } else {
            host_consumer_send(0);
          }
          return false;
          break;
        case KC_DO_NOT_DISTURB:
          if (record->event.pressed) {
            host_system_send(0x9B);
          } else {
            host_system_send(0);
          }
          return false;
          break;
        case KC_LOCK_SCREEN:
          if (record->event.pressed) {
            host_consumer_send(0x19E);
          } else {
            host_consumer_send(0);
          }
          return false;
          break;
        case KC_LAUNCHPAD:
          if (record->event.pressed) {
            host_consumer_send(0x2A0);
          } else {
            host_system_send(0);
          }
          return false;
          break;
//         case TEXT_DESELECT:
//           if (record->event.pressed) {
// //             tap_code(KC_RGHT);  
//             tap_code16(LOPT(KC_LEFT));          
//             tap_code16(LOPT(KC_RGHT));
//           }
//           return false;
//           break;
        case TEXT_SELECT_WORD:
          if (record->event.pressed) { // when keycode TEXT_SELECT_WORD is pressed
              register_code(KC_LOPT);
              tap_code(KC_LEFT);
              register_code(KC_LSFT);
              tap_code(KC_RGHT);
          } else { // when keycode TEXT_SELECT_WORD is released
              unregister_code(KC_LSFT);
              unregister_code(KC_LOPT);
          }
//             tap_code16(LOPT(KC_LEFT));          
//             tap_code16(LAG(KC_RGHT));
//           }
          return false;
          break;
        case LT(_SYM,KC_SPACE): // Space/Lower (layer tap)
        // this is kind of broken because it adds a space if its tapped, wheen really it should just disable the layer toggle. fix it by changing the line before "break;" to "return false;", and then change the press condition to include "&& record->event.pressed" to intercept taps, and add an "else if (record->event.pressed)" to incercept holds. 
            if (record->event.pressed) {
                is_lower_key_held = true;
//                 SEND_STRING("__________AA_SPACE_LOWER pressed__");tap_code16(KC_ENT);
            } else { 
                is_lower_key_held = false;
                layer_off(_SYM);
//                 SEND_STRING("__________AA_SPACE_LOWER_release_hold__");tap_code16(KC_ENT);
            }
            return true;             // Return true for normal processing of tap keycode
            break;
      }
    return true;
};

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SFT_T(KC_ESC):
            // Immediately select the hold action (Shift) when another key (ESC key?) is pressed.
            return true;
        default:
            // Do not select the hold action when another key is pressed. (Don't send Shift on every single keypress on the keyboard.)
            return false;
    }
}

bool muse_mode = false;
uint8_t last_muse_note = 0;
uint16_t muse_counter = 0;
uint8_t muse_offset = 70;
uint16_t muse_tempo = 50;

/* Encoder (Leftover from default keymap) */
// bool encoder_update_user(uint8_t index, bool clockwise) {
//   if (muse_mode) {
//     if (IS_LAYER_ON(_RAISE)) {
//       if (clockwise) {
//         muse_offset++;
//       } else {
//         muse_offset--;
//       }
//     } else {
//       if (clockwise) {
//         muse_tempo+=1;
//       } else {
//         muse_tempo-=1;
//       }
//     }
//   } else {
//     if (clockwise) {
//       register_code(KC_PGDN);
//       unregister_code(KC_PGDN);
//     } else {
//       register_code(KC_PGUP);
//       unregister_code(KC_PGUP);
//     }
//   }
//     return true;
// }

void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
#endif
}