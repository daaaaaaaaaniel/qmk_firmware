#if defined TAPDANCE_TOP
/* Tap Dance keycodes */
enum td_keycodes {
    TD_SHIFT_ESC,
    TD_SPACEBAR,
    TD_LEFT_SPACEBAR,
};
 
/* Define a type containing as many tapdance states as you need */
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP,
//     TD_TRIPLE_TAP,
//     TD_TRIPLE_HOLD
// //     TD_TRIPLE_SINGLE_TAP,
} td_state_t;

// /* Create a global instance of the tapdance state type */
// static td_state_t td_state;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;


/* Declare your tapdance functions */
// Function to determine the current tapdance state
td_state_t cur_dance(qk_tap_dance_state_t *state);

// `finished` and `reset` functions for each tapdance keycode
void shiftesc_finished(qk_tap_dance_state_t *state, void *user_data);
void shiftesc_reset(qk_tap_dance_state_t *state, void *user_data);

void spacebar_finished(qk_tap_dance_state_t *state, void *user_data);
void spacebar_reset(qk_tap_dance_state_t *state, void *user_data);

void leftspacebar_finished(qk_tap_dance_state_t *state, void *user_data);
void leftspacebar_reset(qk_tap_dance_state_t *state, void *user_data);

/************************************************************************/

#elif defined TAPDANCE_BOTTOM

// Determine the tapdance state to return
td_state_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
//     }
//     if (state->count == 2) return TD_DOUBLE_SINGLE_TAP;
//     else return TD_UNKNOWN; // Any number higher than the maximum state value you return above
// }
// 
// // example 2
// td_state_t cur_dance(qk_tap_dance_state_t *state) {
//     if (state->count == 1) {
//         if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
//         else return TD_SINGLE_HOLD; // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
    } else if (2 <= state->count && state->count < 5) {
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP; // Suggested use case for TD_DOUBLE_SINGLE_TAP is when you want to send two keystrokes of the key, and not the 'double tap' action/macro.
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
//     }
//     // Assumes no one is trying to type the same letter three times (at least not quickly). If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
//     if (state->count == 3) {
//         if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
//         else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN; // Any number higher than the maximum state value you return above
}

// Initialize tap structure associated with example tap dance key
// Create an instance of 'td_tap_t' for the sftesc tap dance.
static td_tap_t sftesc_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
static td_tap_t spacebar_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
static td_tap_t leftspacebar_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// Handle the possible states for each tapdance keycode you define

void shiftesc_finished(qk_tap_dance_state_t *state, void *user_data) {
    sftesc_tap_state.state = cur_dance(state);
    switch (sftesc_tap_state.state) {
        case TD_SINGLE_TAP:
            register_mods(MOD_BIT(KC_LSFT));
            if (is_alt_tab_active) { // is alt tab active?
                unregister_mods(MOD_BIT(KC_LSFT));
                tap_code(KC_DOT); // then close the switcher
            }
            break;
        case TD_SINGLE_HOLD:
            register_mods(MOD_BIT(KC_LSFT)); // For a layer-tap key, use `layer_on(_MY_LAYER)` here
            if (is_alt_tab_active) { // is alt tab active?
                tap_code(KC_DOT); // then close the switcher
                is_alt_tab_active = false;
            }
            break;
        case TD_DOUBLE_TAP:
            register_code(KC_ESC);
            register_mods(MOD_BIT(KC_LSFT));
            break;
        case TD_DOUBLE_HOLD:
            tap_code(KC_ESC);
            layer_on(_EXT); // register_code(KC_LSFT);
            break;
        case TD_DOUBLE_SINGLE_TAP: // Allow nesting of 2 tapped keys within tapping term
            register_code(KC_ESC);
            register_mods(MOD_BIT(KC_LSFT)); // maybe switch this to one-shot Shift to mitigate weirdness from tapping too many times?
            break;
        default:
            break;
    }
}
void shiftesc_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (sftesc_tap_state.state) {
        case TD_SINGLE_TAP:
            if (is_alt_tab_active) { // is alt tab active?
                is_alt_tab_active = false;
            } else { // If not in alt tab, unsend Shift
                unregister_mods(MOD_BIT(KC_LSFT));
            }
            break;
        case TD_SINGLE_HOLD:
            unregister_mods(MOD_BIT(KC_LSFT)); // For a layer-tap key, use `layer_off(_MY_LAYER)` here
            break;
        case TD_DOUBLE_TAP:
        	unregister_code(KC_ESC);
            unregister_mods(MOD_BIT(KC_LSFT));
            break;
        case TD_DOUBLE_HOLD:
            layer_off(_EXT); // unregister_mods(MOD_BIT(KC_LSFT));
            break;
        case TD_DOUBLE_SINGLE_TAP:
        	unregister_code(KC_ESC);
            unregister_mods(MOD_BIT(KC_LSFT));
            break;
        default:
            break;
    }
    sftesc_tap_state.state = TD_NONE;
}

void spacebar_each(qk_tap_dance_state_t *state, void *user_data) {
//     bespoke_tap_timer = timer_read();
//     if (timer_elapsed(bespoke_tap_timer) < TAPPING_TERM) { // if tapped, don't use _SPACE layer
//         tap_code(KC_SPACE);
//     } 
}
void spacebar_finished(qk_tap_dance_state_t *state, void *user_data) {
    spacebar_tap_state.state = cur_dance(state);
    switch (spacebar_tap_state.state) {
        case TD_SINGLE_TAP:
            break;
        case TD_SINGLE_HOLD:
            spacebar_layer_tracker++; // this should allow the `post_process_record_user` to go to work!
            layer_on(_SPACE);
            break;
        case TD_DOUBLE_TAP:
            break;
        case TD_DOUBLE_HOLD:
            break;
        case TD_DOUBLE_SINGLE_TAP:
            break;
        default:
            break;
    }
}
void spacebar_reset(qk_tap_dance_state_t *state, void *user_data) {
    if(spacebar_layer_tracker) {
        layer_off(_SPACE);
        spacebar_layer_tracker--;
    }
    if (is_alt_tab_active) { // release alt_tab when releasing this spacebar
        is_alt_tab_active = false;
        unregister_code(KC_LCMD);
    }
    spacebar_tap_state.state = TD_NONE;
}

void leftspacebar_finished(qk_tap_dance_state_t *state, void *user_data) {
    leftspacebar_tap_state.state = cur_dance(state);
    switch (leftspacebar_tap_state.state) {
        case TD_SINGLE_TAP:
            break;
        case TD_SINGLE_HOLD:
            leftspacebar_layer_tracker++; // this should allow the `post_process_record_user` to go to work!
            layer_on(_L_SPACE);
            break;
        case TD_DOUBLE_TAP:
            break;
        case TD_DOUBLE_HOLD:
            break;
        case TD_DOUBLE_SINGLE_TAP:
            break;
        default:
            break;
    }
}
void leftspacebar_reset(qk_tap_dance_state_t *state, void *user_data) {
    if(leftspacebar_layer_tracker) {
        layer_off(_L_SPACE);
        leftspacebar_layer_tracker--;
    }
    if (is_alt_tab_active) { // release alt_tab when releasing this spacebar
        is_alt_tab_active = false;
        unregister_code(KC_LCMD);
    }
    leftspacebar_tap_state.state = TD_NONE;
}


/* Tap Dance definitions */
// Define `ACTION_TAP_DANCE_DOUBLE` or `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
qk_tap_dance_action_t tap_dance_actions[] = {
//     [TD_SHIFT_ESC] = ACTION_TAP_DANCE_DOUBLE(KC_LSFT, KC_ESC), // Tap once for Shift, twice for Escape
    [TD_SHIFT_ESC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, shiftesc_finished, shiftesc_reset), // Tap once for Shift, twice for Escape, 
    [TD_SPACEBAR] = ACTION_TAP_DANCE_FN_ADVANCED(spacebar_each, spacebar_finished, spacebar_reset),
    [TD_LEFT_SPACEBAR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, leftspacebar_finished, leftspacebar_reset)
};


#else
#error Incorrect defines set to use this include file (none set)
#endif
/* Add tap dance item to your keymap in place of a keycode */
//     …, TD(TD_SHIFT_ESC), …