#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

constexpr glm::ivec2 MIN_SCREEN = glm::ivec2(50, 30); 

void drawBox(glm::ivec2 tl, glm::ivec2 br, std::string title = "");

enum Align {
	LEFT,
	CENTER
};

struct Window {
	std::string title = "";
	
	glm::ivec2 position = glm::vec2(0); // defined by top left
	
	glm::ivec2 padding  = glm::vec2(0);
	glm::ivec2 adjustedPadding = glm::vec2(0);

	glm::ivec2 dimensions = glm::vec2(0);
	glm::ivec2 minDimensions = glm::vec2(0);

	std::vector<std::string> text;

	Align alignment = Align::LEFT;

	bool active = true;

	bool operator==(const Window& other) const {
		return (title == other.title &&
				position == other.position &&
				padding == other.padding && // adjusted padding must be the same if this is true
				minDimensions == other.minDimensions &&
				text == other.text && 
				alignment == other.alignment); // dimensions must be the same if this and padding check is true
	}
};

using windowID = uint32_t;

class WindowManager {
	public:
		// window stuff
		windowID addWindow(Window window);
		void deleteWindow(windowID id);

		void clearWindow(windowID id);
		void uptadeWindow(windowID id, Window window);
		void updateText(windowID id, std::vector<std::string> text);
		void renderAll();

		// screen stuff
		bool screenSizeCheck();

		glm::ivec2 curr = glm::ivec2(0);
		glm::ivec2 prev = glm::ivec2(0);
	private:
		std::unordered_map<windowID, Window> windows;
		windowID currentID = 0;
		
		bool prevSmall = false;

		bool checkID(windowID id);

		void renderWindow(windowID id);
};

