#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include <iostream>
#include <ncurses.h>
#include <stdexcept>
#include <string>

#include "ui.hpp"

void drawBox(glm::ivec2 tl, glm::ivec2 br, std::string title) {
	int hDist = br.x - tl.x;
	int vDist = br.y - tl.y;

	// corners
	mvaddch(tl.y, tl.x, ACS_ULCORNER); 
	mvaddch(tl.y, br.x, ACS_URCORNER);
	mvaddch(br.y, tl.x, ACS_LLCORNER); 
	mvaddch(br.y, br.x, ACS_LRCORNER);
	
	// horizontal lines
	for (size_t i = 1; i < hDist; i++) {
		int xPos = tl.x + i;
		mvaddch(tl.y, xPos, ACS_HLINE); 
		mvaddch(br.y, xPos, ACS_HLINE); 
	}
	
	// title
	int len = title.length();
	if (title != "") {
		// +2 for space on either side of title
		// -4 to ensure L & R TEE chars do not overwrite corners
		if ((len + 2) < (hDist - 4)) {
			// make sure title is centered
			// +1 for space (left)
			int leftMargin = (hDist - len) / 2;
			for (size_t j = 0; j < len; j++) {
				mvaddch(tl.y, tl.x + leftMargin + j, title[j]);
			}

			// adding spaces on either side of title
			mvaddch(tl.y, tl.x + leftMargin - 1, ' ');
			mvaddch(tl.y, tl.x + leftMargin + len, ' ');

			// adding special characters on either side of title
			mvaddch(tl.y, tl.x + leftMargin - 2, ACS_RTEE);
			mvaddch(tl.y, tl.x + leftMargin + len + 1, ACS_LTEE);
		}

	}

	// vertical lines
	for (size_t k = 1; k < vDist; k++) {
		int yPos = tl.y + k;
		mvaddch(yPos, tl.x, ACS_VLINE);
		mvaddch(yPos, br.x, ACS_VLINE);
	}
}


// true if the given window ID exists within the windows map
bool WindowManager::checkID(windowID id) {
	return (windows.find(id) != windows.end());
}

windowID WindowManager::addWindow(Window window) {
	// default initialisation to keep update window from freaking out
	windows[currentID] = Window();
	uptadeWindow(currentID, window);
	currentID++;
	
	return currentID - 1;
}

void WindowManager::deleteWindow(windowID id) {
	if (checkID(id)) {
		throw std::runtime_error("no window exists with id " + std::to_string(id));
	}

	// otherwise, id must exist
	windows.erase(id);
}

void WindowManager::uptadeWindow(windowID id, Window window) {
	if (!checkID(id)) {
		throw std::runtime_error("no window exists with id " + std::to_string(id));
	}

	// to account for rectangle shape of single char
	window.adjustedPadding = {window.padding.x * 2, window.padding.y};

	// find longest line
	uint32_t longest = 0;
	for (auto line : window.text) {
		if (line.length() > longest) {
			longest = line.length();
		}
	}

	// x -> widest bit of text
	// y -> amount of lines
	window.dimensions =  {longest, window.text.size()};
	// allow space for side characters
	window.dimensions = window.dimensions + glm::ivec2(1);
	// times by 2 for both sides of text box requiring padding
	window.dimensions = window.dimensions + window.adjustedPadding * 2;
	
	window.dimensions = glm::max(window.dimensions, window.minDimensions);

	windows[id] = window;
}

void WindowManager::updateText(windowID id, std::vector<std::string> text) {
	if (!checkID(id)) {
		throw std::runtime_error("no window exists with id " + std::to_string(id));
	}

	Window window = windows[id];
	window.text = text;
	
	uptadeWindow(id, window);
}

void WindowManager::clearWindow(windowID id) {
	if (!checkID(id)) {
		throw std::runtime_error("no window exists with id " + std::to_string(id));
	}
	Window& window = windows[id];
	
	// +1 to ensure last part is also cleared
	for (size_t i = 0; i < window.dimensions.x + 1; i++) {
		for (size_t j = 0; j < window.dimensions.y + 1; j++) {
			mvaddch(window.position.y + j, window.position.x + i, ' ');
		}
	}
}

void WindowManager::renderWindow(windowID id) {
	if (!checkID(id)) {
		throw std::runtime_error("no window exists with id " + std::to_string(id));
	}

	Window& window = windows[id];

	if (!window.active) { return; }
	
	// probably bad b/c it doesn't need to be clear every time...
	clearWindow(id);

	// find longest line
	uint32_t longest = 0;
	for (auto line : window.text) {
		if (line.length() > longest) {
			longest = line.length();
		}
	}
	
	drawBox(window.position, window.position + window.dimensions, window.title);

	if (window.alignment == Align::LEFT) {
		for (size_t i = 0; i < window.text.size(); i++) {
			mvprintw(window.position.y + window.adjustedPadding.y + i + 1, 
					 window.position.x + window.adjustedPadding.x + 1, 
					 "%s", window.text[i].c_str());
		}
	}
	

	else if (window.alignment == Align::CENTER) {
		for (size_t i = 0; i < window.text.size(); i++) {
			int leftPadding = (longest - window.text[i].length()) / 2;

			mvprintw(window.position.y + window.adjustedPadding.y + i + 1, 
					 window.position.x + window.adjustedPadding.x + leftPadding + 1, 
					 "%s", window.text[i].c_str());
		}
	}
}

void WindowManager::renderAll() {
	for (auto& [id, window] : windows) {
		renderWindow(id);
	}
}


bool WindowManager::screenSizeCheck() {
	// false -> screen size is suff
	// true -> needs to be bigger
	
	prev = curr;
	getmaxyx(stdscr, curr.y, curr.x);
	if (prev != curr) {
		clear();
		return true;
	}

	if (curr.x < MIN_SCREEN.x || curr.y < MIN_SCREEN.y) {
		if (!prevSmall) {
			clear();
		}

		prevSmall = true;
		mvprintw(0, 0, "current  screen dimensions are [%d, %d]", curr.x, curr.y);
		mvprintw(1, 0, "required screen dimensions are at least [%d, %d]", MIN_SCREEN.x, MIN_SCREEN.y);

		refresh();
		return true;
	}

	else {
		prevSmall = false;
		return false;
	}
}
