#include "ofApp.h"
static const float MOVE_SPEED_DIVISOR = 0.2;
static const int TEX_SIZE = 128;
static const int MINIMAP_SIZE = 100;
static const int LEVEL_SIZE = 10;
static const unsigned char LEVEL[LEVEL_SIZE][LEVEL_SIZE] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 2 },
	{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 2 },
	{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 2 },
	{ 1, 0, 1, 0, 1, 0, 0, 0, 0, 2 },
	{ 1, 0, 1, 1, 0, 0, 0, 0, 0, 2 },
	{ 1, 0, 1, 1, 0, 0, 1, 0, 0, 2 },
	{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 2 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 2 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 }
};

static const ofColor WALL_TYPES[] = { ofColor(255), ofColor(255, 0, 0), ofColor(0, 255, 0), ofColor(0, 0, 255) };

//--------------------------------------------------------------
void ofApp::setup(){
	transform.pos = ofVec2f(1.5, 1.5);
	transform.rot = ofVec2f(0, MOVE_SPEED_DIVISOR);
	transform.cam = transform.rot.getPerpendicular();
	renderer = Renderer(ofGetWindowWidth(), ofGetWindowHeight());
	ofPixels checkerboard;
	ofPixels pinstripe;
	checkerboard.allocate(TEX_SIZE, TEX_SIZE, OF_IMAGE_GRAYSCALE);
	pinstripe.allocate(TEX_SIZE, TEX_SIZE, OF_IMAGE_GRAYSCALE);
	for (int x = 0; x < TEX_SIZE; x++) {
		for (int y = 0; y < TEX_SIZE; y++) {
			int idx = x * TEX_SIZE + y;
			checkerboard[idx] = ((x / 32) % 2 == 0) != ((y / 32) % 2 == 0) ? 255 : 0;
			pinstripe[idx] = x / 2 % 2 == 0 ? 255 : 0;
		}
	}
	wallTextures[1] = checkerboard;
	wallTextures[2] = pinstripe;
}

//--------------------------------------------------------------
void ofApp::update(){
	if (ofGetKeyPressed('w')) {
		ofVec2f test = transform.pos + transform.rot;
		if (LEVEL[(int)floor(test.x)][(int)floor(transform.pos.y)] == 0) {
			transform.pos.x += transform.rot.x;
		}
		if (LEVEL[(int)floor(transform.pos.x)][(int)floor(test.y)] == 0) {
			transform.pos.y += transform.rot.y;
		}
	}
	if (ofGetKeyPressed('s')) {
		ofVec2f test = transform.pos - transform.rot / 5;
		if (LEVEL[(int)floor(test.x)][(int)floor(transform.pos.y)] == 0) {
			transform.pos.x -= transform.rot.x;
		}
		if (LEVEL[(int)floor(transform.pos.x)][(int)floor(test.y)] == 0) {
			transform.pos.y -= transform.rot.y;
		}
	}
	if (ofGetKeyPressed('a')) {
		transform.rotate(-ROT_SPEED);
	}
	if (ofGetKeyPressed('d')) {
		transform.rotate(ROT_SPEED);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetBackgroundColor(255, 0, 0);
	ofSetColor(255);
	// raycast
	string crosshairDist;

	// draw walls
	ofPixels pix;
	pix.allocate(renderer.width, renderer.height, 1);

	for(int i = 0; i < renderer.width; i++) {
		// x coord on transform.camera proj plane
		float camX = 2.0 * i / renderer.width - 1;
		ofVec2f rayDir = transform.rot.getNormalized() + transform.cam * camX;

		// distance 
		/*ofVec2f stepDelta = ofVec2f(
			sqrt(1 + (rayDir.y * rayDir.y) / (rayDir.x * rayDir.x)),
			sqrt(1 + (rayDir.x * rayDir.x) / (rayDir.y * rayDir.y)));
			*/
		float theta = atanf(fabs(rayDir.y) / fabs(rayDir.x));
		ofVec2f stepDelta = ofVec2f(1 / cos(theta), 1 / sin(theta));


		// distance from transform.position to grid
		ofVec2f initialDelta;
		int stepX;
		int stepY;
		int mapX = int(transform.pos.x);
		int mapY = int(transform.pos.y);
		if (rayDir.x < 0)
		{
			stepX = -1;
			initialDelta.x = (transform.pos.x - mapX) * stepDelta.x;
		}
		else
		{
			stepX = 1;
			initialDelta.x = (mapX + 1.0 - transform.pos.x) * stepDelta.x;
		}
		if (rayDir.y < 0)
		{
			stepY = -1;
			initialDelta.y = (transform.pos.y - mapY) * stepDelta.y;
		}
		else
		{
			stepY = 1;
			initialDelta.y = (mapY + 1.0 - transform.pos.y) * stepDelta.y;
		}

		// DDA
		bool side;
		bool hit = false;
		int wallType;
		while (!hit)
		{
			if (initialDelta.x < initialDelta.y)
			{
				initialDelta.x += stepDelta.x;
				mapX += stepX;
				side = false;
			}
			else
			{
				initialDelta.y += stepDelta.y;
				mapY += stepY;
				side = true;
			}
			if (LEVEL[mapX][mapY] > 0) {
				hit = true;
				wallType = LEVEL[mapX][mapY];
			}
		}

		float distance;
		float wallX;
		if (!side) {
			distance = fabs((mapX - transform.pos.x + (1 - stepX) / 2) / rayDir.x);
			wallX = transform.pos.y + ((mapX - transform.pos.x + (1 - stepX) / 2) / rayDir.x) * rayDir.y;
		}
		else {
			distance = fabs((mapY - transform.pos.y + (1 - stepY) / 2) / rayDir.y);
			wallX = transform.pos.x + ((mapY - transform.pos.y + (1 - stepY) / 2) / rayDir.y) * rayDir.x;
		}
		wallX -= floor(wallX);

		int texX = int(wallX * TEX_SIZE);
		if ((!side && rayDir.x > 0) || (side && rayDir.y < 0)) {
			texX = 512 - texX - 1;
		}

		int wallHeight = abs(int(renderer.height / distance));

		int cutoff = (renderer.height - wallHeight) / 2;
		if (cutoff < 0) {
			cutoff = 0;
		}

		for (int y = 0; y < cutoff; y++) {
			pix[i + y * renderer.width] = 0;
		}
		for (int y = cutoff; y < renderer.height - cutoff; y++) {
			int d = y * 256 - renderer.height * 128 + wallHeight * 128;  //256 and 128 factors to avoid floats
			int texY = ((d * TEX_SIZE) / wallHeight) / 256;
			pix[i + y * renderer.width] = wallTextures[wallType][texX + texY * TEX_SIZE];
		}
		for (int y = renderer.height - cutoff; y < renderer.height; y++) {
			pix[i + y * renderer.width] = 0;
		}
	}


	// draw rendered frame to screen
	tex.allocate(pix);
	tex.loadData(pix);
	tex.draw(0,0);

	// minimap
	ofSetColor(0);
	ofDrawBox(0, 0, 0, MINIMAP_SIZE * 2, MINIMAP_SIZE * 2, 0);
	ofDrawGrid(MINIMAP_SIZE, LEVEL_SIZE, false, false, false, true);
	// player icon
	ofSetColor(255, 0, 0, 255);
	ofVec2f start = transform.pos / LEVEL_SIZE * MINIMAP_SIZE;
	ofVec2f end = start + transform.rot * 50;
	ofDrawArrow(start, end);
	ofDrawArrow(end - transform.cam * 3, end + transform.cam * 3);

	//debug info
	ofSetColor(0, 255, 0, 255);
	ofDrawBitmapString(to_string(transform.pos.x) + ", " + to_string(transform.pos.y), 0, 10, 0);
	ofDrawBitmapString(to_string(transform.rot.x) + ", " + to_string(transform.rot.y) + " (" + to_string(transform.rot.angle(ofVec2f(1, 0))) + "deg)", 0, 20, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}
 
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	renderer = Renderer(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
