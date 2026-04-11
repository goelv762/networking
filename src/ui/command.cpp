#include "command.hpp"
#include "helper.hpp"

#include <ncurses.h>
#include <bits/stdc++.h>

void CommandLine::update() {
	if (send) {
		send = false;
		commandString = "";
	}

	int currChar = wgetch(stdscr);
	if (checkChar(currChar)) {
		commandString += currChar;
	}
	
	// backspace
	else if (currChar == KEY_BACKSPACE && commandString.length() >= 1) {
		commandString.pop_back();
	}

	// enter
	else if (currChar == 10 && commandString.length() >= 1) {
		msgLog.push_back("[you]: " + commandString);
		send = true;
	}
}

void CommandLine::render(glm::vec2 dim) {

	// clear line first
	std::string clearString(dim.x, ' ');
	// render messages
	int total = 0;
	for (int i = 0; i < msgLog.size(); i++) {
		mvprintw(i + 1, 0, "%s", msgLog[i].c_str());
		total += msgLog[i].size();
	}
	mvprintw(0, 0, "%d", total);
	mvprintw(dim.y, 0, "%s", clearString.c_str());
	mvprintw(dim.y, 0, "%s%s    ", commandPrompt.c_str(), commandString.c_str());
	attron(COLOR_PAIR(1));
	mvaddch(dim.y, commandString.length() + promptLength, '.');
	attroff(COLOR_PAIR(1));
}

void CommandLine::recvMsg(std::string msg) {
	msgLog.push_back(msg);
}
