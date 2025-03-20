
#include "z80input.h"
#include "usbh_hid.h"

uint8_t indata[128];

const uint8_t keyaddr[ZX_KEY_LAST] = {
	0xFE, 0xFE, 0xFE, 0xFE, 0xFE, // ZX_KEY_SHIFT, ZX_KEY_Z,   ZX_KEY_X, ZX_KEY_C, ZX_KEY_V
	0xFD, 0xFD, 0xFD, 0xFD, 0xFD, // ZX_KEY_A,     ZX_KEY_S,   ZX_KEY_D, ZX_KEY_F, ZX_KEY_G
	0xFB, 0xFB, 0xFB, 0xFB, 0xFB, // ZX_KEY_Q,     ZX_KEY_W,   ZX_KEY_E, ZX_KEY_R, ZX_KEY_T
	0xF7, 0xF7, 0xF7, 0xF7, 0xF7, // ZX_KEY_1,     ZX_KEY_2,   ZX_KEY_3, ZX_KEY_4, ZX_KEY_5
	0xEF, 0xEF, 0xEF, 0xEF, 0xEF, // ZX_KEY_0,     ZX_KEY_9,   ZX_KEY_8, ZX_KEY_7, ZX_KEY_6
	0xDF, 0xDF, 0xDF, 0xDF, 0xDF, // ZX_KEY_P,     ZX_KEY_O,   ZX_KEY_I, ZX_KEY_U, ZX_KEY_Y
	0xBF, 0xBF, 0xBF, 0xBF, 0xBF, // ZX_KEY_ENTER, ZX_KEY_L,   ZX_KEY_K, ZX_KEY_J, ZX_KEY_H
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, // ZX_KEY_SPACE, ZX_KEY_SYM, ZX_KEY_M, ZX_KEY_N, ZX_KEY_B
};

const uint8_t keybuf[ZX_KEY_LAST] = {
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_SHIFT, ZX_KEY_Z,   ZX_KEY_X, ZX_KEY_C, ZX_KEY_V
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_A,     ZX_KEY_S,   ZX_KEY_D, ZX_KEY_F, ZX_KEY_G
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_Q,     ZX_KEY_W,   ZX_KEY_E, ZX_KEY_R, ZX_KEY_T
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_1,     ZX_KEY_2,   ZX_KEY_3, ZX_KEY_4, ZX_KEY_5
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_0,     ZX_KEY_9,   ZX_KEY_8, ZX_KEY_7, ZX_KEY_6
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_P,     ZX_KEY_O,   ZX_KEY_I, ZX_KEY_U, ZX_KEY_Y
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_ENTER, ZX_KEY_L,   ZX_KEY_K, ZX_KEY_J, ZX_KEY_H
	0x01, 0x02, 0x04, 0x08, 0x10, // ZX_KEY_SPACE, ZX_KEY_SYM, ZX_KEY_M, ZX_KEY_N, ZX_KEY_B
};

#define ON_KEY(k) indata[keyaddr[k] - 0x7F] |= keybuf[k];

bool OnKey(uint8_t scanCode)
{
	switch (scanCode)
	{
	case KEY_LEFTSHIFT:
	case KEY_RIGHTSHIFT:
		ON_KEY(ZX_KEY_SHIFT);
		break;
	case KEY_LEFTCONTROL:
	case KEY_RIGHTCONTROL:
		ON_KEY(ZX_KEY_SYM);
		break;
	case KEY_ENTER:
    case KEY_KEYPAD_ENTER:
		ON_KEY(ZX_KEY_ENTER);
		break;

	case KEY_SPACEBAR:
		ON_KEY(ZX_KEY_SPACE);
		break;
	case KEY_0_CPARENTHESIS:
		ON_KEY(ZX_KEY_0);
		break;
	case KEY_1_EXCLAMATION_MARK:
		ON_KEY(ZX_KEY_1);
		break;
	case KEY_2_AT:
		ON_KEY(ZX_KEY_2);
		break;
	case KEY_3_NUMBER_SIGN:
		ON_KEY(ZX_KEY_3);
		break;
	case KEY_4_DOLLAR:
		ON_KEY(ZX_KEY_4);
		break;
	case KEY_5_PERCENT:
		ON_KEY(ZX_KEY_5);
		break;
	case KEY_6_CARET:
		ON_KEY(ZX_KEY_6);
		break;
	case KEY_7_AMPERSAND:
		ON_KEY(ZX_KEY_7);
		break;
	case KEY_8_ASTERISK:
		ON_KEY(ZX_KEY_8);
		break;
	case KEY_9_OPARENTHESIS:
		ON_KEY(ZX_KEY_9);
		break;

	case KEY_A:
		ON_KEY(ZX_KEY_A);
		break;
	case KEY_B:
		ON_KEY(ZX_KEY_B);
		break;
	case KEY_C:
		ON_KEY(ZX_KEY_C);
		break;
	case KEY_D:
		ON_KEY(ZX_KEY_D);
		break;
	case KEY_E:
		ON_KEY(ZX_KEY_E);
		break;
	case KEY_F:
		ON_KEY(ZX_KEY_F);
		break;
	case KEY_G:
		ON_KEY(ZX_KEY_G);
		break;
	case KEY_H:
		ON_KEY(ZX_KEY_H);
		break;
	case KEY_I:
		ON_KEY(ZX_KEY_I);
		break;
	case KEY_J:
		ON_KEY(ZX_KEY_J);
		break;
	case KEY_K:
		ON_KEY(ZX_KEY_K);
		break;
	case KEY_L:
		ON_KEY(ZX_KEY_L);
		break;
	case KEY_M:
		ON_KEY(ZX_KEY_M);
		break;
	case KEY_N:
		ON_KEY(ZX_KEY_N);
		break;
	case KEY_O:
		ON_KEY(ZX_KEY_O);
		break;
	case KEY_P:
		ON_KEY(ZX_KEY_P);
		break;
	case KEY_Q:
		ON_KEY(ZX_KEY_Q);
		break;
	case KEY_R:
		ON_KEY(ZX_KEY_R);
		break;
	case KEY_S:
		ON_KEY(ZX_KEY_S);
		break;
	case KEY_T:
		ON_KEY(ZX_KEY_T);
		break;
	case KEY_U:
		ON_KEY(ZX_KEY_U);
		break;
	case KEY_V:
		ON_KEY(ZX_KEY_V);
		break;
	case KEY_W:
		ON_KEY(ZX_KEY_W);
		break;
	case KEY_X:
		ON_KEY(ZX_KEY_X);
		break;
	case KEY_Y:
		ON_KEY(ZX_KEY_Y);
		break;
	case KEY_Z:
		ON_KEY(ZX_KEY_Z);
		break;

    // "Convenience" buttons

    case KEY_MINUS_UNDERSCORE:
    case KEY_KEYPAD_MINUS:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_J);
		break;
    case KEY_EQUAL_PLUS:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_L);
		break;
    case KEY_COMMA_AND_LESS:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_N);
		break;
    case KEY_DOT_GREATER:
    case KEY_KEYPAD_DECIMAL_SEPARATOR_DELETE:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_M);
		break;
    case KEY_SLASH_QUESTION:
    case KEY_KEYPAD_SLASH:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_V);
		break;
    case KEY_SEMICOLON_COLON:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_O);
		break;
    case KEY_KEYPAD_ASTERIKS:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_B);
		break;
    case KEY_KEYPAD_PLUS:
		ON_KEY(ZX_KEY_SYM);
		ON_KEY(ZX_KEY_K);
		break;
	case KEY_BACKSPACE:
		ON_KEY(ZX_KEY_SHIFT);
		ON_KEY(ZX_KEY_0);
		break;
    case KEY_LEFTARROW:
		ON_KEY(ZX_KEY_SHIFT);
		ON_KEY(ZX_KEY_5);
		break;
    case KEY_RIGHTARROW:
		ON_KEY(ZX_KEY_SHIFT);
		ON_KEY(ZX_KEY_8);
		break;
    case KEY_UPARROW:
		ON_KEY(ZX_KEY_SHIFT);
		ON_KEY(ZX_KEY_7);
		break;
    case KEY_DOWNARROW:
		ON_KEY(ZX_KEY_SHIFT);
		ON_KEY(ZX_KEY_6);
		break;
    default:
        return false;
	}

    return true;
}


extern "C" void USBH_HID_EventCallback(USBH_HandleTypeDef* phost)
{
	if (USBH_HID_GetDeviceType(phost) == HID_KEYBOARD)
	{
		memset(indata, 0, sizeof(indata));

		HID_KEYBD_Info_TypeDef* keyboardInfo = USBH_HID_GetKeybdInfo(phost);

		for (uint8_t i = 0; i < 6; i++)
		{
			uint8_t key = keyboardInfo->keys[i];
			if (key != 0)
			{
				OnKey(key);
			}
		}
	}
}
