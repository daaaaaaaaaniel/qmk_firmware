# MCU name
MCU = STM32F303

# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE =  yes     # Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
# CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = no         # Magic commands for debug and configuration
NKRO_ENABLE = yes           # N-Key Rollover (Must be set to 'no' to activate enable Apple FN patch)
BACKLIGHT_ENABLE = no       # Keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Keyboard RGB underglow
AUDIO_ENABLE = yes          # Audio output
WS2812_DRIVER = pwm
CUSTOM_MATRIX = yes
# ENCODER_ENABLE = yes
# DIP_SWITCH_ENABLE = yes
# KEY_OVERRIDE_ENABLE = yes   # Key overrides

# Do not enable RGB_MATRIX_ENABLE together with RGBLIGHT_ENABLE
RGB_MATRIX_ENABLE = no
RGB_MATRIX_DRIVER = WS2812

MIDI_ENABLE = yes
COMBO_ENABLE = yes
WAIT_FOR_USB = yes			# maybe this will fix startup sound?
# TAP_DANCE_ENABLE = yes

SRC += muse.c
SRC += matrix.c
# SRC += swapper.c

LAYOUTS = ortho_5x12