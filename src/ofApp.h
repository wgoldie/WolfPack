#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
	private:
		static const int ROT_SPEED = 3;

		ofTexture tex;
		ofPixels drawBuffer;

		ofPixels wallTextures[3];

		typedef struct Renderer {
			vector<float> heightField;
			vector<char> wallTypes;
			int width;
			int height;

			Renderer(int windowWidth, int windowHeight) {
				width = windowWidth;
				height = windowHeight;
			}

			Renderer() {
				Renderer(0, 0);
			}
		};

		typedef struct Transform {
			ofVec2f pos, rot, cam;
			void rotate(int degrees) {
				rot.rotate(degrees);
				cam.rotate(degrees);

			}
		};

		Transform transform;
		Renderer renderer;
		bool isKeyDown[255];
};
