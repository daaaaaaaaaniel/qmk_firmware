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

// defining my custom variables. i use they for toggling on the Raise and Lower layers.
bool is_lower_key_held = false;
bool is_raise_key_held = false;
bool is_layer_toggled_on = false;

/* declare layers */
enum preonic_layers {
  _QWERTY,
  _INV,
  _LOWER,
  _RAISE,
  _TAB,
  _TAB_MASK,
  _EXTRA,
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
  AR_TOGGLE_LOWER,
  AL_TOGGLE_RAISE,
//   TEXT_DESELECT,
  TEXT_SELECT_WORD,
};

// Tap Dance declarations
enum td_keycodes {
    TD_ENTER, // RShift when held, one-shot RShift + Enter when tapped. Add additional keycodes for each tapdance.
//     TD_LSFT
};

// Define a type containing as many tapdance states as you need
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
//     TD_DOUBLE_SINGLE_TAP,
    TD_DOUBLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// // Create a global instance of the tapdance state type
// static td_state_t td_state;

/* Declare your tapdance functions */
// Function to determine the current tapdance state
td_state_t cur_dance(qk_tap_dance_state_t *state);
// `finished` and `reset` functions for each tapdance keycode
void shiftent_finished(qk_tap_dance_state_t *state, void *user_data);
void shiftent_reset(qk_tap_dance_state_t *state, void *user_data);
// void lshiftesc_finished(qk_tap_dance_state_t *state, void *user_data);
// void lshiftesc_reset(qk_tap_dance_state_t *state, void *user_data);

#define AA_RCMD RCMD_T(KC_BSPC) // command (hold); backspace (tap)
#define AA_ROPT ROPT_T(KC_DEL) // option (hold); delete (tap)
#define AA_RCTL RCTL_T(KC_LBRC)
// #define AA_LCMD // | (old version) - OSM(MOD_LGUI) // one-shot command
// #define AA_LOPT // | (old version) // OSM(MOD_LALT) // one-shot option
// #define AA_LCTL // | (old version) // OSM(MOD_LCTL) // one-shot control
// #define AA_LSFT // | (old version) // OSM(MOD_LSFT)// one-shot left shift
#define AA_RSFT OSM(MOD_RSFT)// one-shot right shift
#define SX_ESC SFT_T(KC_ESC) // shift (hold); escape (tap)
#define DD_CMD LCMD_T(KC_DEL) // command (hold); delete (tap)
#define DD_BSPC ROPT_T(KC_BSPC) // option (hold); backspace (tap)
#define KX_CUT LCMD(KC_X) // command-x
#define KX_COPY LCMD(KC_C) // command-c
#define KX_PSTE LCMD(KC_V) // command-v
#define SHOW_ALL_APP_WINDOWS LCTL(KC_DOWN) // Control-Down Arrow
#define NEXTTAB RSG(KC_RBRC) // Command-Shift-] (or perhaps Control-Tab)
#define PREVTAB RSG(KC_LBRC) // Command-Shift-[ (or perhaps Control-Shift-Tab)
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
#define TX_SEL TEXT_SELECT_WORD
/* keycodes for moving between layers */
#define AA_TAB LT(_TAB, KC_TAB)
#define AA_LSPC LT(_LOWER,KC_SPACE)
#define AA_RSPC LT(_RAISE,KC_SPACE)
#define AR_LOWR AR_TOGGLE_LOWER
#define AL_RAIS AL_TOGGLE_RAISE
/* keycodes for Amethyst */
#define AM_MOD1 LM(_QWERTY, MOD_LALT | MOD_LSFT)
#define AM_MOD2 LM(_QWERTY, MOD_MEH) // Control + Option + Shift 


/* Sounds */
#ifdef AUDIO_ENABLE
  float song_preonic_sound[][2] = SONG(PREONIC_SOUND); // song for switching into MIDI layer 
  float song_plover_gb[][2]  = SONG(PLOVER_GOODBYE_SOUND);  // song for switching out of MIDI layer (back to QWERTY)
#endif

/* Combos */
// When both SPACE keys are tapped together, execute ENTER. When both SPACE keys are HELD, activate _EXTRA layer.
enum combos {
  SPC_ENTER,
};

const uint16_t PROGMEM adj_combo[] = {LT(_LOWER, KC_SPC), LT(_RAISE, KC_SPC), COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
 [SPC_ENTER] = COMBO(adj_combo, LT(_EXTRA, KC_ENT)), // HITTING and HOLDING both space bars together activates _EXTRA layer (Combo that executes a layer tap), although concievably it could activate any layer, thus making holding the COMBO different from pressing the keys at different times and holding them. 
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY (Default layer)
 * ,-----------------------------------------------------------------------------------.
 * |  Esc |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   `  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  | Enter|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  fn  | Ctrl | Opt  | Cmd  |     Lower   |    Raise    | Bksp |  Del |   [  |   ]  |
 * `-----------------------------------------------------------------------------------'
 */
[_QWERTY] = LAYOUT_preonic_2x2u(
  KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  KC_GRV,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
  AA_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, LT(_TAB, KC_QUOT),
  SX_ESC,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, TD(TD_ENTER), /* AA_RSFT, */
  DF(_INV)/* KC_CAPS */, KC_LCTL, KC_LOPT, KC_LCMD,     AA_LSPC,          AA_RSPC,      AA_RCMD, AA_ROPT, AA_RCTL, KC_RBRC
),

/* Inverted Space Bars (inverted base layer)
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |     Lower   |    Raise    |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_INV] = LAYOUT_preonic_2x2u(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  DF(_QWERTY), _______, _______, _______,     AA_RSPC,          AA_LSPC,      _______, _______, _______, _______
),

/* Raise (Navigation layer) - holding Right Space
 * ,-----------------------------------------------------------------------------------.
 * |LockSc|Brght-|Brght+|MsnCtl|Lnchpd| Dict |DoNDst| Rwnd | Play | Ffwd | Mute | Ctrl |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | PgUp |      |  MUp | Click|RClick|SwapCh|WoSel←| Sel ←| Sel →|WoSel→|  Up  |      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | PgDwn| MLeft| MDown|MRight| Click|WrdSel|WrdBck| Left |Right |WrdFwd| Down |LinEnd|
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |  Esc |      | Cut  | Copy | Paste|      |      |      |      |      |      | Shift|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |  Del | Lower (Lock)|             |      |      |      | Enter|
 * `-----------------------------------------------------------------------------------'
 */
[_RAISE] = LAYOUT_preonic_2x2u(
  KC_LOCK, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, KC_RCTL,
  KC_PGUP, KC_EXLM, KC_MS_U, KC_BTN1, KC_BTN2, KX_SWAP, SELWPRV, SEL_PRV, SEL_NXT, SELWNXT, KC_UP,   KC_NO,
  KC_PGDN, KC_MS_L, KC_MS_D, KC_MS_R, KC_BTN1, TX_SEL,  WD_PREV, KC_LEFT, KC_RGHT, WD_NEXT, KC_DOWN, MO(_TAB),
  SX_ESC,  KC_NO,   KX_CUT,  KX_COPY, KX_PSTE, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   AA_RSFT,
  _______, _______, _______, DD_CMD,      AR_LOWR,          _______,      _______, _______, KC_RCTL, KC_ENT
),

/* Lower (Symbol layer) - holding Left Space
 * ,-----------------------------------------------------------------------------------.
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |   ~  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  |  +   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |AppSwi|   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  Esc |      |      |      |      |      |   {  |   }  |   [  |   ]  |   \  |  |   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | MIDI |      |      |      |             | Raise (Lock)|  Del | Bksp |      | Enter|
 * `-----------------------------------------------------------------------------------'
 */
[_LOWER] = LAYOUT_preonic_2x2u(
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PLUS,
  ALL_APP, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
  SX_ESC,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC, KC_BSLS, KC_PIPE,
  MIDI,    _______, _______, _______,      _______,  AL_RAIS,      RCMD_T(KC_DEL), DD_BSPC, _______, KC_ENT
),

/* Tab (Window Switcher layer) - holding Tab
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |AmMod2|AmMod1|      |      |SwapCh|      |  Up  |      |PrvTab|NxtTab|      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      |//Opt/| Shift|  Cmd |      |WrdSel|WrdBck| Left |Right |WrdFwd|      |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      | Down |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |   PrevTab   |   NextTab   |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_TAB] = LAYOUT_preonic_2x2u( // NOTE: add a tap dance routine to LineStart/LineEnd such that tapping once goes to the start of the line and each additional tap goes up/down by a line !! this will make it feel more like clockwise/counterclockwise movement
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, AM_MOD2, AM_MOD1, _______, _______, KX_SWAP, _______, KC_UP,   _______, PREVTAB, NEXTTAB, _______,
  MO(_TAB),MO(_TAB_MASK),KC_LSFT,KC_LCMD,_______,TX_SEL,WD_PREV, KC_LEFT, KC_RGHT, WD_NEXT, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, KC_DOWN, _______, _______, _______,
  _______, _______, _______, _______,     PREVTAB,          NEXTTAB,      _______, _______, _______, _______
),

/* Mod Tabs - replace left/right with up/down
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |AmMod2|AmMod1|      |      |SwapCh|      |  Up  |      |PrvTab|NxtTab|      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      |  Opt | Shift|  Cmd |      |WrdSel| Left |  Up  | Down |Right |      |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      | Down |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |   PrevTab   |   NextTab   |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_TAB_MASK] = LAYOUT_preonic_2x2u( // NOTE: add a tap dance routine to LineStart/LineEnd such that tapping once goes to the start of the line and each additional tap goes up/down by a line !! this will make it feel more like clockwise/counterclockwise movement
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, AM_MOD2, AM_MOD1, _______, _______, KX_SWAP, _______, KC_UP,   _______, PREVTAB, NEXTTAB, _______,
  MO(_TAB),_______, KC_LSFT, KC_LCMD, _______, TX_SEL,  KC_LEFT, KC_UP,   KC_DOWN, KC_RGHT, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, KC_DOWN, _______, _______, _______,
  _______, _______, _______, _______,     PREVTAB,          NEXTTAB,      _______, _______, _______, _______
),

/* Extra (Media Functions layer) - holding Left Space + Right Space
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
[_EXTRA] = LAYOUT_preonic_2x2u(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  ALL_APP, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_DICT, KC_DOND, KC_MRWD, KC_MPLY, KC_MFFD, KC_MUTE, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU,
  _______, _______, _______, _______,     _______,          _______,      _______, _______, _______, _______
),

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
        case AR_TOGGLE_LOWER: // pressing _LOWER while on the _RAISE layer
          if (record->event.pressed) {
            if (is_raise_key_held == 0) {
              layer_off(_RAISE);
//               SEND_STRING("___________RAISE-off__");tap_code16(KC_ENT);
//               SEND_STRING("___________RAISE-off__");tap_code16(KC_ENT);
            }
            is_lower_key_held = true;
            layer_on(_LOWER);
//             SEND_STRING("__________AR_LOWR pressed__");tap_code16(KC_ENT);
          } else { // key up
              // if both keys were held together, toggle ON the _LOWER layer 
              if (is_raise_key_held == 0 && is_layer_toggled_on == true) { // release from pressing from hands-free mode
                is_layer_toggled_on = false;
                layer_off(_RAISE);
                layer_off(_LOWER);
//                 SEND_STRING("__________LOWER_KEYUP-release-hands-free-mode__");tap_code16(KC_ENT);
              }
              if (is_raise_key_held == 1) {
                is_layer_toggled_on = true;
                layer_off(_RAISE);
//                 SEND_STRING("__________KEYUP_hands-free-on_LOWER__");tap_code16(KC_ENT);
              }
            // in every case
            is_lower_key_held = false;
//             SEND_STRING("__________KEYUP_LOWER__");tap_code16(KC_ENT);
          }
          break;
        case AL_TOGGLE_RAISE: // pressing _RAISE while on the _LOWER layer
          if (record->event.pressed) {
            if (is_lower_key_held == 0) {
              layer_off(_LOWER);
//               SEND_STRING("__________hands-free-raise-on__");tap_code16(KC_ENT);
            }
            is_raise_key_held = true;
            layer_on(_RAISE);
//             SEND_STRING("__________AL_RAIS pressed__");tap_code16(KC_ENT);
          } else { // key up
              if (is_lower_key_held == 0 && is_layer_toggled_on == true) { // release from pressing while in hands-free mode
                is_layer_toggled_on = false;
                layer_off(_LOWER);
                layer_off(_RAISE);
//                 SEND_STRING("__________RAISE_KEYUP-release-hands-free-mode__");tap_code16(KC_ENT);
              }
              if (is_lower_key_held == 1) { // both keys held, don't turn off the _RAISE layer
                is_layer_toggled_on = true;
                layer_off(_LOWER);
//                 SEND_STRING("__________KEYUP-hands-free-on_RAISE__");tap_code16(KC_ENT);
              }
            // key up
            is_raise_key_held = false;
//             SEND_STRING("__________KEYUP_RAISE__");tap_code16(KC_ENT);
          }
          break;
        case LT(_RAISE,KC_SPACE): // Space/Raise (layer tap)
        // this is kind of broken because it adds a space if its tapped, wheen really it should just disable the layer toggle. fix it by changing the line before "break;" to "return false;", and then change the press condition to include "&& record->event.pressed" to intercept taps, and add an "else if (record->event.pressed)" to incercept holds. 
            if (record->event.pressed) {
                is_raise_key_held = true;
//                 SEND_STRING("__________AA_SPACE_RAISE pressed__");tap_code16(KC_ENT);
            } else { // Intercept hold function
                is_raise_key_held = false;
                layer_off(_RAISE);
//                 SEND_STRING("__________AA_SPACE_RAISE_release_hold__");tap_code16(KC_ENT);
            }
            return true;             // Return true for normal processing of tap keycode
            break;
        case LT(_LOWER,KC_SPACE): // Space/Lower (layer tap)
        // this is kind of broken because it adds a space if its tapped, wheen really it should just disable the layer toggle. fix it by changing the line before "break;" to "return false;", and then change the press condition to include "&& record->event.pressed" to intercept taps, and add an "else if (record->event.pressed)" to incercept holds. 
            if (record->event.pressed) {
                is_lower_key_held = true;
//                 SEND_STRING("__________AA_SPACE_LOWER pressed__");tap_code16(KC_ENT);
            } else { 
                is_lower_key_held = false;
                layer_off(_LOWER);
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

bool encoder_update_user(uint8_t index, bool clockwise) {
  if (muse_mode) {
    if (IS_LAYER_ON(_RAISE)) {
      if (clockwise) {
        muse_offset++;
      } else {
        muse_offset--;
      }
    } else {
      if (clockwise) {
        muse_tempo+=1;
      } else {
        muse_tempo-=1;
      }
    }
  } else {
    if (clockwise) {
      register_code(KC_PGDN);
      unregister_code(KC_PGDN);
    } else {
      register_code(KC_PGUP);
      unregister_code(KC_PGUP);
    }
  }
    return true;
}

//* Leftover from default keymap *//
//
// bool dip_switch_update_user(uint8_t index, bool active) {
//     switch (index) {
//         case 0:
//             if (active) {
//                 layer_on(_ADJUST);
//             } else {
//                 layer_off(_ADJUST);
//             }
//             break;
//         case 1:
//             if (active) {
//                 muse_mode = true;
//             } else {
//                 muse_mode = false;
//             }
//     }
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

/* 
 * bool music_mask_user(uint16_t keycode) {
 *   switch (keycode) {
 *     case RAISE:
 *     case LOWER:
 *       return false;
 *     default:
 *       return true;
 *   }
 * }
 */
 
/* Tap Dance */
// Determine the tapdance state to return
td_state_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap action when hitting 'pp'. Suggested use case for this return value is when you want to send two keystrokes of the key, and not the 'double tap' action/macro.
//         if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
//         else if (state->pressed) return TD_DOUBLE_HOLD;
//         else return TD_DOUBLE_TAP;
        if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    } else return TD_UNKNOWN;
//     if (state->count == 2) return TD_DOUBLE_TAP;
//     else return TD_UNKNOWN; // Any number higher than the maximum state value you return above
}


// Handle the possible states for each tapdance keycode you define
/* One-Shot RIFHT Shift / Enter */
// Create an instance of 'td_tap_t' for the 'shiftent' tap dance.
static td_tap_t shiftent_tap_state = {
    .is_press_action = true,
    .state = 0 // .state = TD_NONE
};
void shiftent_finished(qk_tap_dance_state_t *state, void *user_data) {
    shiftent_tap_state.state = cur_dance(state);
    switch (shiftent_tap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_ENT);
            set_oneshot_mods(MOD_BIT(KC_RSFT)); // emulate one shot mod
            break;
        case TD_SINGLE_HOLD:
//             register_mods(MOD_BIT(MOD_RSFT));
            register_code(KC_RSFT);
            break;
        case TD_DOUBLE_TAP:
            register_code(KC_ENT);
            break;
//         case TD_DOUBLE_SINGLE_TAP:
//             set_oneshot_mods(MOD_BIT(KC_RSFT));
// //             register_code(KC_ENT);
//             break;
        case TD_DOUBLE_HOLD:
            tap_code(KC_ENT);
            tap_code(KC_ENT);
            register_code(KC_RSFT);
            break;
        default:
            if (state->count > 2) {
                for (uint8_t i=0; i < state->count; ++i) {
                    tap_code(KC_ENT);
                    if (state->pressed) {
                        register_code(KC_RSFT);
                    }
                }
//                 register_code16(KC_ENT);
            }
            break;
    }
}
void shiftent_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (shiftent_tap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_ENT);
            break;
        case TD_SINGLE_HOLD:
//             clear_oneshot_mods();
            unregister_code(KC_RSFT);
//             unregister_mods(MOD_BIT(KC_RSFT));
            break;
        case TD_DOUBLE_TAP:
            unregister_code(KC_ENT);
            break;
//         case TD_DOUBLE_SINGLE_TAP:
// //             unregister_code(KC_ENT);
// //             unregister_code(KC_ENT);
//             break;
        case TD_DOUBLE_HOLD:
            unregister_code16(KC_RSFT);
            break;
        default:
//             unregister_code(KC_ENT);
//             unregister_code(KC_ENT);
            if (!state->pressed) {
                unregister_code16(KC_RSFT);
            }
            break;
    }
    shiftent_tap_state.state = 0; // shiftent_tap_state.state = TD_NONE;
}

// /* One-Shot LEFT Shift / Double-tap Escape */
// static td_tap_t lshiftesc_tap_state = {
//     .is_press_action = true,
//     .state = 0
// };
// void lshiftesc_finished(qk_tap_dance_state_t *state, void *user_data) {
//     lshiftesc_tap_state.state = cur_dance(state);
//     switch (lshiftesc_tap_state.state) {
//         case TD_SINGLE_TAP:
//             set_oneshot_mods(MOD_BIT(KC_LSFT)); // emulate one shot mod
//             break;
//         case TD_SINGLE_HOLD:
//             register_code(KC_LSFT);
//             break;
//         case TD_DOUBLE_TAP:
//             register_code(KC_ESC);
//             break;
//         case TD_DOUBLE_HOLD:
//             tap_code(KC_ESC);
//             register_code(KC_LSFT);
//             break;
//         default:
//             break;    
//     }
// }
// void lshiftesc_reset(qk_tap_dance_state_t *state, void *user_data) {
//     switch (lshiftesc_tap_state.state) {
//         case TD_SINGLE_TAP:
//             break;
//         case TD_SINGLE_HOLD:
// //             clear_oneshot_mods();
//             unregister_code(KC_LSFT);
// //             unregister_mods(MOD_BIT(KC_LSFT));
//             break;
//         case TD_DOUBLE_TAP:
//             unregister_code(KC_ESC);
//             break;
//         case TD_DOUBLE_HOLD:
//             unregister_code(KC_LSFT);
//             break;
//         default:
//             break;
//     }
//     lshiftesc_tap_state.state = 0;
// }


// Define `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
qk_tap_dance_action_t tap_dance_actions[] = {
//     [TD_ENTER] = ACTION_TAP_DANCE_DOUBLE(KC_RSFT, KC_ENT), // simple: Tap once for RShift, twice for Enter
    [TD_ENTER] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, shiftent_finished, shiftent_reset), // complex: once for Enter + one-shot Shift
//     [TD_LSFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lshiftesc_finished, lshiftesc_reset)
};
