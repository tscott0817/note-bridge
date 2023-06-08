#include "cmath"
#include "guitar.h"
#include <utility>
#include <algorithm>
#include <string>

Guitar::Guitar(int screenWidth, int screenHeight, float posX, float posY, float width, float height) {

    /** Window Init **/
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    /** State Managing **/
    active = false;  // TODO: May use to 'pause'
    canDraw = false;
    canDrawConnection = false;

    /** Parent Container **/
    containerImage = LoadImage("../images/blue_background.png");     // Loaded in CPU memory (RAM)
    containerTexture = LoadTextureFromImage(containerImage);  // Image converted to texture, GPU memory (VRAM)
    UnloadImage(containerImage);   // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM
    container = {static_cast<float>(this->screenWidth * posX), static_cast<float>(this->screenHeight * posY), static_cast<float>(this->screenWidth * width), static_cast<float>(this->screenHeight * height)};
    containerCenter = {container.width * .5f, container.height * .5f};
    containerLocAdded = false;

    /** Neck **/
    neckImage = LoadImage("../images/wood_dark.png");
    neckTexture = LoadTextureFromImage(neckImage);
    UnloadImage(neckImage);
    neckRectangle = {container.x, container.y, container.width, container.height * .9f}; // TODO: Fill container with neck (Currently have padding for testing)
    neckCenter = {static_cast<float>(neckRectangle.width * .5f), static_cast<float>(neckRectangle.height * .5f)};

    /** Frets **/
    fretImage = LoadImage("../images/fret.png");
    fretTexture = LoadTextureFromImage(fretImage);
    UnloadImage(fretImage);
    fretRectangle = {neckRectangle.x, neckRectangle.y, static_cast<float>(neckRectangle.width * .01), neckRectangle.height};
    fretCenter = {static_cast<float>(fretRectangle.width * .5f), static_cast<float>(fretRectangle.height * .5f)};

    /** Strings **/
    stringImage = LoadImage("../images/silver.png");
    stringTexture = LoadTextureFromImage(stringImage);
    UnloadImage(stringImage);
    stringRectangle = {neckRectangle.x, neckRectangle.y, neckRectangle.width, neckRectangle.height * .02f};
    stringCenter = {static_cast<float>(stringRectangle.width * .5f), static_cast<float>(stringRectangle.height * .5f)};
    // Fill stringVec with each const char
    noteTextVec.emplace_back(lowE);
    noteTextVec.emplace_back(a);
    noteTextVec.emplace_back(d);
    noteTextVec.emplace_back(g);
    noteTextVec.emplace_back(b);
    noteTextVec.emplace_back(highE);

    /** Note Containers **/
    noteRectangle = {neckRectangle.x, neckRectangle.y, neckRectangle.width * .05f, neckRectangle.height * .15f};
    notesLocAdded = false;

    /** Text and Font **/
    testText = "Test Text";
    noteName = "X";
    testFont = LoadFontEx("../resources/fonts/OpenSans-Light.ttf", 200, nullptr, 100);
    fontSize = (float)testFont.baseSize;
    fontPosition = { 500.0f, 1200.0f};
    textSize = { 10.0f, 10.0f };

    // Generate mipmap levels to use trilinear filtering
    GenTextureMipmaps(&testFont.texture);
    SetTextureFilter(testFont.texture, TEXTURE_FILTER_TRILINEAR);

    /** Colors **/
    hoverColor = Color{190, 33, 55, 200};
    rootColor = Color{0, 121, 241, 200};
    clickColor = Color{0, 200, 0, 200};
    secondColor = MAROON;
    thirdColor = GREEN;
    fourthColor = YELLOW;
    fifthColor = PURPLE;

    /** Vector Inits **/
    for (int i = 0; i < 100; i++) {      // TODO: Just filling to 100 for space right now
        std::vector<Vector2> tempLoc;
        std::vector<Color> tempColor;
        std::vector<int> tempClicked;
        for (int j = 0; j < 100; j++) {
            // TODO: Replace all with emplace_back
            tempLoc.emplace_back(Vector2{0, 0});  // With emplace_back, need to specify type
            tempColor.push_back(rootColor);
            tempClicked.push_back(0);
        }
        noteLocations.push_back(tempLoc);
        noteColorVec.push_back(tempColor);
        noteClickedBoolVec.push_back(tempClicked);
        activeNotesVec.emplace_back("X");
    }
    // TODO: The filepath will not play sound, but is same the above font filepath, so it should?
//    testSound = LoadSound("../resources/audio/key13.ogg");
//    testSound = LoadSound("C:/Users/tyler/Dev/guitar-app/resources/audio/key13.ogg");
    testSound = LoadSound("C:/Users/tyler/Dev/guitar-app/resources/audio/key13.ogg");


//    /** Connection Point **/
//    connectImage = LoadImage("../images/fret.png");
//    connectTexture = LoadTextureFromImage(connectImage);
//    UnloadImage(connectImage);
//    connectRectangle = {container.x, container.y - (container.height * .55f), static_cast<float>(container.width * .02f), static_cast<float>(container.width * .02f)};
//    connectCenter = {static_cast<float>(connectRectangle.width / 2), static_cast<float>(connectRectangle.height / 2)};
//
//    /** Bezier Curve **/
//    bezierEnd = {connectRectangle.x, connectRectangle.y};
//    bezierStart = {connectRectangle.x, connectRectangle.y};
//    bezierStart = {connectRectangle.x, connectRectangle.y};
}

void Guitar::draw(float windowScale) {

    /** Container **/
    DrawTexturePro(containerTexture,
                   container,
                   (Rectangle) {container.x, container.y, container.width, container.height},  /** Params = (x-pos, y-pos, height, width) **/
                   containerCenter, 0, WHITE);
    containerLoc = {container.x, container.y};  // TODO: Want to update location only when container is moved, not every frame

    // TODO: Not currently implementing connection points
//    /** Connection Point **/
//    DrawTexturePro(connectTexture,
//                   connectRectangle,
//                   (Rectangle) {connectRectangle.x, connectRectangle.y, connectRectangle.width, connectRectangle.height},
//                   connectCenter, 0, WHITE);

    /** Neck **/
    DrawTexturePro(neckTexture,
                   neckRectangle,
                   (Rectangle) {neckRectangle.x, neckRectangle.y, neckRectangle.width, neckRectangle.height},
                   neckCenter, 0, WHITE);

    /** Frets **/
    // Frets separated by 1/8 width of neck
    DrawTexturePro(stringTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x - (neckRectangle.width * .49)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x - (neckRectangle.width * .41)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x - (neckRectangle.width * .33)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x - (neckRectangle.width * .25)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x - (neckRectangle.width * .17)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x - (neckRectangle.width * .09)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x - (neckRectangle.width * .01)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x + (neckRectangle.width * .07)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x + (neckRectangle.width * .15)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x + (neckRectangle.width * .23)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x + (neckRectangle.width * .31)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x + (neckRectangle.width * .39)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    DrawTexturePro(fretTexture,
                   fretRectangle,
                   (Rectangle) {static_cast<float>(fretRectangle.x + (neckRectangle.width * .47)), fretRectangle.y, fretRectangle.width, fretRectangle.height},
                   fretCenter, 0, WHITE);

    /** Strings **/
    // Strings separated 1/6 height of neck
    DrawTexturePro(stringTexture,
                   stringRectangle,
                   (Rectangle) {stringRectangle.x, static_cast<float>(stringRectangle.y - (neckRectangle.height * .40)), stringRectangle.width, stringRectangle.height},
                   stringCenter, 0, WHITE);
    DrawTexturePro(stringTexture,
                   stringRectangle,
                   (Rectangle) {stringRectangle.x, static_cast<float>(stringRectangle.y - (neckRectangle.height * .24)), stringRectangle.width, stringRectangle.height},
                   stringCenter, 0, WHITE);
    DrawTexturePro(stringTexture,
                   stringRectangle,
                   (Rectangle) {stringRectangle.x, static_cast<float>(stringRectangle.y - (neckRectangle.height * .08)), stringRectangle.width, stringRectangle.height},
                   stringCenter, 0, WHITE);
    DrawTexturePro(stringTexture,
                   stringRectangle,
                   (Rectangle) {stringRectangle.x, static_cast<float>(stringRectangle.y + (neckRectangle.height * .08)), stringRectangle.width, stringRectangle.height},
                   stringCenter, 0, WHITE);
    DrawTexturePro(stringTexture,
                   stringRectangle,
                   (Rectangle) {stringRectangle.x, static_cast<float>(stringRectangle.y + (neckRectangle.height * .24)), stringRectangle.width, stringRectangle.height},
                   stringCenter, 0, WHITE);
    DrawTexturePro(stringTexture,
                   stringRectangle,
                   (Rectangle) {stringRectangle.x, static_cast<float>(stringRectangle.y + (neckRectangle.height * .40)), stringRectangle.width, stringRectangle.height},
                   stringCenter, 0, WHITE);


    /** Note Containers **/
    for (int i = 1; i <= 12; i++) {  // Rows
        for (int j = 1; j <= 6; j++) {  // Columns
            DrawRectangle(static_cast<float>(neckRectangle.x - (neckRectangle.width * .53f) + ((neckRectangle.width * .08) * i) - (noteRectangle.width / 2)), static_cast<float>((neckRectangle.y) - ((neckRectangle.height * .16) * j) + (neckRectangle.height * .56f) - (noteRectangle.height / 2)), noteRectangle.width, noteRectangle.height, BLACK);
            DrawEllipse(static_cast<float>(neckRectangle.x - (neckRectangle.width * .53f) + ((neckRectangle.width * .08) * i)), static_cast<float>((neckRectangle.y) - ((neckRectangle.height * .16) * j) + (neckRectangle.height * .56f)), static_cast<float>(noteRectangle.width / 2), static_cast<float>(noteRectangle.height / 2), noteColorVec[i][j]);

            // Store the container coordinates (only need to once for now)
            // TODO: Always true for now
            if (!notesLocAdded) {
                noteLocations[i][j] = {static_cast<float>(neckRectangle.x - (neckRectangle.width * .53f) + ((neckRectangle.width * .08) * i) - (noteRectangle.width / 2)), static_cast<float>((neckRectangle.y) - ((neckRectangle.height * .16) * j) + (neckRectangle.height * .56f) - (noteRectangle.height / 2))};
            }

            float noteTextSize = (noteRectangle.width > noteRectangle.height) ? static_cast<float>(noteRectangle.height) : static_cast<float>(noteRectangle.width);
            Vector2 noteNewLoc = {static_cast<float>(neckRectangle.x - (neckRectangle.width * .53f) + ((neckRectangle.width * .08) * i) - (noteRectangle.width / 3)), static_cast<float>((neckRectangle.y) - ((neckRectangle.height * .16) * j) + (neckRectangle.height * .56f) - (noteRectangle.height / 2))};
//            DrawTextEx(testFont, lowE[i - 1], noteNewLoc, noteTextSize, 0, WHITE);

            // Draw text using the stringVec, each const char *[] is a string
            DrawTextEx(testFont, noteTextVec[j-1][i-1], noteNewLoc, noteTextSize, 0, WHITE);

        }
    }
}

void Guitar::hover(Vector2 mousePos) {
    for (int i = 0; i < noteLocations.size(); i++) {
        for (int j = 0; j < noteLocations[i].size(); j++) {
            if (mousePos.x > noteLocations[i][j].x && mousePos.x < noteLocations[i][j].x + (noteRectangle.width) &&
                mousePos.y > noteLocations[i][j].y && mousePos.y < noteLocations[i][j].y + (noteRectangle.height) &&
                noteClickedBoolVec[i][j] == 0) {
                noteColorVec[i][j] = hoverColor;
            }
            else if (noteClickedBoolVec[i][j] == 0) {
                noteColorVec[i][j] = rootColor;
            }
        }
    }
}

bool Guitar::isHovering(Vector2 mousePos) {
    // return true if hovering over piano container
    if (mousePos.x > containerLoc.x - (container.width * .5f) && mousePos.x < containerLoc.x + (container.width * .5f) &&
        mousePos.y > containerLoc.y - (container.height * .5f) && mousePos.y < containerLoc.y + (container.height * .5f)) {
        std::cout << "Hovering Guitar" << std::endl;
        return true;
    }
    else {
        return false;
    }
}

void Guitar::clickColorHold(Vector2 mousePos) {
    for (int i = 0; i < noteLocations.size(); i++) {
        for (int j = 0; j < noteLocations[i].size(); j++) {
            if (mousePos.x > noteLocations[i][j].x &&
                mousePos.x < noteLocations[i][j].x + (noteRectangle.width) &&
                mousePos.y > noteLocations[i][j].y &&
                mousePos.y < noteLocations[i][j].y + (noteRectangle.height) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && noteClickedBoolVec[i][j] == 0) {

                //PlaySound(testSound);
                noteClickedBoolVec[i][j] = 1;
                noteColorVec[i][j] = clickColor;
                selectedNotesVec.emplace_back(noteTextVec[j-1][i-1]);

            }

            else if (mousePos.x > noteLocations[i][j].x &&
                     mousePos.x < noteLocations[i][j].x + (noteRectangle.width) &&
                     mousePos.y > noteLocations[i][j].y &&
                     mousePos.y < noteLocations[i][j].y + (noteRectangle.height) &&
                     IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && noteClickedBoolVec[i][j] == 1) {

                noteClickedBoolVec[i][j] = 0;
                noteColorVec[i][j] = rootColor;

                // TODO: This seems to get the correct index, but feels a bit odd.
                int index = 0;
                for (int k = 0; k < selectedNotesVec.size(); k++) {
                    if (selectedNotesVec[k] == noteTextVec[j-1][i-1]) {
                        index = k;
                    }
                }
                selectedNotesVec.erase(selectedNotesVec.begin() + index);
            }
        }
    }
}

void Guitar::notesActivate() {
    // print out the work ;active;
    std::cout << "Active" << std::endl;
    // Using std::find, check if activeNotesVec contains a "C". If it does, then change color of all indices with a "C"
    if (std::find(activeNotesVec.begin(), activeNotesVec.end(), "C") != activeNotesVec.end()) {
        // Change the color of all C's
        noteColorVec[1][5] = clickColor;
        noteColorVec[3][2] = clickColor;
        noteColorVec[5][4] = clickColor;
        noteColorVec[8][1] = clickColor;
        noteColorVec[8][6] = clickColor;
        noteColorVec[10][3] = clickColor;
    }
    else {
        noteColorVec[1][5] = rootColor;
        noteColorVec[3][2] = rootColor;
        noteColorVec[5][4] = rootColor;
        noteColorVec[8][1] = rootColor;
        noteColorVec[8][6] = rootColor;
        noteColorVec[10][3] = rootColor;
    }
//    if (std::find(activeNotesVec.begin(), activeNotesVec.end(), "Db") != activeNotesVec.end()) {
//        // Change the color of all C's
//        noteColorVec[2][3] = clickColor;
//        noteColorVec[4][3] = clickColor;
//        noteColorVec[6][5] = clickColor;
//        noteColorVec[9][2] = clickColor;
//        noteColorVec[9][7] = clickColor;
//        noteColorVec[11][4] = clickColor;
//    }
//    else {
//        noteColorVec[2][3] = rootColor;
//        noteColorVec[4][3] = rootColor;
//        noteColorVec[6][5] = rootColor;
//        noteColorVec[9][2] = rootColor;
//        noteColorVec[9][7] = rootColor;
//        noteColorVec[11][4] = rootColor;
//    }
    if (std::find(activeNotesVec.begin(), activeNotesVec.end(), "D") != activeNotesVec.end()) {
        // Change the color of all C's
        noteColorVec[3][5] = clickColor;
        noteColorVec[5][2] = clickColor;
        noteColorVec[7][4] = clickColor;
        noteColorVec[10][1] = clickColor;
        noteColorVec[10][6] = clickColor;
        noteColorVec[12][3] = clickColor;
    }
    else {
        noteColorVec[3][5] = rootColor;
        noteColorVec[5][2] = rootColor;
        noteColorVec[7][4] = rootColor;
        noteColorVec[10][1] = rootColor;
        noteColorVec[10][6] = rootColor;
        noteColorVec[12][3] = rootColor;
    }

}

void Guitar::soundTests() {
    // TODO: Just test, remove
//    testSound = LoadSound("C:/Users/tyler/Dev/guitar-app/resources/audio/key13.ogg");
    if (IsKeyPressed(KEY_SPACE)){
        PlaySound(testSound);
        std::cout << "Playing Sound From Guitar Class" << std::endl;
    }
}

bool Guitar::connectionHover(Vector2 mousePos) {
    if (mousePos.x > connectRectangle.x && mousePos.x < connectRectangle.x + (connectRectangle.width) &&
        mousePos.y > connectRectangle.y && mousePos.y < connectRectangle.y + (connectRectangle.height)) {
        return true;
    } else {
        return false;
    }
}

void Guitar::clickAndDrag(Vector2 mousePos) {
    if (mousePos.x > containerLoc.x - (container.width * .5f) && mousePos.x < containerLoc.x + (container.width * .5f) &&
        mousePos.y > containerLoc.y - (container.height * .5f) && mousePos.y < containerLoc.y + (container.height * .5f)) {

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            // TODO: Should only have to change the container?? Maybe since initial build is in constructor.
            // TODO: Probably reset them all to their constructor values, think mousePos works only because relative to container
            container.x = mousePos.x;
            container.y = mousePos.y;
            // TODO: Not sure why these have to be different?
            connectRectangle.x = container.x;
            connectRectangle.y = container.y - (container.height * .55f);
            bezierStart.x = container.x;
            bezierStart.y = container.y - (container.height * .55f);
            neckRectangle.x = mousePos.x;
            neckRectangle.y = mousePos.y;
            fretRectangle.x = mousePos.x;
            fretRectangle.y = mousePos.y;
            stringRectangle.x = mousePos.x;
            stringRectangle.y = mousePos.y;
            noteRectangle.x = mousePos.x;
            noteRectangle.y = mousePos.y;
        }
    }
    // Update the container location
    containerLoc = {container.x, container.y};
}

/** Getter **/
bool Guitar::getStateActive() { return active; }
bool Guitar::getCanDraw() { return canDraw; }
bool Guitar::getCanDrawConnection() { return canDrawConnection;}
Rectangle Guitar::getContainer() { return container; }
Rectangle Guitar::getConnectionRec() { return connectRectangle;}
std::vector<std::string> Guitar::getSelectedNotes() { return selectedNotesVec; }
//std::vector<std::vector<int>> Guitar::getNoteClickedBoolVec() { return noteClickedBoolVec; }

/** Setters **/
void Guitar::setStateActive(bool state) { active = state; }
void Guitar::setCanDraw(bool state) { canDraw = state; }
void Guitar::setCanDrawConnection(bool state) { canDrawConnection = state; }
//void Guitar::setNoteClickBoolVec(std::vector<std::vector<int>> newVec) { noteClickedBoolVec = newVec; }
//void Neck::setConnectionPos(Vector2 pos) { connectRectangle.x = pos.x; connectRectangle.y = pos.y;}
//void Neck::setConnectionRec(Rectangle rec) { connectRectangle = rec;}
//void Guitar::addSelectNote(const std::string &notes) {selectedNotesVec.push_back(notes);}
void Guitar::setActiveNotes(std::vector<std::string> newVec) {activeNotesVec = std::move(newVec);}

// To remove textures from memory after program closes, must be after main loop ends
void Guitar::destroy() {
    UnloadTexture(containerTexture);
    UnloadTexture(neckTexture);
    UnloadTexture(fretTexture);
    UnloadTexture(stringTexture);
    UnloadSound(testSound);
}
