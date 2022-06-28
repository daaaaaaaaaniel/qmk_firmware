 const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

 /*  
 * ,------+------+------+------.
 * |   (  |   [  |   {  |   <  |
 * `------+------+------+------'
 *
 *  ⇧Shift
 * ,------+------+------+------.
 * |   )  |   ]  |   }  |   >  |
 * `------+------+------+------'
 */
[_SYM] = LAYOUT_preonic_grid(
  KC_LPRN,   KC_LBRC,   KC_LCBR,   KC_LABK,   _______
)

};


uint8_t mod_state; // Initialize variable holding the binary representation of active modifiers.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  mod_state = get_mods(); // Store the current modifier state in the variable for later reference
  switch (keycode) {
        case KC_LPRN: // Shift + `(` = `)` 
            {
            // Initialize a boolean variable that keeps track of the right parenthesis key status: registered or not?
            static bool rightparenth_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_RPRN);
                    // Update the boolean variable to reflect the status of KC_RPRN
                    rightparenth_registered = true;
                    return false;
                }
            } else { // on release of KC_LPRN
                // In case KC_RPRN is still being sent even after the release of KC_LPRN
                if (rightparenth_registered) {
                    unregister_code16(KC_RPRN);
                    rightparenth_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_LPRN keycode as usual outside of shift
            return true;
        }
        case KC_LBRC: // Shift + `[` = `]` 
            {
            // Initialize a boolean variable that keeps track of the close-bracket status: registered or not?
            static bool closebracket_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    // First temporarily canceling both shifts so that shift isn't applied to the KC_RBRC keycode
                    del_mods(MOD_MASK_SHIFT);
                    register_code16(KC_RBRC);
                    // Update the boolean variable to reflect the status of KC_RBRC
                    closebracket_registered = true;
                    // Reapplying modifier state so that the held shift key(s) still work even after having tapped the bracket key.
                    set_mods(mod_state);
                    return false;
                }
            } else { // on release of KC_LBRC
                // In case KC_RBRC is still being sent even after the release of KC_LBRC
                if (closebracket_registered) {
                    unregister_code16(KC_RBRC);
                    closebracket_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_LBRC keycode as usual outside of shift
            return true;
        }
        case KC_LCBR: // Shift + `{` = `}`
            {
            // Initialize a boolean variable that keeps track of the close-curly-bracked status: registered or not?
            static bool closecurlybracket_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_RCBR);
                    // Update the boolean variable to reflect the status of KC_RCBR
                    closecurlybracket_registered = true;
                    return false;
                }
            } else { // on release of KC_LCBR
                // In case KC_RCBR is still being sent even after the release of KC_LCBR
                if (closecurlybracket_registered) {
                    unregister_code16(KC_RCBR);
                    closecurlybracket_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_LCBR keycode as usual outside of shift
            return true;
        }
        case KC_LABK: // Shift + `<` = `>`
            {
            // Initialize a boolean variable that keeps track of the greater-than status: registered or not?
            static bool greaterthan_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_SHIFT) {
                    register_code16(KC_RABK);
                    // Update the boolean variable to reflect the status of KC_RABK
                    greaterthan_registered = true;
                    return false;
                }
            } else { // on release of KC_LABK
                // In case KC_RABK is still being sent even after the release of KC_LABK
                if (greaterthan_registered) {
                    unregister_code16(KC_RABK);
                    greaterthan_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_LABK keycode as usual outside of shift
            return true;
        }
      }
    return true;
};