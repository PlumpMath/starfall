//Copyright (c) 2013 Mark Farrell

#include "Starfall\Application.h"
#include "Starfall\Platform.h"
#include "Starfall\Login.h"

#include <Poco\NumberParser.h>
#include <Poco\StringTokenizer.h>

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>

using std::string;

using std::cout;
using std::endl;

using namespace Starfall;

Application::Application() : 
	config(ConfigurationFile::Client()),

	window(sf::VideoMode(config.getInt("window.width"), config.getInt("window.height")), config.getString("window.title", "Fail"), sf::Style::Default, sf::ContextSettings(32)) // Create the main window
{

	unsigned short numFiles;
	Awesomium::WriteDataPak(Awesomium::WSLit(Assets::Path("login.pak").c_str()), Awesomium::WSLit(Assets::Path("login").c_str()), Awesomium::WSLit(""), numFiles);

	this->loginScene = new LoginScene(this);
}


Application::~Application() {
	delete this->loginScene;
	Awesomium::WebCore::Shutdown();
}

void Application::update() {
	this->loginScene->update();
}

void Application::render() {
	this->loginScene->render();
	// Finally, display the rendered frame on screen
	window.display();
}

bool Application::checkRequirements() {
	bool hasRequirements = true;
	if(GLEW_OK != glewInit()) {
		cout << "[Application::run] Glew failed to initialized." << endl;
		hasRequirements = false;
	}

	std::ostringstream versionStream;
	versionStream << glGetString(GL_VERSION);
	Poco::StringTokenizer tokenizer(versionStream.str(), ".", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
	if(tokenizer.count() >= 2) {
		int majorVersion;
		int minorVersion;
		if(Poco::NumberParser::tryParse(tokenizer[0], majorVersion) &&
			Poco::NumberParser::tryParse(tokenizer[1], minorVersion)) {
				std::ostringstream versionErrorStream;
				versionErrorStream << "[Application::run] " << "This application requires OpenGL " << this->config.getInt("gl.version.major") << "." << this->config.getInt("gl.version.minor") << "; Version " << majorVersion << "." << minorVersion << " is not supported." << endl;		
				if(majorVersion >= this->config.getInt("gl.version.major")) {
					if(minorVersion < this->config.getInt("gl.version.minor")) {
						cout << versionErrorStream.str();
						hasRequirements = false;
					}
				} else {
					cout << versionErrorStream.str();
					hasRequirements = false;
				}
		} else {
			hasRequirements = false;
		}
	} else {
		hasRequirements = false;
	}
	
	return hasRequirements;
}

void Application::run() {

	sf::Image icon; 
	if(!icon.loadFromFile(Assets::Path("icon.png"))) {
		cout << "Failed to load icon." << endl;
		Platform::Halt();
	}

	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setActive();  // Make it the active window for OpenGL calls

	if(!this->checkRequirements()) {
		Platform::Halt();
	}

	this->loginScene->load();
	this->loginScene->enter();
 
    while (window.isOpen())
    {
		this->update();
		this->render();
    }
}

void Application::logout() {
	this->loginScene->enter();
}