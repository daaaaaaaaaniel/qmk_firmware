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
  _L_SPACE,
  _EXT,
  _NUM,
  _SYM,
  _NAV,
  _SPEC,
  _EXTRA,
//   _CMD1,
  _MIDI4,
  _MIDI8
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
  APP_SWITCHER, // alt-tab
//   APP_SWITCHER_REVERSE,
  AA_LSPC,
  AA_RSPC,
  AA_TAB,
  AA_SYM,
//   AA_MOD3,
  SHIFTED_SLASHGRAVE_OVERRIDE
};
bool is_alt_tab_active = false;
uint16_t bespoke_tap_timer = 0;
static uint8_t spacebar_layer_tracker;
static uint8_t leftspacebar_layer_tracker;

/* alias for keys in high-use positions */
/* right hand */
#define AA_RTOP KC_MINS // top key position in rightmost column (below function row)  
#define AA_QUOT LT(_SYM, KC_QUOT) // quote key position
#define AA_RSFT LSFT_T(KC_GRV) // right shift key position
#define AA_MOD3 CMD_T(KC_ENT) // command (hold); return (tap)
#define AA_MOD4 LT(_EXT, KC_BSPC) // _EXT (hold); backspace (tap)
#define AA_MOD5 LT(_NUM, KC_TAB) // _NUM (hold); Tab (tap)
#define AA_MOD6 LT(_NUM, KC_ENT) // _NUM (hold); Enter (tap)

#define OSM_CTL OSM(MOD_LCTL)
#define OSM_ALT OSM(MOD_LALT)
#define OSM_OPT OSM(MOD_LALT)

#define HRSFT_A SFT_T(KC_A) // Home row mod A/Shift
#define HRCTL_S CTL_T(KC_S) // Home row mod S/Ctrl
#define HROPT_D OPT_T(KC_D) // Home row mod D/Opt
#define HRCMD_F CMD_T(KC_F) // Home row mod F/Cmd

// #define O_RSFT OSM(MOD_RSFT) // one-shot right shift
#define SX_GRV SFT_T(KC_GRV) // shift (hold); grave (tap)
// #define DD_BSPC ROPT_T(KC_BSPC) // option (hold); backspace (tap)
#define KX_CUT LCMD(KC_X) // command-x
#define KX_COPY LCMD(KC_C) // command-c
#define KX_PSTE LCMD(KC_V) // command-v
// #define KX_PALT LCMD(LSFT(KC_X))
#define NEXTTAB RCTL(KC_TAB) // RControl-Tab (for some reason there's no left-equivalent keycode) // /* alt version: */ SCMD(KC_RBRC) // Command-Shift-] - or RSG(KC_RBRC)
#define PREVTAB RCS(KC_TAB) // RControl-Shift-Tab (for some reason there's no left-equivalent keycode) // /* alt version: */ SCMD(KC_LBRC) // Command-Shift-[ - or RSG(KC_LBRC)
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
#define K_NAV LT(_NAV, KC_K)
#define WN_FOCU G(KC_GRV) // window focus - Cmd + `
#define SHOW_ALL_APP_WINDOWS LCTL(KC_DOWN) // Control-Down Arrow
#define CHARACTER_VIEWER G(C(KC_SPC)) // emoji/character viewer

/* symbols */
#define KC_EURO S(A(KC_2)) // €
#define KC_BRITISH_POUND A(KC_3) // £
// #define KC_YEN A(KC_Y) // ¥
#define KC_NOT_SIGN A(KC_L) // ¬
#define KC_DIVISION A(KC_SLSH) // ÷
// #define KC_DAGGER A(KC_T) // †
#define KC_SECTION A(KC_6) // §
#define KC_PILCROW A(KC_7) // ¶
#define EN_DASH A(KC_MINS) // –
#define EM_DASH S(A(KC_MINS)) // —
#define KC_DEGREE S(A(KC_8)) // °

/* short names / aliases */
#define CH_EMOJ CHARACTER_VIEWER
#define KC_BRIT KC_BRITISH_POUND
#define KC_SECT KC_SECTION
#define KC_DEGR KC_DEGREE
#define KC_MCTL KC_MISSION_CONTROL
#define KC_SPLT KC_SPOTLIGHT
#define KC_DICT KC_DICTATION
#define KC_DOND KC_DO_NOT_DISTURB
#define KC_LCKS KC_LOCK_SCREEN
#define KC_LPAD KC_LAUNCHPAD
#define ALL_APP SHOW_ALL_APP_WINDOWS // window switcher
#define APP_SWI APP_SWITCHER // alt-tab
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
#define TD_SESC TD(TD_SHIFT_ESC)
// #define TD_SPC  TD(TD_SPACEBAR)
#define TD_LSPC LT(_L_SPACE, KC_SPC)//TD(TD_LEFT_SPACEBAR)
#define TD_RSPC TD(TD_SPACEBAR)

// #define WN_REV APP_SWITCHER_REVERSE // same effect as WN_FOCU (command-grave)
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
//   SPACE_LTEXTRA, // KC_LSPC + KC_RSPC = space_combo - _EXTRA (hold); OSM(MOD_MEH) (tap)
  QGRAVE_FOCUS, // Q+adjacent key on far right 
  TABA_ESCAPE,
  JK_LEFT,
  KL_RIGHT,
  JI_UP,
  MK_DOWN,
  BACKSPACECOMMA_DELETE,
  RIGHTPERIOD_LEFT,
  SCLNQUOT_ENTER, // enter_combo; semicolon + quote 
  PRTOP_BACKSPACE, // backspace_combo; p + minus
  COMBO_LENGTH
};
uint16_t COMBO_LEN = COMBO_LENGTH;

/* press these keys together to trigger combo */
const uint16_t PROGMEM cmd_combo[] = {AA_MOD3, AA_MOD4, COMBO_END};
// const uint16_t PROGMEM space_combo[] = {TD_LSPC, TD_RSPC, COMBO_END}; // 
const uint16_t PROGMEM focus_combo[] = {KC_GRV, KC_Q, COMBO_END}; // possibly switch to QK_GESC ? 
const uint16_t PROGMEM escape_combo[] = {AA_TAB, KC_A, COMBO_END};
const uint16_t PROGMEM left_combo[] = {KC_J, K_NAV, COMBO_END};// {KC_AMPR, KC_ASTR, COMBO_END};
const uint16_t PROGMEM right_combo[] = {K_NAV, KC_L, COMBO_END};// {KC_ASTR, KC_LPRN, COMBO_END};
const uint16_t PROGMEM up_combo[] = {KC_J, KC_I, COMBO_END};// {KC_AMPR, KC_8, COMBO_END};
const uint16_t PROGMEM down_combo[] = {KC_M, K_NAV, COMBO_END};// {KC_RCBR, KC_ASTR, COMBO_END};
const uint16_t PROGMEM delete_combo[] = {KC_COMM, AA_MOD4, COMBO_END};
const uint16_t PROGMEM right2_combo[] = {KC_DOT, AA_MOD5, COMBO_END};
const uint16_t PROGMEM enter_combo[] = {KC_SCLN, AA_QUOT, COMBO_END};
const uint16_t PROGMEM backspace_combo[] = {KC_P, AA_RTOP, COMBO_END};

/* triggering combo has this effect */
combo_t key_combos[] = {
 [CMD_CAPSLOCKS] = COMBO(cmd_combo, KC_CAPS),
//  [SPACE_LTEXTRA] = COMBO(space_combo, LT(_EXTRA, OSM(MOD_MEH))),
 [QGRAVE_FOCUS] = COMBO(focus_combo, G(KC_GRV)),
 [TABA_ESCAPE] = COMBO(escape_combo, KC_ESC),
 [JK_LEFT] = COMBO(left_combo, KC_LEFT),
 [KL_RIGHT] = COMBO(right_combo, KC_RGHT),
 [JI_UP] = COMBO(up_combo, KC_UP),
 [MK_DOWN] = COMBO(down_combo, KC_DOWN),
 [BACKSPACECOMMA_DELETE] = COMBO(delete_combo, KC_DEL),
 [RIGHTPERIOD_LEFT] = COMBO(right2_combo, KC_RGHT),
 [SCLNQUOT_ENTER] = COMBO(enter_combo, KC_ENT),
 [PRTOP_BACKSPACE] = COMBO(backspace_combo, KC_BSPC),
};
/* END Combos */

// just throwing this in kind of randomly… 
// one shot mods // https://docs.qmk.fm/#/one_shot_keys?id=one-shot-keys
/* 
void oneshot_mods_changed_user(uint8_t mods) {
    if (!{HOLDING DOWN THE CUSTOM KEY}) { 
        oneshot_disable();
    }
    else {
        oneshot_enable(); 
    }
}
*/


/* KEY OVERRIDES */
// Shift + , = !
const key_override_t shift_comma_exclam_override = ko_make_basic(MOD_MASK_SHIFT, KC_COMMA, S(KC_1)); //ko_make_with_layers(MOD_MASK_SHIFT, KC_COMMA, S(KC_1), 1<<_QWERTY);
// Shift + . = ?
const key_override_t shift_period_question_override = ko_make_basic(MOD_MASK_SHIFT, KC_DOT, S(KC_SLASH));//ko_make_with_layers(MOD_MASK_SHIFT, KC_DOT, S(KC_SLASH), 1<<_QWERTY);
// Shift + / = `
const key_override_t shift_slash_hash_override = ko_make_basic(MOD_MASK_SHIFT, KC_SLASH, S(KC_3));                                           
// This globally defines all key overrides to be used
const key_override_t **key_overrides = (const key_override_t *[]){
    &shift_comma_exclam_override,
    &shift_period_question_override,
    &shift_slash_hash_override,
    NULL // Null terminate the array of overrides!
};


#define TAPDANCE_TOP
#include "tapdance.inc"
#undef TAPDANCE_TOP


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// SEE PREVIOUS 1x2uC KEYMAP HERE - https://github.com/daaaaaaaaaniel/qmk_firmware/blob/3289f8d3af05a677b77b2cc89f0f98e43c83b2b3/keyboards/preonic/keymaps/daaaaaaaaaniel/keymap.c

 /* QWERTY (Default layer)
 * ,-----------------------------------------------------------------------------------.
 * |  Esc |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   `  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |§ ⇥Tab|   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |§ '   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |⇧ `   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  |(Lock)|⌃ Ctrl|⌥ Opt |⌘Cmd ⏎|␣ Spc |␣ Spc |⎈ Bksp|#  ⇥  |#  ⏎  |  Esc |⇧⌘  ⏎ |
 * `-----------------------------------------------------------------------------------'
 *
 *  ⇧Shift + QWERTY
 * ,-----------------------------------------------------------------------------------.
 * |  Esc |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   &  |   (  |   )  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   ~  |      |      |      |      |      |      |      |      |      |      |  _   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |§ ⇥Tab|      |      |      |      |      |      |      |      |      |   ;  |§ "   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧Shift|      |      |      |      |      |      |      |   !  |   ?  |   #  |⇧ ~   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  |(Lock)|⌃ Ctrl|⌥ Opt |⌘Cmd ⏎|␣ Spc |␣ Spc |⎈ Bksp|#  ⇥  |#  ⏎  |  Esc |⇧⌘  ⏎ |
 * `-----------------------------------------------------------------------------------'
 */
[_QWERTY] = LAYOUT_preonic_grid(
  KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  KC_GRV,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    AA_RTOP,
  AA_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    K_NAV,   KC_L,    KC_SCLN, AA_QUOT,
  TD_SESC, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, AA_RSFT,
  KC_CAPS, KC_LOCK, OSM_CTL, OSM_OPT, AA_MOD3, TD_LSPC, TD_RSPC, AA_MOD4, AA_MOD5, AA_MOD6, KC_ESC,  SCMD_T(KC_ENT)
),

 /* RIGHT SPACE ␣ (nav controls) - holding Space 
 * ,-----------------------------------------------------------------------------------.
 * |WinSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |  F12 |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   ~  |      |      |      |      |      | PgUp | Home |   ↑  | End  |   _  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |AppSwi|⇧  A  |⌃  S  |⌥  D  |⌘  F  |      |⇧PgDwn|   ←  |   ↓  |   →  |⇧  *  |  ^   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧ Esc |⌥  Z  |⇧  X  |⌥  C  |⌘  V  |      |⌥  N  |⌘  M  |   [  |   ]  |⌥  \  |⇧ ⏎   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | MIDI |      |      |      |      |=|||||||||||=|⌘ Del |⌥ Esc |⌃  ⇥  |  Esc |⇧⌘  ⏎ |
 * `-----------------------------------------------------------------------------------'
 *
 *  ⇧Shift + SPACE ␣ 
 * ,-----------------------------------------------------------------------------------.
 * |WinSwi|      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   ~  |      |      |      |      |      |      |      |      |      |   _  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |AppSwi|      |      |      |      |      |      |      |      |      |⇧  ÷  |  °   | // Shift + `*` doesn't actually send ÷ 
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧ Esc |      |      |      |      |      |      |      |   {  |   }  |⌥  |  |⇧  ⏎  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | MIDI |      |      |      |      |=|||||||||||=|⌘ Del |⌥ Esc |⌃  ⇥  |  Esc |⇧⌘  ⏎ |
 * `-----------------------------------------------------------------------------------'
 */
[_SPACE] = LAYOUT_preonic_grid(
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, KC_F12,
  KC_TILD, KC_PGUP, _______, KC_UP,   _______, _______, KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_UNDS, KC_PLUS,
  APP_SWI, HRSFT_A, HRCTL_S, HROPT_D, HRCMD_F, _______, RSFT_T(KC_PGDN), KC_LEFT, KC_DOWN, KC_RGHT, RSFT_T(KC_ASTR), KC_CIRC,
  SFT_T(KC_ESC),LOPT_T(KC_Z), SFT_T(KC_X), OPT_T(KC_C), CMD_T(KC_V), _______, ROPT_T(KC_N), RCMD_T(KC_M), KC_LBRC, KC_RBRC, ROPT_T(KC_BSLS), RSFT_T(KC_ENT),
  MIDI,    _______, _______, _______, _______, LT(_L_SPACE,KC_SPC), LT(_SPACE,KC_SPC), RCMD_T(KC_DEL), ROPT_T(KC_ESC), RCTL_T(KC_TAB), KC_ESC, _______
),

 /* LEFT SPACE ␣ 
 * ,-----------------------------------------------------------------------------------.
 * |WinSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |  F12 |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   ~  |      | Home |   ↑  | End  | PgUp | PgUp | Home |   ↑  | End  |   _  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |AppSwi|      |   ←  |   ↓  |   →  | PgDwn|⇧PgDwn|   ←  |   ↓  |   →  |⇧  *  |  ^   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧ Esc |      |      |      |      |      |⌥  N  |⌘  M  |   [  |   ]  |⌥  \  |⇧ ⏎   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | MIDI |      |      |      |      |=|||||||||||=|⌘ Del |⌥ Esc |⌃  ⇥  |  Esc |⇧⌘  ⏎ |
 * `-----------------------------------------------------------------------------------'
 */
[_L_SPACE] = LAYOUT_preonic_grid(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, KC_HOME, KC_UP,   KC_END,  KC_PGUP, _______, _______, _______, _______, _______, _______,
  _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  MIDI,    _______, _______, _______, _______, LT(_L_SPACE,KC_SPC), LT(_SPACE,KC_SPC), RCMD_T(KC_DEL), ROPT_T(KC_ESC), RCTL_T(KC_TAB), KC_ESC, _______
),

 /* ⎈EXTENSION   
 * replace these with the un-shifted keys, then access this layer only with `LM(layer, mod)` 
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |WinSwi|  Esc |   €  |AppSwR|AppSwi|SwapCh|   §  |   %  |   =  |   -  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |§     |⇧Shift|      |      |      |      |WrdSel|   ~  |   &  |   $  |   *  |  ^   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧Shift|      | Cut  | Copy | Paste| Enter|   —  |   –  |   (  |   )  |   @  |⇧  ⏎  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty|      |      |      |      |      |      |=||||=|⌥ Del |⇧ Esc |⌥⇧    |  ⏎   |
 * `-----------------------------------------------------------------------------------'
 *
 *  ⇧Shift + ⎈EXTENSION
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |   ¶  |   %  |   +  |   _  |   +  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |§     |      |      |      |      |      |      |   ~  |   ¬  |   £  |   ÷  |   °  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧Shift|      |      |      |      |      |   –  |   –  |   <  |   >  |   #  |⇧  ⏎  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty|      |      |      |      |      |      |=||||=|⌥ Del |⌃ Esc |⌥⇧    |   ⏎  |
 * `-----------------------------------------------------------------------------------'
 */
[_EXT] = LAYOUT_preonic_grid( // [Ergonomic Keyboard Mods: Extend Layer](http://colemakmods.github.io/ergonomic-mods/extend.html)
  _______, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, KC_F12,
  KC_TILD, ALL_APP, KC_ESC,  KC_EURO, WN_FOCU, APP_SWI, KX_SWAP, KC_SECT, KC_PERC, KC_EQL, KC_MINS , KC_PLUS,
  LT(_SYM, KC_GRV) // intercept this key and use it for changing tabs, or for chaning windows in the current application
         , KC_LSFT, _______, _______, _______, _______, TX_SEL,  KC_TILD, KC_AMPR, KC_DLR,  KC_ASTR, KC_CIRC,
  _______, KC_NO,   KX_CUT,  KX_COPY, KX_PSTE, KC_ENT,  EN_DASH, EM_DASH, KC_LPRN, KC_RPRN, KC_AT,   SFT_T(KC_ENT),
  TO(_QWERTY),_______,_______,_______,_______, _______, _______, MO(_EXT), ROPT_T(KC_DEL), RCTL_T(KC_ESC), S(KC_ROPT), KC_ENT
),

 /* Numeric Keypad # - holding MOD5/MOD6
 * ,-----------------------------------------------------------------------------------.
 * |WinSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst|NumLck|Clear⌧|   =  |   -  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Focus|      |      |      |      |      |      |   ,  |   /  |   7  |   8  |  9   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |AppSwi| CmdA |      |      |      |      |      |   .  |⌘  *  |   4  |   5  |§  6  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |⇧ Esc |      |      |      |      |      |      |Clear⌧|⌘Enter|   1  |   2  |⇧  3  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  |      |      |      |      |      |      |⌘ Del |=||||=|⇧  0  |   0  |⇧⌘  . |
 * `-----------------------------------------------------------------------------------'
 */
[_NUM] = LAYOUT_preonic_grid(
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_NUM,  KC_CLR,  KC_PEQL, KC_PMNS, KC_PPLS,
  WN_FOCU, _______, _______, _______, _______, _______, _______, KC_PCMM, KC_PSLS, KC_7,    KC_8,    KC_9,
  APP_SWI, G(KC_A), _______, _______, _______, _______, _______, KC_PDOT, RCMD_T(KC_PAST), KC_4, KC_5, LT(_SYM, KC_6),
  SFT_T(KC_ESC),_______,_______,_______,_______,_______,_______, KC_PCMM, RCMD_T(KC_PENT), KC_1, KC_2, RSFT_T(KC_3),
  KC_CAPS, _______, _______, _______, _______, _______, _______, LCMD_T(KC_DEL), MO(_NUM), RSFT_T(KC_0), RCMD_T(KC_0), SCMD_T(KC_PDOT)
),

 /* Numbers and Symbols § - holding _SYM
 * ,-----------------------------------------------------------------------------------.
 * |WinSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute | F12  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   ~  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |§  `  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  |§ +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Emoji|   §  |   €  |   £  |   |  |  \   |   {  |   }  |   _  |   [  |   ]  |⎈     |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  |      |      |      |      |      |      |⌘ Del |⌥ Up  |⌃ Down|   —  |  –   |
 * `-----------------------------------------------------------------------------------'
 */
[_SYM] = LAYOUT_preonic_grid(
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, KC_F12,
  KC_TILD, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  LT(_SYM,KC_GRV),  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, LT(_SYM,KC_EQL),
  CH_EMOJ, KC_SECT, KC_EURO, KC_BRIT, KC_PIPE, KC_BSLS, KC_LCBR, KC_RCBR, KC_UNDS, KC_LBRC, KC_RBRC, _______,
  KC_CAPS, _______, _______, _______, _______, LT(_SPACE,KC_SPACE), LT(_SPACE,KC_SPACE), RCMD_T(KC_DEL), ROPT_T(KC_UP), RCTL_T(KC_DOWN), EM_DASH, EN_DASH
),

 /* Navigation - holding K
 * (keep it minimal so it doesn't interfere with regular typing)
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |  Up  |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      | Left |=||||=| Right|      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      | Down |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_NAV] = LAYOUT_preonic_grid(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, KC_UP,   _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, KC_LEFT, MO(_NAV),KC_RGHT, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, KC_DOWN, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),

 /* Special (Macros) - holding Space + pinkie modifer (Shift/_EXT/_SYM)
 * ,-----------------------------------------------------------------------------------.
 * |      |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |=||||=|   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |=||||=|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |=||||=|   Z  |   X  |   C  |   V  |   B  |   N  |Paletr|   ,  |   .  |   /  |=||||=|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |=|||||||||||=|      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_SPEC] = LAYOUT_preonic_grid(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______,KC_NO/* Q */,KC_NO/* W */,KC_NO/* E */,KC_NO/* R */,KC_NO/* T */,KC_NO/* Y */,KC_NO/* U */,KC_NO/* I */,KC_NO/* O */,KC_NO/* P */,_______,
  _______,KC_NO/* A */,KC_NO/* S */,KC_NO/* D */,KC_NO/* F */,KC_NO/* G */,KC_NO/* H */,KC_NO/* J */,KC_NO/* K */,KC_NO/* L */,KC_NO/* ; */,_______,
  _______,KC_NO/* Z */,KC_NO/* X */,KC_NO/* C */,KC_NO/* V */,KC_NO/* B */,KC_NO/* N */,KC_NO/* M */,KC_NO/* N */,KC_NO/* . */,KC_NO/* / */,_______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),


 /* Extra (Media Functions layer) - holding Left Space + Right Space
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |WinSwi|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      | Next | Vol- | Vol+ | Mute |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |=|||||=|||||=|      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_EXTRA] = LAYOUT_preonic_grid(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, _______,
  _______, KC_NO,   KX_CUT,  KX_COPY, KX_PSTE, KC_NO,   KC_NO,   KC_MNXT, KC_VOLD, KC_VOLU, KC_MUTE, _______,
  TO(_QWERTY), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),


// /* CMD1 (Empty - this is just for accessing a tri-layer when CMD1+CMD2 are active simultaneously)
//  */
// [_CMD1] = LAYOUT_preonic_grid(
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
// ),


 /* MIDI - Fourths String Layout
 * ,-----------------------------------------------------------------------------------.
 * |  C3  |  D♭3 |  D3  |  E♭3 |  E3  |  F3  |  G♭3 |  G3  |  A♭3 |  A3  |  B♭3 |  B3  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  G2  |  A♭2 |  A2  |  B♭2 |  B2  |  C3  |  D♭3 |  D3  |  E♭3 |  E3  |  F3  |  G♭3 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |  D2  |  E♭2 |  E2  |  F2  |  G♭2 |  G2  |  A♭2 |  A2  |  B♭2 |  B2  |  C3  |  D♭3 |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  A1  |  B♭1 |  B1  |  C2  |  D♭2 |  D2  |  E♭2 |  E2  |  F2  |  G♭2 |  G2  |  A♭2 |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty| Vel-1| Vel+1| Bend-| Bend+|      |      |Note-1|Note+1|Oct-1 |Oct+1 |_MIDI8|
 * `-----------------------------------------------------------------------------------'
 */
[_MIDI4] = LAYOUT_preonic_grid(
  MI_C_3,  MI_Db_3, MI_D_3,  MI_Eb_3, MI_E_3,  MI_F_3,  MI_Gb_3, MI_G_3,  MI_Ab_3, MI_A_3,  MI_Bb_3, MI_B_3,
  MI_G_2,  MI_Ab_2, MI_A_2,  MI_Bb_2, MI_B_2,  MI_C_3,  MI_Db_3, MI_D_3,  MI_Eb_3, MI_E_3,  MI_F_3,  MI_Gb_3,
  MI_D_2,  MI_Eb_2, MI_E_2,  MI_F_2,  MI_Gb_2, MI_G_2,  MI_Ab_2, MI_A_2,  MI_Bb_2, MI_B_2,  MI_C_3,  MI_Db_3,
  MI_A_1,  MI_Bb_1, MI_B_1,  MI_C_2,  MI_Db_2, MI_D_2,  MI_Eb_2, MI_E_2,  MI_F_2,  MI_Gb_2, MI_G_2,  MI_Ab_2,
  QWERTY,  MI_VELD, MI_VELU, MI_BENDD,MI_BENDU, _______,_______, MI_TRNSD,MI_TRNSU,MI_OCTD, MI_OCTU, TO(_MIDI8)
),

 /* MIDI - Octaves
 * ,-----------------------------------------------------------------------------------.
 * |  C0  |  D♭0 |  D0  |  E♭0 |  E0  |  F0  |  G♭0 |  G0  |  A♭0 |  A0  |  B♭0 |  B0  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  C1  |  D♭1 |  D1  |  E♭1 |  E1  |  F1  |  G♭1 |  G1  |  A♭1 |  A1  |  B♭1 |  B1  |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |  C2  |  D♭2 |  D2  |  E♭2 |  E2  |  F2  |  G♭2 |  G2  |  A♭2 |  A2  |  B♭2 |  B2  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  C3  |  D♭3 |  D3  |  E♭3 |  E3  |  F3  |  G♭3 |  G3  |  A♭3 |  A3  |  B♭3 |  B3  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Qwerty| Vel-1| Vel+1| Bend-| Bend+|      |      |Note-1|Note+1|Oct-1 |Oct+1 |_MIDI4|
 * `-----------------------------------------------------------------------------------'
 */
[_MIDI8] = LAYOUT_preonic_grid(
  MI_C,    MI_Db,   MI_D,    MI_Eb,   MI_E,    MI_F,    MI_Gb,   MI_G,    MI_Ab,   MI_A,    MI_Bb,   MI_B,
  MI_C_1,  MI_Db_1, MI_D_1,  MI_Eb_1, MI_E_1,  MI_F_1,  MI_Gb_1, MI_G_1,  MI_Ab_1, MI_A_1,  MI_Bb_1, MI_B_1,
  MI_C_2,  MI_Db_2, MI_D_2,  MI_Eb_2, MI_E_2,  MI_F_2,  MI_Gb_2, MI_G_2,  MI_Ab_2, MI_A_2,  MI_Bb_2, MI_B_2,
  MI_C_3,  MI_Db_3, MI_D_3,  MI_Eb_3, MI_E_3,  MI_F_3,  MI_Gb_3, MI_G_3,  MI_Ab_3, MI_A_3,  MI_Bb_3, MI_B_3,
  QWERTY,  MI_VELD, MI_VELU, MI_BENDD,MI_BENDU, _______,_______, MI_TRNSD,MI_TRNSU,MI_OCTD, MI_OCTU, TO(_MIDI4)
)

};

// // i think this is what made it possible to distingish between the combo and the staggered pressing of both space keys?
// layer_state_t layer_state_set_user(layer_state_t state) {
//   return update_tri_layer_state(state, _LOWER, _RAISE, _EXTRA);
// }

layer_state_t layer_state_set_user(layer_state_t state) {
  state = update_tri_layer_state(state, _L_SPACE, _SPACE, _EXTRA);
//   state = update_tri_layer_state(state, _RAISE, _SYMB, _SPECIAL);
  return state;
}

uint8_t mod_state; // Initialize variable holding the binary representation of active modifiers.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  mod_state = get_mods(); // Store the current modifier state in the variable for later reference
  switch (keycode) {
        case QWERTY:
          if (record->event.pressed) {
            // set_single_persistent_default_layer(_QWERTY);
            layer_off(_MIDI8);
            layer_off(_MIDI4);
            layer_move(_QWERTY);
            stop_all_notes();
            PLAY_SONG(song_plover_gb);
          }
          return false;
          break;
        case MIDI:
          if (record->event.pressed) {
            layer_on(_MIDI4);
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
        case RSFT_T(KC_ASTR): // used on _SPACE layer next to arrow cluster. 
/*         // TO FIX
           // SEND DIVISION SIGN INSTEAD OF _
           // UNREGISTER THE MODS, LIKE THE KC_ASTR EXAMPLE BELOW
         // Send Shift on hold. Send * on tap. Send _ on Shift + tap.
            {
            // Initialize a boolean variable that keeps track of the percent key status: registered or not?
            static bool percentkey_registered;
            if (record->tap.count && record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_PERC);
                    // Update the boolean variable to reflect the status of KC_PERC
                    percentkey_registered = true;
                    return false;
                } else {
                    tap_code16(KC_ASTR); // Send KC_ASTR on tap
                    return false;        // Return false to ignore further processing of key
                }
            } else { // on release of KC_ASTR
                // In case KC_PERC is still being sent even after the release of KC_ASTR
                if (percentkey_registered) {
                    unregister_code16(KC_PERC);
                    percentkey_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_ASTR keycode as usual outside of shift
            return true;
        }
*/
       
// if the above code broke it, revert to this.
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_ASTR); // Send KC_ASTR on tap
                return false;        // Return false to ignore further processing of key
            }
            break;

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
        case AA_SYM: // this isn't used anywhere in the keymap currently. It provides an example of how to use Layer Tap with complex tapped keycodes incl. modifiers.
          if (record->event.pressed) {
            // Records press timer
            bespoke_tap_timer = timer_read();
            // turn on the SYM layer
            layer_on(_SYM); // always turn on _SYM
          } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) { // if tapped, turn _SYM off before sending keypress
            // turn off the SYM layer
            layer_off(_SYM);
            // Sends out 'command-control-space' (character viewer) if the key is held for less than tapping term 
            tap_code16(G(C(KC_SPC)));
          } else { // on key release
            // turn off the SYM layer
            layer_off(_SYM);
          } 
          return false;
          break;
        case AA_TAB: // modifed from https://github.com/davidagross/qmk_firmware/blob/1ccdb0dd461023398076eb3ea92ff050c5aba6ef/keyboards/minidox/keymaps/dgroseph/keymap.c (NAVHOM key) which is based on http://blog.hgao.net/post/qmk-mod-key/
          if (record->event.pressed) { // on keypress turn on _SYM layer
            bespoke_tap_timer = timer_read(); // Records press timer
            layer_on(_SYM); // turn on the _SYM layer
//           } else if (is_alt_tab_active) {
//             // turn off the _SYM layer
//             layer_off(_SYM);
//             // deactivate alt-tab
//             is_alt_tab_active = false;
//             unregister_code(KC_LCMD);
          } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) { // Sends out 'tab' if the key is held for less than tapping term
            layer_off(_SYM); // turn off the _SYM layer
            tap_code(KC_TAB);
            if (!is_alt_tab_active) { // enable alt_tab mode
            // this doesn't actually send any keypressed. it's only necessary so that the tapdance TD_SHIFT_ESC key knows to send Esc with a single tap
              is_alt_tab_active = true;
            }
          } else {
            // turn off the _SYM layer on key release
            layer_off(_SYM);
          } 
          return false;
          break;
/*           
        case AA_MOD3: // modifed from https://github.com/davidagross/qmk_firmware/blob/1ccdb0dd461023398076eb3ea92ff050c5aba6ef/keyboards/minidox/keymaps/dgroseph/keymap.c (NAVHOM key) which is based on http://blog.hgao.net/post/qmk-mod-key/
          if (record->event.pressed) { // on keypress turn on LCMD layer
            bespoke_tap_timer = timer_read(); // Records press timer
            register_mods(MOD_BIT(KC_LCMD)); // press LCMD
//           } else if (is_alt_tab_active) {
//             // turn off the _EXT layer
//             layer_off(_EXT);
//             // deactivate alt-tab
//             is_alt_tab_active = false;
//             unregister_code(KC_LCMD);
          } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) { // Sends out 'return' if the key is held for less than tapping term
            unregister_mods(MOD_BIT(KC_LCMD)); // release LCMD
            tap_code(KC_ENT);
          } else {
            // release LCMD on key release
            unregister_mods(MOD_BIT(KC_LCMD));
            if (is_alt_tab_active) { // deactivate alt-tab on release
            // TBA: may want to also deactivate alt-tab when 'esc', 'return', or '.' are pressed. For now it's fine to assume the user will release the mod key soon after closing the alt-tab switcher.
                is_alt_tab_active = false;
            } 
          } 
*/
          return false;
          break;
        case APP_SWITCHER:
          if (record->event.pressed) {
            if (!is_alt_tab_active) { // ?? hold down on alt_tab after pressing this key (on the _SPACE layer, which is simulating cmd+tab)
              is_alt_tab_active = true;
              register_code(KC_LCMD);
            }
            tap_code(KC_TAB);
          }
          return false;
          break;
//         case APP_SWITCHER_REVERSE:
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
        case AA_RSPC: // modifed from https://github.com/davidagross/qmk_firmware/blob/1ccdb0dd461023398076eb3ea92ff050c5aba6ef/keyboards/minidox/keymaps/dgroseph/keymap.c (NAVHOM key) which is based on http://blog.hgao.net/post/qmk-mod-key/
        // 1) starts timer to check if tapped or held; 2) turn on OSM and _SPACE layer while held; 3) turn off OSM and _SPACE layer when released 
          if (record->event.pressed) {
            // Records press timer
            bespoke_tap_timer = timer_read();
            if (timer_elapsed(bespoke_tap_timer) >= TAPPING_TERM) { // if held, use _SPACE layer
                spacebar_layer_tracker++;
            }
            if (!is_oneshot_enabled()) { // check if oneshot mods are enabled, then turn them on
                oneshot_enable(); // turn ON oneshot mods
            }
          } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) { // if tapped, don't use _SPACE layer
            // turn off the SPACE layer
//            layer_off(_SPACE);
            // Sends out 'space' if the key is held for less than tapping term 
            tap_code(KC_SPACE);
          } else if (timer_elapsed(bespoke_tap_timer) >= TAPPING_TERM) { // if held, use _SPACE layer
//             turn on the SPACE layer
            spacebar_layer_tracker++;
//             layer_on(_SPACE);
          } else { // on key release
            layer_off(_SPACE);
            if ((get_oneshot_mods()) && !has_oneshot_mods_timed_out()) { // turn off active one shot mods
                clear_oneshot_mods();
            }
            if (is_alt_tab_active) { // release alt_tab when releasing this spacebar
                is_alt_tab_active = false;
                unregister_code(KC_LCMD);
            }
            oneshot_disable(); // disable oneshot mods
          } 
          return false;
          break;
        case KC_LPRN: // Shift + `(` = < 
            {
            // Initialize a boolean variable that keeps track of the greater-than status: registered or not?
            static bool greaterthan_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_LABK);
                    // Update the boolean variable to reflect the status of KC_LABK
                    greaterthan_registered = true;
                    return false;
                }
            } else { // on release of KC_LPRN
                // In case KC_LABK is still being sent even after the release of KC_LPRN
                if (greaterthan_registered) {
                    unregister_code16(KC_LABK);
                    greaterthan_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_LPRN keycode as usual outside of shift
            return true;
        }
        case KC_RPRN: // Shift + `)` = < 
            {
            // Initialize a boolean variable that keeps track of the less-than key status: registered or not?
            static bool lessthan_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_RABK);
                    // Update the boolean variable to reflect the status of KC_RABK
                    lessthan_registered = true;
                    return false;
                }
            } else { // on release of KC_RPRN
                // In case KC_RABK is still being sent even after the release of KC_RPRN
                if (lessthan_registered) {
                    unregister_code16(KC_RABK);
                    lessthan_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_RPRN keycode as usual outside of shift
            return true;
        }
        case KC_CIRC: // Shift + `^` = ° 
            {
            // Initialize a boolean variable that keeps track of the degree key status: registered or not?
            static bool degreekey_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_DEGR);
                    // Update the boolean variable to reflect the status of KC_DEGR
                    degreekey_registered = true;
                    return false;
                }
            } else { // on release of KC_CIRC
                // In case KC_DEGR is still being sent even after the release of KC_CIRC
                if (degreekey_registered) {
                    unregister_code16(KC_DEGR);
                    degreekey_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_CIRC keycode as usual outside of shift
            return true;
        }
        case KC_AT: // Shift + `@` = # 
            {
            // Initialize a boolean variable that keeps track of the hash key status: registered or not?
            static bool hashkey_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_HASH);
                    // Update the boolean variable to reflect the status of KC_HASH
                    hashkey_registered = true;
                    return false;
                }
            } else { // on release of KC_AT
                // In case KC_HASH is still being sent even after the release of KC_AT
                if (hashkey_registered) {
                    unregister_code16(KC_HASH);
                    hashkey_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_AT keycode as usual outside of shift
            return true;
        }
//         case KC_EQL: // Shift + `=` = _ 
//             {
//             // Initialize a boolean variable that keeps track of the underscore key status: registered or not?
//             static bool underscore_registered;
//             if (record->event.pressed) {
//                 // Detect the activation of either shift keys
//                 if (mod_state & MOD_MASK_SHIFT) {
//                     register_code16(KC_UNDS);
//                     // Update the boolean variable to reflect the status of KC_UNDS
//                     underscore_registered = true;
//                     return false;
//                 }
//             } else { // on release of KC_EQL
//                 // In case KC_UNDS is still being sent even after the release of KC_EQL
//                 if (underscore_registered) {
//                     unregister_code16(KC_UNDS);
//                     underscore_registered = false;
//                     return false;
//                 }
//             }
//             // Let QMK process the KC_EQL keycode as usual outside of shift
//             return true;
//         }
//         case KC_ASTR: // Shift + `*` = % 
//             {
//             // Initialize a boolean variable that keeps track of the percent key status: registered or not?
//             static bool percentkey_registered;
//             if (record->event.pressed) {
//                 // Detect the activation of either shift keys
//                 if (mod_state & MOD_MASK_SHIFT) {
//                     register_code16(KC_PERC);
//                     // Update the boolean variable to reflect the status of KC_PERC
//                     percentkey_registered = true;
//                     return false;
//                 }
//             } else { // on release of KC_ASTR
//                 // In case KC_PERC is still being sent even after the release of KC_ASTR
//                 if (percentkey_registered) {
//                     unregister_code16(KC_PERC);
//                     percentkey_registered = false;
//                     return false;
//                 }
//             }
//             // Let QMK process the KC_ASTR keycode as usual outside of shift
//             return true;
//         }
        case KC_ASTR: // Shift + `*` = ÷ 
            {
            // Initialize a boolean variable that keeps track of the division key status: registered or not?
            static bool divisionsign_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    // First temporarily canceling both shifts so that shift isn't applied to the KC_DIVISION keycode
                    del_mods(MOD_MASK_SHIFT);
                    register_code16(KC_DIVISION);
                    // Update the boolean variable to reflect the status of KC_DIVISION
                    divisionsign_registered = true;
                    // Reapplying modifier state so that the held shift key(s) still work even after having tapped the Astrisk/Division key.
                    set_mods(mod_state);
                    return false;
                }
            } else { // on release of KC_ASTR
                // In case KC_DIVISION is still being sent even after the release of KC_ASTR
                if (divisionsign_registered) {
                    unregister_code16(KC_DIVISION);
                    divisionsign_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_ASTR keycode as usual outside of shift
            return true;
        }
        case KC_DLR: // Shift + `$` = £ 
            {
            // Initialize a boolean variable that keeps track of the british pound key status: registered or not?
            static bool britishpound_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    // First temporarily canceling both shifts so that shift isn't applied to the KC_BRITISH_POUND keycode
                    del_mods(MOD_MASK_SHIFT);
                    register_code16(KC_BRITISH_POUND);
                    // Update the boolean variable to reflect the status of KC_BRITISH_POUND
                    britishpound_registered = true;
                    // Reapplying modifier state so that the held shift key(s) still work even after having tapped the Dollar/British Pound key.
                    set_mods(mod_state);
                    return false;
                }
            } else { // on release of KC_DLR
                // In case KC_BRITISH_POUND is still being sent even after the release of KC_DLR
                if (britishpound_registered) {
                    unregister_code16(KC_BRITISH_POUND);
                    britishpound_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_DLR keycode as usual outside of shift
            return true;
        }
        case KC_AMPR: // Shift + `&` = ¬ 
            {
            // Initialize a boolean variable that keeps track of the not-sign key status: registered or not?
            static bool notsign_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    // First temporarily canceling both shifts so that shift isn't applied to the KC_NOT_SIGN keycode
                    del_mods(MOD_MASK_SHIFT);
                    register_code16(KC_NOT_SIGN);
                    // Update the boolean variable to reflect the status of KC_NOT_SIGN
                    notsign_registered = true;
                    // Reapplying modifier state so that the held shift key(s) still work even after having tapped the Ampersand/Not-Sign key.
                    set_mods(mod_state);
                    return false;
                }
            } else { // on release of KC_AMPR
                // In case KC_NOT_SIGN is still being sent even after the release of KC_AMPR
                if (notsign_registered) {
                    unregister_code16(KC_NOT_SIGN);
                    notsign_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_AMPR keycode as usual outside of shift
            return true;
        }
        case KC_SECTION: // Shift + `§` = ¶ 
            {
            // Initialize a boolean variable that keeps track of the pilcrow key status: registered or not?
            static bool pilcrowkey_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    // First temporarily canceling both shifts so that shift isn't applied to the KC_PILCROW keycode
                    del_mods(MOD_MASK_SHIFT);
                    register_code16(KC_PILCROW);
                    // Update the boolean variable to reflect the status of KC_PILCROW
                    pilcrowkey_registered = true;
                    // Reapplying modifier state so that the held shift key(s) still work even after having tapped the Ampersand/Not-Sign key.
                    set_mods(mod_state);
                    return false;
                }
            } else { // on release of KC_SECTION
                // In case KC_PILCROW is still being sent even after the release of KC_SECTION
                if (pilcrowkey_registered) {
                    unregister_code16(KC_PILCROW);
                    pilcrowkey_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_SECTION keycode as usual outside of shift
            return true;
        }

        case SHIFTED_SLASHGRAVE_OVERRIDE:
          if (record->event.pressed) {
            if (mod_state & MOD_MASK_SHIFT) { 
              unregister_mods(MOD_BIT(KC_LSFT));
              tap_code(KC_GRV);
              register_mods(MOD_BIT(KC_LSFT));
            } else {
              register_code(KC_SLSH);
            }
          } else {
              unregister_code(KC_SLSH);
          }
          return false;
          break;
//         case KC_SLSH:
//         {
//           static bool slashoverride_registered;
//           if (record->event.pressed) {
//             if (mod_state & MOD_MASK_SHIFT) { 
//               del_mods(MOD_MASK_SHIFT);
//               register_code(KC_GRV);
//               slashoverride_registered = true;
//               set_mods(mod_state);
//               return false;
//             }
//           } else {
//             if (slashoverride_registered) { // In case KC_GRV is still being sent even after the release of KC_GRV
//               unregister_code(KC_GRV);
//               slashoverride_registered = false;
//               return false;
//             }
//             if (get_mods() & MOD_MASK_SHIFT) { 
//               unregister_code(KC_GRV);
//               register_mods(MOD_BIT(KC_LSFT));
//             } else {
//               unregister_code(KC_SLSH);
//             }
//           }
//         }
//         return true; // Let QMK process the KC_SLSH keycode as usual outside of shift
//           break;
      }
    return true;
};

// processed after regular keypress        
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    /* case TD_SPC: */ case TD_LSPC: case TD_RSPC:
      if (record->event.pressed) {
        bespoke_tap_timer = timer_read();
      } else if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) { // if tapped, don't use _SPACE layer
        tap_code(KC_SPACE);
      }
      break;
  }
}

/* per-key hold on other key press configuration */
// SX_GRV aka SFT_T(KC_GRV)
// its supposed to help make the shift/grave key more biased towards the hold (shift) action and reduce delay.
// update: after testing, it seems like it actually helps a lot! tested twice and confirmed to make a major improvment! 
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SX_GRV:
            // Immediately select the hold action (Shift) when another key is pressed.
            return true;
        default:
            // Do not select the hold action when another key is pressed. (Don't send Shift on every single keypress on the keyboard.)
            return false;
    }
}

// copied from documentation, but i switched the default from FALSE to TRUE and made the spacebar the exception!
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case AA_RSPC:
            // Do not immediately select the hold action when another key is tapped.
            return false;
        default:
            // Select the hold action when another key is tapped.
            return true;
    }
}

/* per-key tapping term configuration */
// https://docs.qmk.fm/#/tap_hold?id=tapping-term
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case AA_RSPC: // give the _SPACE layer a longer tapping term, making it less likely to accidentally turn on the layer during regular typing.
            return 65;
        default:
            return TAPPING_TERM;
    }
}

// // [QMK triggers: reacting to interesting events](https://getreuer.info/posts/keyboards/triggers/index.html#action-on-double-tap-without-delay)
// i was trying to make it so that a mod would act as Opt when held and Cmd when double tapped?
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

#define TAPDANCE_BOTTOM
#include "tapdance.inc"
#undef TAPDANCE_BOTTOM