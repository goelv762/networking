#include <string>
#include <vector>

#include <glm/glm.hpp>

enum command {
	// misc.
	QUIT
};

class CommandLine {
	public:
		std::string commandString = "";
		bool send = false;

		void update();
		void render(glm::vec2 dim);
		void recvMsg(std::string msg);

	private:
		std::string commandPrompt = "-> ";
		int promptLength = commandPrompt.length();

		std::vector<std::string> msgLog = {};
};
