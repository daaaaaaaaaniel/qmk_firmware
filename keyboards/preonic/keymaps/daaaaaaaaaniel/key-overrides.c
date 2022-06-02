/* Key Overrides */
// https://docs.qmk.fm/#/feature_key_overrides?id=key-overrides

/* made redundant by QK_GESC QK_GRAVE_ESCAPE
// Shift + esc = ~
const key_override_t shift_tilde_esc_override = ko_make_with_layers(MOD_MASK_SHIFT, KC_ESC, S(KC_GRV), 1<<_QWERTY);
// // GUI + esc = window focus (GUI`)
// const key_override_t cmd_grave_esc_override = ko_make_with_layers(MOD_MASK_GUI, KC_ESC, WN_FOCU, 1<<_QWERTY); // broken with karabiner? needs further testing
*/

/* 
// impossible to do what I want with key overrides. need to use a macro instead. the key overrides should only kick in if is_alt_tab_active == true.
// GUI + tapped Left Shift(hold/grave(tap) = escape(tap)/shift(hold) // aka SX_GRV or SFT_T(KC_GRV) (or Left Shift)
const key_override_t cmd_esc_grave_override = ko_make_with_layers_negmods_and_options(MOD_MASK_GUI, SFT_T(KC_GRV),
                                            SFT_T(KC_ESC), 1<<_QWERTY, MOD_MASK_CSA, ko_option_no_reregister_trigger);
 */
                                           
// This globally defines all key overrides to be used
const key_override_t **key_overrides = (const key_override_t *[]){
//     &shift_tilde_esc_override,
//     &cmd_grave_esc_override,
//     &cmd_esc_grave_override,
    NULL // Null terminate the array of overrides!
};
/* END Key Overrides */