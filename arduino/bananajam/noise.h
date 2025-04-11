#pragma once

// https://onlinesequencer.net/3857464

const uint16_t THEME_TONES[] PROGMEM = {
    NOTE_G3,   34,  NOTE_C5,   136, NOTE_C5, 136, NOTE_C5,   136, NOTE_C5,  136,
    NOTE_G4,   204, NOTE_REST, 68,  NOTE_G4, 136, NOTE_E4,   136, NOTE_E4,  136,
    NOTE_E4,   136, NOTE_E4,   136, NOTE_C4, 272, NOTE_REST, 136, NOTE_C4,  136,
    NOTE_C4,   136, NOTE_C4,   136, NOTE_C4, 136, NOTE_C4,   136, NOTE_C4,  136,
    NOTE_D4,   204, NOTE_REST, 68,  NOTE_C4, 204, NOTE_REST, 68,  NOTE_C4,  204,
    NOTE_REST, 204, NOTE_C4,   68,  NOTE_E4, 68,  NOTE_C5,   136, TONES_END};

const uint16_t CHANGE_TONES[] PROGMEM = {
    NOTE_G3, 34, NOTE_C4, 68, NOTE_E4, 68, NOTE_C5, 136, TONES_END};

const uint16_t MOVE_TONES[] PROGMEM = {NOTE_G3, 17, NOTE_E3,  17,
                                       NOTE_B3, 17, TONES_END};

const uint16_t JUMP_TONES[] PROGMEM = {NOTE_E4, 17, NOTE_D4,  17, NOTE_C4, 17,
                                       NOTE_E3, 17, TONES_END};

const uint16_t CROUCH_TONES[] PROGMEM = {NOTE_E3, 34, NOTE_F3, 34, TONES_END};
