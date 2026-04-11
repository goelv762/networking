#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <ncurses.h>

#include "prot/tcp.hpp"
#include "prot/udp.hpp"

#include "ui/command.hpp"
#include "ui/ui.hpp"

void sendMsg(Client *c, CommandLine *cl) {
	WindowManager wm;
	
	MEVENT event;
    char ch;

	initscr();
	start_color();
	use_default_colors();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	curs_set(0);
	
	init_pair(1, COLOR_BLACK, COLOR_BLACK);
	
	bool exit = false;

	while (!exit) {
		if (wm.screenSizeCheck()) { continue; }

		// move(0, 0);
		// clrtoeol();

		cl->update();
		cl->render(wm.curr - glm::ivec2(0, 1));
		
		if (cl->send) {
			if (cl->commandString != "quit") {
				c->sendMessage(cl->commandString);
			} else {
				exit = true;
			}
		} 		
        refresh();
    }

	// TODO: make it so that the client stops listening for messages aswell
    endwin();
}

void receive(Client* c, CommandLine *cl) {
	while (true) {
		std::string buffer;
		c->recvMessage(buffer);
		
		cl->recvMsg(buffer);
	}
}

int main (int argc, const char * argv[]) {
	Broadcast bc;
	bc.cast();
	ServerInfo info = bc.recive();

	Client c(info.ip, info.port);
	
	CommandLine cl;

	std::thread s(sendMsg, &c, &cl);
	std::thread r(receive, &c, &cl);
	
	s.join();
	r.join();
	 
    return 0;
}
