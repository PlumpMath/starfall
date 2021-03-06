//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).
#pragma once
#include "Starfall/Platform.h"
#include "Starfall/Assets.h"

using namespace Starfall;

std::ofstream Platform::out;

void Platform::Init() {
	Platform::out.open("client.log");
	cout.rdbuf(out.rdbuf());
#ifdef PLATFORM_IS_WINDOWS
	cout << "[Platform::Init] Starting on Windows." << endl;
#endif
}

void Platform::Halt(int code) {

	Assets::Loader.stopAll(); //Stop all threads currently running in the thread pool

	switch(code) {
		case 0: 
			cout << "[Platform::Halt] The Application has finished." << endl;
		break;
		case 1: 
			cout << "[Platform::Halt] The Application has crashed.\n\nPlease contact: markfarr2011@gmail.com" << endl;
		break;
	}

	if(Platform::out.is_open()) {
		Platform::out.close();
	}

	exit(code);
}