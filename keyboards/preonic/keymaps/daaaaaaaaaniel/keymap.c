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
// bool is_lower_key_held = false;
// bool is_raise_key_held = false;
// bool is_layer_toggled_on = false;

/* declare layers */
enum preonic_layers {
  _QWERTY,
  _SPACE,
  _SYM,
  _EXT,
  _HOME,
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
  WINDOW_SWITCHER, // alt-tab
//   WINDOW_SWITCHER_REVERSE,
  AA_LSPC,
  AA_RSPC,
  AA_TAB
};
bool is_alt_tab_active = false;
uint16_t bespoke_tap_timer = 0;

#define AA_MOD4 RCMD_T(KC_BSPC) // command (hold); backspace (tap)
#define AA_MOD5 RCTL_T(KC_LEFT) // control (hold); left arrow (tap)
#define AA_MOD6 ROPT_T(KC_UP) // option (hold); up arrow (tap)
#define AA_RSFT RSFT_T(KC_UP) // right shift (hold); up arrow (tap) /* alt version: // OSM(MOD_RSFT) // one-shot right shift */
#define SX_GRV SFT_T(KC_GRV) // shift (hold); grave (tap)
#define DD_CMD LCMD_T(KC_DEL) // command (hold); delete (tap)
#define DD_BSPC ROPT_T(KC_BSPC) // option (hold); backspace (tap)
#define KX_CUT LCMD(KC_X) // command-x
#define KX_COPY LCMD(KC_C) // command-c
#define KX_PSTE LCMD(KC_V) // command-v
#define SHOW_ALL_APP_WINDOWS LCTL(KC_DOWN) // Control-Down Arrow
#define WN_FOCU G(KC_GRV)
#define NEXTTAB RCTL(KC_TAB) // SCMD(KC_RBRC) // Command-Shift-] (or perhaps Control-Tab)
#define PREVTAB RCS(KC_TAB) // SCMD(KC_LBRC) // Command-Shift-[ (or perhaps Control-Shift-Tab)
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
/* keycodes for moving between layers */
// #define AA_TAB  LT(_EXT, KC_TAB)
// #define AA_RSPC LSFT_T(KC_SPACE) // shift (hold); space (tap) // /* alt version:*/ LT(_SYM,KC_SPACE)
// #define AA_LSPC  LT(_SYM,KC_ENT)
#define WN_SWIT WINDOW_SWITCHER

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

// #define WN_REV WINDOW_SWITCHER_REVERSE // same effect as WN_FOCU (command-grave)
/* keycodes for Amethyst */
#define AM_MOD1 LM(_QWERTY, MOD_LALT | MOD_LSFT)
#define AM_MOD2 LM(_QWERTY, MOD_MEH) // Control + Option + Shift 
// #define AA_GRV  LSA_T(KC_GRV) // previously this was used for Amethyst on the base layer
// #define AA_Q    LCTL_T(KC_Q) // previously this was used for Amethyst on the base layer


/* Sounds */
#ifdef AUDIO_ENABLE
  float song_preonic_sound[][2] = SONG(PREONIC_SOUND); // song for switching into MIDI layer 
  float song_plover_gb[][2]  = SONG(PLOVER_GOODBYE_SOUND);  // song for switching out of MIDI layer (back to QWERTY)
#endif

/* Combos */
// When both SPACE keys are tapped together, execute ENTER. When both SPACE keys are HELD, activate _EXTRA layer.
enum combos {
  CMD_CAPSLOCKS,
  SPACE_LTEXTRA,
  QGRAVE_FOCUS,
  TABA_ESCAPE,
  TABA_ESCAPE_SYM,
  JK_LEFT,
  KL_RIGHT,
  JI_UP,
  MK_DOWN,
  BACKSPACECOMMA_DELETE,
  RIGHTPERIOD_LEFT,
  COMBO_LENGTH
};
uint16_t COMBO_LEN = COMBO_LENGTH;

/* press these keys together to trigger combo */
const uint16_t PROGMEM cmd_combo[] = {KC_LCMD, AA_MOD4, COMBO_END};
const uint16_t PROGMEM space_combo[] = {AA_LSPC, AA_RSPC, COMBO_END};
const uint16_t PROGMEM focus_combo[] = {KC_GRV, KC_Q, COMBO_END};
const uint16_t PROGMEM escape_combo[] = {AA_TAB, KC_A, COMBO_END};
const uint16_t PROGMEM escape_sym_combo[] = {WN_SWIT, KC_EXLM, COMBO_END};
const uint16_t PROGMEM left_combo[] = {KC_J, KC_K, COMBO_END};// {KC_AMPR, KC_ASTR, COMBO_END};
const uint16_t PROGMEM right_combo[] = {KC_K, KC_L, COMBO_END};// {KC_ASTR, KC_LPRN, COMBO_END};
const uint16_t PROGMEM up_combo[] = {KC_J, KC_I, COMBO_END};// {KC_AMPR, KC_8, COMBO_END};
const uint16_t PROGMEM down_combo[] = {KC_M, KC_K, COMBO_END};// {KC_RCBR, KC_ASTR, COMBO_END};
const uint16_t PROGMEM delete_combo[] = {KC_COMM, AA_MOD4, COMBO_END};
const uint16_t PROGMEM right2_combo[] = {KC_DOT, AA_MOD5, COMBO_END};

/* triggering combo has this effect */
combo_t key_combos[] = {
 [CMD_CAPSLOCKS] = COMBO(cmd_combo, KC_CAPS),
 [SPACE_LTEXTRA] = COMBO(space_combo, LT(_EXTRA, OSM(MOD_MEH))),
 [QGRAVE_FOCUS] = COMBO(focus_combo, G(KC_GRV)),
 [TABA_ESCAPE] = COMBO(escape_combo, KC_ESC),
 [TABA_ESCAPE_SYM] = COMBO(escape_sym_combo, KC_ESC),
 [JK_LEFT] = COMBO(left_combo, KC_LEFT),
 [KL_RIGHT] = COMBO(right_combo, KC_RGHT),
 [JI_UP] = COMBO(up_combo, KC_UP),
 [MK_DOWN] = COMBO(down_combo, KC_DOWN),
 [BACKSPACECOMMA_DELETE] = COMBO(delete_combo, KC_DEL),
 [RIGHTPERIOD_LEFT] = COMBO(right2_combo, KC_RGHT),

};
/* END Combos */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* 
NOTE
I'm changing the _RAISE and _LOWER layers. In the next `push`, I'm only having QWERTY and one other "special" layer for numbers/symbols. The Nav layer will be accessed through the pinkies, not the thumbs.
*/

// SEE PREVIOUS 1x2uC KEYMAP HERE - https://github.com/daaaaaaaaaniel/qmk_firmware/blob/3289f8d3af05a677b77b2cc89f0f98e43c83b2b3/keyboards/preonic/keymaps/daaaaaaaaaniel/keymap.c

 /* QWERTY (Default layer)
 * ,-----------------------------------------------------------------------------------.
 * |  Esc |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   `  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |`Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |  Up  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  | Opt  | Ctrl | Cmd  | Shift|   R-space   | Bksp | Left |  Up  | Down |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_QWERTY] = LAYOUT_preonic_1x2uC(
  KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  KC_GRV,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  AA_TAB,LT(_HOME,KC_A),KC_S,KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, LT(_EXT, KC_QUOT),
  SX_GRV,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, AA_RSFT,
  KC_CAPS, KC_LCTL, KC_LOPT, KC_LCMD, LT(_SYM,KC_ENT), AA_RSPC,  AA_MOD4, AA_MOD5, AA_MOD6, KC_DOWN, KC_RGHT
),

/* Space (nav controls, no changes to alphas) - holding Space
 * ,-----------------------------------------------------------------------------------.
 * |AppSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |  F12 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Focus|   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |WinSwi|   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  Esc |      |      |   ~  |   |  |  \   |   [  |   ]  |   _  |   {  |   }  |AppSwi|
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | MIDI |      |      |      |      |             |  Del | Right| Down |   —  |   –  |
 * `-----------------------------------------------------------------------------------'
 */
[_SPACE] = LAYOUT_preonic_1x2uC(
  ALL_APP,  KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, KC_F12,
  WN_FOCU, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  WN_SWIT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  KC_ESC,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, ALL_APP,
  MIDI,   TO(_SYM), _______, _______, _______, MO(_SPACE), RCMD_T(KC_DEL), RCTL_T(KC_RGHT), ROPT_T(KC_DOWN), S(A(KC_MINS)), A(KC_MINS)
),

/* Numbers and Symbols - holding Space
 * ,-----------------------------------------------------------------------------------.
 * |AppSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |  F12 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Focus|   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |WinSwi|   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  Esc |      |      |   ~  |   |  |  \   |   [  |   ]  |   _  |   {  |   }  | Down |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | MIDI |      |      |      |      |             |  Del | Right|  Up  |   —  |   –  |
 * `-----------------------------------------------------------------------------------'
 */
[_SYM] = LAYOUT_preonic_1x2uC(
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, KC_F12,
  WN_FOCU, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  WN_SWIT, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PLUS,
  KC_ESC,  KC_NO,   KX_CUT,  KC_TILD, KC_PIPE, KC_BSLS, KC_LBRC, KC_RBRC, KC_UNDS, KC_LCBR, KC_RCBR, RSFT_T(KC_DOWN),
  MIDI,   TO(_SYM), _______, _______, MO(_SYM), LT(_SPACE,KC_SPACE), RCMD_T(KC_DEL), RCTL_T(KC_RGHT), ROPT_T(KC_DOWN), S(A(KC_MINS)), A(KC_MINS)
),

/* Tab (Window Managment layer) - holding Tab
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |AppSwi|  Esc |WinSwR|WinSwi|  Esc |SwapCh|WrdSel|  Up  |PrvTab|NxtTab|      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      | Shift| Opt  | Cmd  |      |      |      | Left | Down |Right | Opt  |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      | Cut  | Copy | Paste|      | Home | PgDwn| PgUp | End  |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty|(Lock)|      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_EXT] = LAYOUT_preonic_1x2uC( // [Ergonomic Keyboard Mods: Extend Layer](http://colemakmods.github.io/ergonomic-mods/extend.html)
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, ALL_APP, KC_ESC,  WN_FOCU, WN_SWIT, KC_ESC,  KX_SWAP, TX_SEL,  KC_UP,   PREVTAB, NEXTTAB, _______,
  MO(_EXT),MO(_HOME),KC_LSFT,KC_LOPT, KC_LCMD, KC_NO,   KC_NO,   KC_LEFT, KC_DOWN, KC_RGHT, KC_ROPT, MO(_EXT),
  _______, KC_NO,   KX_CUT,  KX_COPY, KX_PSTE, KC_NO,   KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_NO,   _______,
  TO(_QWERTY),TO(_EXT), _______, DD_CMD, _______,   _______,     _______, _______, _______, _______, _______
),

/* Home row mods - holding A
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |AppSwi|  Esc |WinSwR|WinSwi|  Esc |SwapCh|WrdSel|  Up  |PrvTab|NxtTab|      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      | Shift| Opt  | Cmd  |      |      |      | Left | Down |Right | Opt  |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      | Cut  | Copy | Paste|      | Home | PgDwn| PgUp | End  |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty|(Lock)|      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_HOME] = LAYOUT_preonic_1x2uC( // [Ergonomic Keyboard Mods: Extend Layer](http://colemakmods.github.io/ergonomic-mods/extend.html)
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  MO(_EXT),MO(_HOME),KC_LSFT,KC_LOPT, KC_LCMD, KC_NO,   _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  TO(_QWERTY),TO(_EXT), _______, DD_CMD, _______,   _______,     RCMD_T(KC_DEL), RCTL_T(KC_RGHT), ROPT_T(KC_DOWN), _______, _______
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
 * |      |(Lock)|      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_EXTRA] = LAYOUT_preonic_1x2uC(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, _______,
  _______, KC_NO,   KX_CUT,  KX_COPY, KX_PSTE, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_MNXT, KC_VOLD, KC_VOLU,
  TO(_QWERTY),TO(_EXTRA), _______, _______, _______,      _______,     _______, _______, _______, _______, _______
),

// /* CMD1 (Empty - this is just for accessing a tri-layer when CMD1+CMD2 are active simultaneously)
//  */
// [_CMD1] = LAYOUT_preonic_1x2uC(
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______,      _______,     _______, _______, _______, _______, _______
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
 * |Qwerty| Vel-1| Vel+1| Bend+| Bend-|             |      |Oct-1 |Oct+1 |Note-1|Note+1|
 * `-----------------------------------------------------------------------------------'
 */
[_MIDI] = LAYOUT_preonic_1x2uC(
  MI_C,    MI_Db,   MI_D,    MI_Eb,   MI_E,    MI_F,    MI_Gb,   MI_G,    MI_Ab,   MI_A,    MI_Bb,   MI_B,
  MI_C_1,  MI_Db_1, MI_D_1,  MI_Eb_1, MI_E_1,  MI_F_1,  MI_Gb_1, MI_G_1,  MI_Ab_1, MI_A_1,  MI_Bb_1, MI_B_1,
  MI_C_2,  MI_Db_2, MI_D_2,  MI_Eb_2, MI_E_2,  MI_F_2,  MI_Gb_2, MI_G_2,  MI_Ab_2, MI_A_2,  MI_Bb_2, MI_B_2,
  MI_C_3,  MI_Db_3, MI_D_3,  MI_Eb_3, MI_E_3,  MI_F_3,  MI_Gb_3, MI_G_3,  MI_Ab_3, MI_A_3,  MI_Bb_3, MI_B_3,
  QWERTY, MI_VELD, MI_VELU, MI_BENDD, MI_BENDU,    _______,     _______, MI_OCTD, MI_OCTU, MI_TRNSD, MI_TRNSU
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
//         case LT(_SYM,KC_SPACE): // Space/Lower (layer tap)
//         // this is kind of broken because it adds a space if its tapped, wheen really it should just disable the layer toggle. fix it by changing the line before "break;" to "return false;", and then change the press condition to include "&& record->event.pressed" to intercept taps, and add an "else if (record->event.pressed)" to incercept holds. 
//             if (record->event.pressed) {
//                 is_lower_key_held = true;
// //                 SEND_STRING("__________AA_SPACE_LOWER pressed__");tap_code16(KC_ENT);
//             } else { 
//                 is_lower_key_held = false;
//                 layer_off(_SYM);
// //                 SEND_STRING("__________AA_SPACE_LOWER_release_hold__");tap_code16(KC_ENT);
//             }
//             return true;             // Return true for normal processing of tap keycode
//             break;
        case AA_LSPC: // modifed from https://github.com/davidagross/qmk_firmware/blob/1ccdb0dd461023398076eb3ea92ff050c5aba6ef/keyboards/minidox/keymaps/dgroseph/keymap.c (NAVHOM key) which is based on http://blog.hgao.net/post/qmk-mod-key/
          if (record->event.pressed) {
            // Records press timer
            bespoke_tap_timer = timer_read();
            // turn on the SYM layer
            layer_on(_SYM);
          } else if (is_alt_tab_active) {
            // turn off the SYM layer
            layer_off(_SYM);
            // deactivate alt-tab
            is_alt_tab_active = false;
            unregister_code(KC_LCMD);
          } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) {
            // turn off the SYM layer
            layer_off(_SYM);
            // Sends out 'enter' if the key is held for less than tapping term 
            tap_code(KC_ENT);
          } else {
            // turn off the SYM layer
            layer_off(_SYM);
          } 
          return false;
          break;
        case AA_RSPC: // modifed from https://github.com/davidagross/qmk_firmware/blob/1ccdb0dd461023398076eb3ea92ff050c5aba6ef/keyboards/minidox/keymaps/dgroseph/keymap.c (NAVHOM key) which is based on http://blog.hgao.net/post/qmk-mod-key/
          if (record->event.pressed) {
            // Records press timer
            bespoke_tap_timer = timer_read();
            // turn on the SPACE layer
            layer_on(_SPACE);
          } else if (is_alt_tab_active) {
            // turn off the SPACE layer
            layer_off(_SPACE);
            // deactivate alt-tab
            is_alt_tab_active = false;
            unregister_code(KC_LCMD);
          } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) {
            // turn off the SPACE layer
            layer_off(_SPACE);
            // Sends out 'space' if the key is held for less than tapping term 
            tap_code(KC_SPACE);
          } else {
            // turn off the SPACE layer
            layer_off(_SPACE);
          } 
          return false;
          break;
        case AA_TAB: // modifed from https://github.com/davidagross/qmk_firmware/blob/1ccdb0dd461023398076eb3ea92ff050c5aba6ef/keyboards/minidox/keymaps/dgroseph/keymap.c (NAVHOM key) which is based on http://blog.hgao.net/post/qmk-mod-key/
          if (record->event.pressed) {
            // Records press timer
            bespoke_tap_timer = timer_read();
            // turn on the _EXT layer
            layer_on(_EXT);
          } else if (is_alt_tab_active) {
            // turn off the _EXT layer
            layer_off(_EXT);
            // deactivate alt-tab
            is_alt_tab_active = false;
            unregister_code(KC_LCMD);
          } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) {
            // turn off the _EXT layer
            layer_off(_EXT);
            // Sends out 'home' if the key is held for less than tapping term 
            tap_code(KC_TAB);
          } else {
            // turn off the _EXT layer
            layer_off(_EXT);
          } 
          return false;
          break;
        case WINDOW_SWITCHER:
          if (record->event.pressed) {
            if (!is_alt_tab_active) {
              is_alt_tab_active = true;
              register_code(KC_LCMD);
            }
            tap_code(KC_TAB);
          }
          return false;
          break;
//         case WINDOW_SWITCHER_REVERSE:
//           if (record->event.pressed) {
//             if (!is_alt_tab_active) {
//               is_alt_tab_active = true;
//               register_code(KC_LCMD);
//             }
//             register_code(KC_LSFT);
//             tap_code(KC_TAB);
//             unregister_code(KC_LSFT);
//           }
//           return false;
//           break;
      }
    return true;
};

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SFT_T(KC_GRV):
            // Immediately select the hold action (Shift) when another key (grave key?) is pressed.
            return true;
        default:
            // Do not select the hold action when another key is pressed. (Don't send Shift on every single keypress on the keyboard.)
            return false;
    }
}

// // [QMK triggers: reacting to interesting events](https://getreuer.info/posts/keyboards/triggers/index.html#action-on-double-tap-without-delay)
// if (record->event.pressed) {
//   static bool tapped = false;
//   static uint16_t tap_timer = 0;
//   if (keycode == KC_LOPT) {
//     if (tapped && !timer_expired(record->event.time, tap_timer)) {
//       // The key was double tapped.
//       //clear_mods();  // If needed, clear the mods.
//       // Do something interesting...
//       register_code(KC_LCMD);
//     }
//     tapped = true;
//     tap_timer = record->event.time + TAPPING_TERM;
//   } else {
//     // On an event with any other key, reset the double tap state.
//     tapped = false;
//   }
// }

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