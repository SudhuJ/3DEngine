local flowInput = {}

-- check mouse input
function flowInput.isMouse(button)
    return APIMouseDown(button)
end

-- check key input
function flowInput.isKey(key)
    return APIisKeyDown(key)
end

-- mouse input codes
flowInput.MOUSE_LEFT = 0
flowInput.MOUSE_MIDDLE = 1
flowInput.MOUSE_RIGHT = 2
flowInput.MOUSE_BUTTON_4 = 3
flowInput.MOUSE_BUTTON_5 = 4
flowInput.MOUSE_BUTTON_6 = 5
flowInput.MOUSE_BUTTON_7 = 6
flowInput.MOUSE_BUTTON_8 = 7

-- key input codes
flowInput.KEY_SPACE = 32
flowInput.KEY_APOSTROPHE = 39
flowInput.KEY_COMMA = 44
flowInput.KEY_MINUS = 45
flowInput.KEY_PERIOD = 46
flowInput.KEY_SLASH = 47
flowInput.KEY_0 = 48
flowInput.KEY_1 = 49
flowInput.KEY_2 = 50
flowInput.KEY_3 = 51
flowInput.KEY_4 = 52
flowInput.KEY_5 = 53
flowInput.KEY_6 = 54
flowInput.KEY_7 = 55
flowInput.KEY_8 = 56
flowInput.KEY_9 = 57
flowInput.KEY_SEMICOLON = 59
flowInput.KEY_EQUAL = 61
flowInput.KEY_A = 65
flowInput.KEY_B = 66
flowInput.KEY_C = 67
flowInput.KEY_D = 68
flowInput.KEY_E = 69
flowInput.KEY_F = 70
flowInput.KEY_G = 71
flowInput.KEY_H = 72
flowInput.KEY_I = 73
flowInput.KEY_J = 74
flowInput.KEY_K = 75
flowInput.KEY_L = 76
flowInput.KEY_M = 77
flowInput.KEY_N = 78
flowInput.KEY_O = 79
flowInput.KEY_P = 80
flowInput.KEY_Q = 81
flowInput.KEY_R = 82
flowInput.KEY_S = 83
flowInput.KEY_T = 84
flowInput.KEY_U = 85
flowInput.KEY_V = 86
flowInput.KEY_W = 87
flowInput.KEY_X = 88
flowInput.KEY_Y = 89
flowInput.KEY_Z = 90
flowInput.KEY_LEFT_BRACKET = 91
flowInput.KEY_BACKSLASH = 92
flowInput.KEY_RIGHT_BRACKET = 93
flowInput.KEY_GRAVE_ACCENT = 96
flowInput.KEY_WORLD_1 = 161
flowInput.KEY_WORLD_2 = 162

-- Functional keys
flowInput.KEY_ESCAPE = 256
flowInput.KEY_ENTER = 257
flowInput.KEY_TAB = 258
flowInput.KEY_BACKSPACE = 259
flowInput.KEY_INSERT = 260
flowInput.KEY_DELETE = 261
flowInput.KEY_RIGHT = 262
flowInput.KEY_LEFT = 263
flowInput.KEY_DOWN = 264
flowInput.KEY_UP = 265
flowInput.KEY_PAGE_UP = 266
flowInput.KEY_PAGE_DOWN = 267
flowInput.KEY_HOME = 268
flowInput.KEY_END = 269
flowInput.KEY_CAPS_LOCK = 280
flowInput.KEY_SCROLL_LOCK = 281
flowInput.KEY_NUM_LOCK = 282
flowInput.KEY_PRINT_SCREEN = 283
flowInput.KEY_PAUSE = 284
flowInput.KEY_F1 = 290
flowInput.KEY_F2 = 291
flowInput.KEY_F3 = 292
flowInput.KEY_F4 = 293
flowInput.KEY_F5 = 294
flowInput.KEY_F6 = 295
flowInput.KEY_F7 = 296
flowInput.KEY_F8 = 297
flowInput.KEY_F9 = 298
flowInput.KEY_F10 = 299
flowInput.KEY_F11 = 300
flowInput.KEY_F12 = 301
flowInput.KEY_F13 = 302
flowInput.KEY_F14 = 303
flowInput.KEY_F15 = 304
flowInput.KEY_F16 = 305
flowInput.KEY_F17 = 306
flowInput.KEY_F18 = 307
flowInput.KEY_F19 = 308
flowInput.KEY_F20 = 309
flowInput.KEY_F21 = 310
flowInput.KEY_F22 = 311
flowInput.KEY_F23 = 312
flowInput.KEY_F24 = 313
flowInput.KEY_F25 = 314
flowInput.KEY_KP_0 = 320
flowInput.KEY_KP_1 = 321
flowInput.KEY_KP_2 = 322
flowInput.KEY_KP_3 = 323
flowInput.KEY_KP_4 = 324
flowInput.KEY_KP_5 = 325
flowInput.KEY_KP_6 = 326
flowInput.KEY_KP_7 = 327
flowInput.KEY_KP_8 = 328
flowInput.KEY_KP_9 = 329
flowInput.KEY_KP_DECIMAL = 330
flowInput.KEY_KP_DIVIDE = 331
flowInput.KEY_KP_MULTIPLY = 332
flowInput.KEY_KP_SUBTRACT = 333
flowInput.KEY_KP_ADD = 334
flowInput.KEY_KP_ENTER = 335
flowInput.KEY_KP_EQUAL = 336
flowInput.KEY_LEFT_SHIFT = 340
flowInput.KEY_LEFT_CONTROL = 341
flowInput.KEY_LEFT_ALT = 342
flowInput.KEY_LEFT_SUPER = 343
flowInput.KEY_RIGHT_SHIFT = 344
flowInput.KEY_RIGHT_CONTROL = 345
flowInput.KEY_RIGHT_ALT = 346
flowInput.KEY_RIGHT_SUPER = 347
flowInput.KEY_MENU = 348

return flowInput
