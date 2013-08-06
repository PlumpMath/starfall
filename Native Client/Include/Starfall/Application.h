//Copyright (c) 2013 Mark Farrell
#pragma once

#include <string>
#include <algorithm>
#include <cmath>
#include <Windows.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Utf.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/DataSource.h>
#include <Awesomium/DataPak.h>
#include <Awesomium/WebViewListener.h>
#include <algorithm>
#include <fstream>

#include "Starfall\Skybox.h"

using std::string;


namespace Starfall {

	class Application : Awesomium::JSMethodHandler {
		public:

			sf::TcpSocket socket;

			Awesomium::WebCore* core;
			Awesomium::WebView* view;
			Awesomium::DataSource* dataSource;

			sf::Clock uiClock;
			float maxFps;
			float uiResetTime;

			sf::Clock clock;
			Skybox skybox;

			sf::RenderWindow window;

			sf::Image surfaceImage;
			sf::Texture surfaceTexture;
			sf::Sprite surfaceSprite;

			Awesomium::JSObject loginControlsObject;

			int mapKey(sf::Keyboard::Key &sfmlCode);
			void keyEvent(Awesomium::WebKeyboardEvent::Type type, sf::Event& event);

			void update();
			void render();
			Application();
			~Application();
			void run();

			virtual void OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args);
			virtual Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args);
	};
}
