#pragma once

#pragma warning(disable : 4244 4305) // double <-> float conversions
#pragma warning(disable : 28159) // GetTickCount64

#include "natives.h"
#include "keyboard.h"

int fnt = 0;

enum MenuNavigation { NAV_Back, NAV_Select, NAV_Scroll, NAV_Toggle, NAV_Scroll_L, NAV_Scroll_R, NAV_None };

struct MenuItem {
	LPCSTR text;
	bool togglable;
	bool scrollable;
	bool *tState;
	float *sState;
	float x;
	float y;
	float z;
	bool pUpdated;
};

struct Menu {
	std::string caption;
	std::vector<MenuItem> lines;
	void (*process_menu_function)(std::string& caption, std::vector<MenuItem>& lines, int* active_index, MenuNavigation& navigation);
	float lineWidth = 300.0;
	float lineHeight = 20.0;
	float lineSpacing = 58.0;
	float spaceFromTitle = 68.0;
	int active_index = 0;
};

void draw_rect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7) {
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}

void draw_menu_line(std::string caption, std::string state, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active,
    bool title, bool rescaleText = true) {
	int text_col[4] = { 255, 255, 255, 255 }, rect_col[4] = { 70, 95, 95, 255 };
	float text_scale = 0.35;
	int font = 0;
	float fontHeight = 0.015;

	if (active) {
		text_col[0] = 0;
		text_col[1] = 0;
		text_col[2] = 0;

		rect_col[0] = 218;
		rect_col[1] = 242;
		rect_col[2] = 216;

		if (rescaleText)
			text_scale = 0.37;
	}

	if (title) {
		rect_col[0] = 0;
		rect_col[1] = 0;
		rect_col[2] = 0;

		if (rescaleText)
			text_scale = 0.50;
		font = 1;

		fontHeight = 0.02;
	}

	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	textLeft += lineLeft;

	float lineWidthScaled = lineWidth / (float)screen_w; // line width
	float lineTopScaled = lineTop / (float)screen_h; // line top offset
	float textLeftScaled = textLeft / (float)screen_w; // text left offset
	float lineHeightScaled = lineHeight / (float)screen_h; // line height

	float lineLeftScaled = lineLeft / (float)screen_w;

	HUD::SET_TEXT_FONT(font);
	HUD::SET_TEXT_SCALE(.25 * (text_scale * 2), .6 * (text_scale * 2));
	HUD::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	HUD::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	//HUD::SET_TEXT_WRAP(0, 0.3100000);
	//HUD::SET_TEXT_RIGHT_JUSTIFY(0);
	//HUD::SET_TEXT_JUSTIFY(1);
	HUD::DISPLAY_TEXT_WITH_LITERAL_STRING(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f), "STRING", (LPSTR)caption.c_str());

	HUD::SET_TEXT_FONT(fnt);
	HUD::SET_TEXT_SCALE(.25 * (text_scale * 2), .6 * (text_scale * 2));
	HUD::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	HUD::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	if (state != "")
		HUD::DISPLAY_TEXT_WITH_LITERAL_STRING(lineWidthScaled - (float)state.length() / 200, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f), "STRING", (LPSTR)state.c_str());
	
	// rect
	draw_rect(lineLeftScaled, lineTopScaled, lineWidthScaled,
	    (((fontHeight + lineHeightScaled * 2.0f)) + 0.005f), rect_col[0], rect_col[1], rect_col[2], rect_col[3]);
}

void get_button_state(bool* a, bool* b, bool* up, bool* down, bool* l, bool* r) {
	if (a)
		*a = IsKeyJustDown(VK_NUMPAD5) || IsKeyJustDown(VK_RETURN);
	if (b)
		*b = IsKeyJustDown(VK_NUMPAD0) || IsKeyJustDown(VK_BACK);
	if (up)
		*up = IsKeyJustDown(VK_NUMPAD8) || IsKeyJustDown(VK_UP);
	if (down)
		*down = IsKeyJustDown(VK_NUMPAD2) || IsKeyJustDown(VK_DOWN);
	if (r)
		*r = IsKeyRepeating(VK_NUMPAD6) || IsKeyRepeating(VK_RIGHT);
	if (l)
		*l = IsKeyRepeating(VK_NUMPAD4) || IsKeyRepeating(VK_LEFT);
}

void menu_beep(MenuNavigation nav) {
	switch (nav) {
	case NAV_Back:
		AUDIO::PLAY_SOUND_FRONTEND(-1, "FRONTEND_MENU_BACK");
		break;
	case NAV_Scroll:
		AUDIO::PLAY_SOUND_FRONTEND(-1, "FRONTEND_MENU_HIGHLIGHT_DOWN_UP");
		break;
	case NAV_Select:
		AUDIO::PLAY_SOUND_FRONTEND(-1, "FRONTEND_MENU_SELECT");
		break;
	case NAV_Toggle:
		AUDIO::PLAY_SOUND_FRONTEND(-1, "FRONTEND_MENU_TOGGLE_ON");
		break;
	}
}

std::string statusText;
DWORD statusTextDrawTicksMax;

void update_status_text() {
	if (GetTickCount() < statusTextDrawTicksMax) {
		HUD::SET_TEXT_FONT(0);
		HUD::SET_TEXT_SCALE(0.4, 0.4);
		HUD::SET_TEXT_COLOUR(255, 255, 255, 255);
		HUD::SET_TEXT_WRAP(0.0, 1.0);
		HUD::SET_TEXT_CENTRE(1);
		HUD::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		HUD::SET_TEXT_EDGE(1, 0, 0, 0, 205);

		HUD::DISPLAY_TEXT_WITH_LITERAL_STRING(.5, .7, "STRING", (char*)statusText.c_str());
	}
}

void set_status_text(std::string str, DWORD time = 2500, bool isGxtEntry = false) {
	statusText = str;
	statusTextDrawTicksMax = GetTickCount() + time;
}

std::string state_as_str(bool* tState, bool togglable, float* sState, bool scrollable, bool selected = false) {
	char str[16];
	if (scrollable && sState)
		sprintf_s(str, selected ? "< %.2f >" : "%.2f", *sState);

	return (togglable ? ((tState && *tState) ? "[ON]" : "[OFF]") : (sState && scrollable) ? str : "");
}

void draw_menu(std::string caption, MenuItem lines[], int lineCount, int active_index) {
	const float lineWidth = 400.0;
	const float lineHeight = 20.0;
	const float lineSpacing = 58.0;
	const float spaceFromTitle = 68.0;
	const float lineLeft = 18.0;

	// draw menu
	draw_menu_line(caption, "", lineWidth, 15.0, 18.0, lineLeft, 5.0, false, true);
	for (int i = 0; i < lineCount; i++)
		draw_menu_line(lines[i].text, state_as_str(lines[i].tState, lines[i].togglable, lines[i].sState, lines[i].scrollable, i == active_index),
		    lineWidth,
			    lineHeight, spaceFromTitle + i * lineSpacing, lineLeft, 9.0, i == active_index, false);
}
