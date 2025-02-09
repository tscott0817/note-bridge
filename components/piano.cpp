#include "piano.h"
#include <utility>
#include <algorithm>
#include <string>

Piano::Piano(int screenWidth, int screenHeight, float posX, float posY, float width, float height) {

    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    containerImage = LoadImage("../images/blue_background.png");     // Loaded in CPU memory (RAM)
    containerTexture = LoadTextureFromImage(containerImage);  // Image converted to texture, GPU memory (VRAM)
    UnloadImage(containerImage);   // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM
    container = {static_cast<float>(this->screenWidth * posX), static_cast<float>(this->screenHeight * posY), static_cast<float>(this->screenWidth * width), static_cast<float>(this->screenHeight * height)};
    containerCenter = {container.width * .5f, container.height * .5f};
    containerLocAdded = false;
    bgLoc = {static_cast<float>(this->screenWidth * posX), static_cast<float>(this->screenHeight * posY), static_cast<float>(this->screenWidth * width), static_cast<float>(this->screenHeight * height)};

    keyboardImage = LoadImage("../images/blue_background.png");     // Loaded in CPU memory (RAM)
    keyboardTexture = LoadTextureFromImage(keyboardImage);  // Image converted to texture, GPU memory (VRAM)
    UnloadImage(keyboardImage);   // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM
    keyboardRectangle = {static_cast<float>(this->screenWidth * posX), static_cast<float>(this->screenHeight * posY), static_cast<float>(this->screenWidth * width), static_cast<float>(this->screenHeight * height)};
    keyboardCenter = {container.width * .5f, container.height * .5f};
    // keyboardLocAdded = false;
    keyboardLoc = {static_cast<float>(this->screenWidth * posX), static_cast<float>(this->screenHeight * posY), static_cast<float>(this->screenWidth * width), static_cast<float>(this->screenHeight * height)};


    keyWhiteImage = LoadImage("../images/silver.png");
    keyWhiteTexture = LoadTextureFromImage(keyWhiteImage);
    UnloadImage(keyWhiteImage);
    keyWhiteRectangle = {container.x, container.y + (container.height * .125f), static_cast<float>(container.width * .125), static_cast<float>(container.height * .75)};
    keyWhiteCenter = {static_cast<float>(keyWhiteRectangle.width * .5f), static_cast<float>(keyWhiteRectangle.height * .5f)};

    keyBlackImage = LoadImage("../images/fret.png");
    keyBlackTexture = LoadTextureFromImage(keyBlackImage);
    UnloadImage(keyBlackImage);
    keyBlackRectangle = {container.x, container.y + (container.height * .125f), static_cast<float>(keyWhiteRectangle.width * .6), static_cast<float>(keyWhiteRectangle.height * .6)};
    keyBlackCenter = {static_cast<float>(keyBlackRectangle.width * .5f), static_cast<float>(keyBlackRectangle.height * .5f)};

    /** Colors **/
    whiteKeyColor = WHITE;
    blackKeyColor = BLACK;

    whiteKeyHover = false;
    blackKeyHover = false;

    /** Vector Inits **/
    for (int i = 0; i < 100; i++) {      // TODO: Just filling to 100 for space right now
        keyWhiteColorVec.emplace_back(WHITE);
        keyBlackColorVec.emplace_back(BLACK);
        keyWhiteLocations.emplace_back(Vector2{0, 0});
        keyBlackLocations.emplace_back(Vector2{0, 0});
        noteClickedBoolVecWhite.emplace_back(0);
        noteClickedBoolVecBlack.emplace_back(0);
    }
    notesLocAdded = false;

    InitAudioDevice();
    notesVec.emplace_back(F);
    noteTextVec.emplace_back(keysWhite);
    noteTextVec.emplace_back(keysBlack);

    containerColor = Color{51, 51, 51, 0};
    keyboardColor = Color{51, 51, 51, 255};

}

void Piano::draw(float windowScale) {

    /** Parent Container **/
    DrawRectangle(container.x - (container.width * .5f), container.y - (container.height * .5f), container.width, container.height, containerColor);
    containerLoc = {container.x, container.y};  // TODO: Not sure if needed; Maybe want to update location only when container is moved, not every frame

    // Draw the keyboard
    DrawRectangle(keyboardRectangle.x - (keyboardRectangle.width * .504f), keyboardRectangle.y - (keyboardRectangle.height * .504f), keyboardRectangle.width * 1.01f, keyboardRectangle.height * 1.02f, BLACK);
    DrawRectangle(keyboardRectangle.x - (keyboardRectangle.width * .5f), keyboardRectangle.y - (keyboardRectangle.height * .5f), keyboardRectangle.width, keyboardRectangle.height, keyboardColor);

    /** White Keys **/
    // Draw one rectangle to encapsulate each key
    DrawRectangle(keyboardRectangle.x - (keyboardRectangle.width * .458f), keyboardRectangle.y - (keyboardRectangle.height * .265f), keyboardRectangle.width * .92f, keyboardRectangle.height * .77f, BLACK);

    DrawRectangle(keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) - (container.width * .39f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f), keyWhiteRectangle.width, keyWhiteRectangle.height, keyWhiteColorVec[0]);
    DrawRectangle(keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) - (container.width * .26f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f), keyWhiteRectangle.width, keyWhiteRectangle.height, keyWhiteColorVec[1]);
    DrawRectangle(keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) - (container.width * .13f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f), keyWhiteRectangle.width, keyWhiteRectangle.height, keyWhiteColorVec[2]);
    DrawRectangle(keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f), keyWhiteRectangle.width, keyWhiteRectangle.height, keyWhiteColorVec[3]);
    DrawRectangle(keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) + (container.width * .13f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f), keyWhiteRectangle.width, keyWhiteRectangle.height, keyWhiteColorVec[4]);
    DrawRectangle(keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) + (container.width * .26f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f), keyWhiteRectangle.width, keyWhiteRectangle.height, keyWhiteColorVec[5]);
    DrawRectangle(keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) + (container.width * .39f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f), keyWhiteRectangle.width, keyWhiteRectangle.height, keyWhiteColorVec[6]);

    keyWhiteLocations[0] = {keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) - (container.width * .39f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f)};
    keyWhiteLocations[1] = {keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) - (container.width * .26f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f)};
    keyWhiteLocations[2] = {keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) - (container.width * .13f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f)};
    keyWhiteLocations[3] = {keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f)};
    keyWhiteLocations[4] = {keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) + (container.width * .13f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f)};
    keyWhiteLocations[5] = {keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) + (container.width * .26f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f)};
    keyWhiteLocations[6] = {keyWhiteRectangle.x - (keyWhiteRectangle.width * .5f) + (container.width * .39f), keyWhiteRectangle.y - (keyWhiteRectangle.height * .5f)};

    /** Black Keys **/
    DrawRectangle(keyBlackRectangle.x - (container.width * .365f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width, keyBlackRectangle.height, BLACK);
    DrawRectangle(keyBlackRectangle.x - (container.width * .235f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width, keyBlackRectangle.height, BLACK);
    DrawRectangle(keyBlackRectangle.x + (keyWhiteRectangle.width * .2f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width, keyBlackRectangle.height, BLACK);
    DrawRectangle(keyBlackRectangle.x + (container.width * .1575f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width, keyBlackRectangle.height, BLACK);
    DrawRectangle(keyBlackRectangle.x + (container.width * .2875f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width, keyBlackRectangle.height, BLACK);

    DrawRectangle(keyBlackRectangle.x - (container.width * .36225f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width * .95f, keyBlackRectangle.height * .98f, keyBlackColorVec[0]);
    DrawRectangle(keyBlackRectangle.x - (container.width * .23225f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width * .95f, keyBlackRectangle.height * .98f, keyBlackColorVec[1]);
    DrawRectangle(keyBlackRectangle.x + (keyWhiteRectangle.width * .223f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width * .95f, keyBlackRectangle.height * .98f, keyBlackColorVec[2]);
    DrawRectangle(keyBlackRectangle.x + (container.width * .16025f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width * .95f, keyBlackRectangle.height * .98f, keyBlackColorVec[3]);
    DrawRectangle(keyBlackRectangle.x + (container.width * .29025f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f), keyBlackRectangle.width * .95f, keyBlackRectangle.height * .98f, keyBlackColorVec[4]);

    keyBlackLocations[0] = {keyBlackRectangle.x - (container.width * .365f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f)};
    keyBlackLocations[1] = {keyBlackRectangle.x - (container.width * .235f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f)};
    keyBlackLocations[2] = {keyBlackRectangle.x + (keyWhiteRectangle.width * .2f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f)};
    keyBlackLocations[3] = {keyBlackRectangle.x + (container.width * .1575f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f)};
    keyBlackLocations[4] = {keyBlackRectangle.x + (container.width * .2875f), keyBlackRectangle.y - (keyBlackRectangle.height * .85f)};

}

void Piano::selectNote(Vector2 mousePos) {
    if (!IsKeyDown(KEY_LEFT_CONTROL)) {  // TODO: Make this into field of base class
        for (int i = 0; i < keyBlackLocations.size(); i++) {
            if (mousePos.x > keyBlackLocations[i].x &&
                mousePos.x < keyBlackLocations[i].x + (keyBlackRectangle.width) &&
                mousePos.y > keyBlackLocations[i].y &&
                mousePos.y < keyBlackLocations[i].y + (keyBlackRectangle.height) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && noteClickedBoolVecBlack[i] == 0) {

                noteClickedBoolVecBlack[i] = 1;
                addNoteShared(noteTextVec[1][i]); // TODO: [1][x] is for black keys, [0][x] is for white keys
                PlaySound(notesSoundVecBlack[i]);

            } else if (mousePos.x > keyBlackLocations[i].x &&
                       mousePos.x < keyBlackLocations[i].x + (keyBlackRectangle.width) &&
                       mousePos.y > keyBlackLocations[i].y &&
                       mousePos.y < keyBlackLocations[i].y + (keyBlackRectangle.height) &&
                       IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && noteClickedBoolVecBlack[i] == 1) {

                noteClickedBoolVecBlack[i] = 0;
                removeNoteShared(noteTextVec[1][i]);
                PlaySound(notesSoundVecBlack[i]);
            }
        }
        if (!hoverBlackNotes(mousePos)) {
            for (int i = 0; i < keyWhiteLocations.size(); i++) {
                if (mousePos.x > keyWhiteLocations[i].x &&
                    mousePos.x < keyWhiteLocations[i].x + (keyWhiteRectangle.width) &&
                    mousePos.y > keyWhiteLocations[i].y &&
                    mousePos.y < keyWhiteLocations[i].y + (keyWhiteRectangle.height) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && noteClickedBoolVecWhite[i] == 0) {

                    noteClickedBoolVecWhite[i] = 1;
                    addNoteShared(noteTextVec[0][i]);  // TODO: [1][x] is for black keys, [0][x] is for white keys
                    PlaySound(notesSoundVecWhite[i]);

                } else if (mousePos.x > keyWhiteLocations[i].x &&
                           mousePos.x < keyWhiteLocations[i].x + (keyWhiteRectangle.width) &&
                           mousePos.y > keyWhiteLocations[i].y &&
                           mousePos.y < keyWhiteLocations[i].y + (keyWhiteRectangle.height) &&
                           IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && noteClickedBoolVecWhite[i] == 1) {

                    noteClickedBoolVecWhite[i] = 0;
                    removeNoteShared(noteTextVec[0][i]);
                    PlaySound(notesSoundVecWhite[i]);
                }
            }
        }
    }
}

bool Piano::hoverBlackNotes(Vector2 mousePos) {
    for (auto& keyBlackLocation : keyBlackLocations) {
        if (mousePos.x > keyBlackLocation.x &&
            mousePos.x < keyBlackLocation.x + keyBlackRectangle.width &&
            mousePos.y > keyBlackLocation.y &&
            mousePos.y < keyBlackLocation.y + keyBlackRectangle.height) {
            return true;
        }
    }
    return false;  // Return false if no black key matches
}

void Piano::clickAndDrag(Vector2 mousePos) {
    if (mousePos.x > containerLoc.x - (container.width * .5f) && mousePos.x < containerLoc.x + (container.width * .5f) &&
        mousePos.y > containerLoc.y - (container.height * .5f) && mousePos.y < containerLoc.y + (container.height * .5f)) {
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            // TODO: Should only have to change the container?? Maybe since initial build is in constructor.
            container.x = mousePos.x;
            container.y = mousePos.y;
            keyboardRectangle.x = container.x;
            keyboardRectangle.y = container.y;
            keyWhiteRectangle.x = container.x;
            keyWhiteRectangle.y = container.y + (container.height * .125f);
            keyBlackRectangle.x = container.x;
            keyBlackRectangle.y = container.y + (container.height * .125f);
            whiteKeyHover = false;
            blackKeyHover = false;
        }
        // TODO: Don't think I like this here
        else {
            whiteKeyHover = true;
            blackKeyHover = true;
        }
    }
    containerLoc = {container.x, container.y};
}

void Piano::notesActivate() {
    std::vector<std::string> whiteKeysTemp = {"C", "D", "E", "F", "G", "A", "B"};
    std::vector<std::string> blackKeysTemp = {"Db/C#", "Eb/D#", "Gb/F#", "Ab/G#", "Bb/A#"};
//    std::vector<std::string> blackKeysTempSharp = {"C#", "D#", "F#", "G#", "A#"};

    for (int i = 0; i < whiteKeysTemp.size(); i++) {
        if (std::find(sharedNotesVec.begin(), sharedNotesVec.end(), whiteKeysTemp[i]) != sharedNotesVec.end()) {
            keyWhiteColorVec[i] = whiteKeyColorVec[i];
            noteClickedBoolVecWhite[i] = 1;
        } else {
            keyWhiteColorVec[i] = whiteKeyColor;
            noteClickedBoolVecWhite[i] = 0;
        }
    }

    for (int i = 0; i < blackKeysTemp.size(); i++) {
        if (std::find(sharedNotesVec.begin(), sharedNotesVec.end(), blackKeysTemp[i]) != sharedNotesVec.end()) {
            keyBlackColorVec[i] = blackKeyColorVec[i];
            noteClickedBoolVecBlack[i] = 1;
        } else {
            keyBlackColorVec[i] = blackKeyColor;
            noteClickedBoolVecBlack[i] = 0;
        }
    }

//    for (int i = 0; i < blackKeysTempSharp.size(); i++) {
//        if (std::find(sharedNotesVec.begin(), sharedNotesVec.end(), blackKeysTempSharp[i]) != sharedNotesVec.end()) {
//            keyBlackColorVec[i] = blackKeyColorVec[i];
//            noteClickedBoolVecBlack[i] = 1;
//        } else {
//            keyBlackColorVec[i] = blackKeyColor;
//            noteClickedBoolVecBlack[i] = 0;
//        }
//    }
}

/** Getters **/
bool Piano::getCanDraw() {return canDraw;}

/** Setters **/
void Piano::setCanDraw(bool canDraw) {this->canDraw = canDraw;}

/** Destruct **/
// TODO: Not sure if this is best approach
void Piano::destroy() {UnloadTexture(containerTexture); UnloadSound(F); UnloadSound(soundNoteTest); CloseAudioDevice();}

void Piano::initAudio() {
    Sound noteC = LoadSound("../resources/audio/key08.ogg");  // TODO: Need to fix filepath
    Sound noteDb = LoadSound("../resources/audio/key09.ogg");
    Sound noteD = LoadSound("../resources/audio/key10.ogg");
    Sound noteEb = LoadSound("../resources/audio/key11.ogg");
    Sound noteE = LoadSound("../resources/audio/key12.ogg");
    Sound noteF = LoadSound("../resources/audio/key13.ogg");
    Sound noteGb = LoadSound("../resources/audio/key14.ogg");
    Sound noteG = LoadSound("../resources/audio/key15.ogg");
    Sound noteAb = LoadSound("../resources/audio/key16.ogg");
    Sound noteA = LoadSound("../resources/audio/key17.ogg");
    Sound noteBb = LoadSound("../resources/audio/key18.ogg");
    Sound noteB = LoadSound("../resources/audio/key19.ogg");

    notesSoundVecWhite.emplace_back(noteC);
    notesSoundVecBlack.emplace_back(noteDb);
    notesSoundVecWhite.emplace_back(noteD);
    notesSoundVecBlack.emplace_back(noteEb);
    notesSoundVecWhite.emplace_back(noteE);
    notesSoundVecWhite.emplace_back(noteF);
    notesSoundVecBlack.emplace_back(noteGb);
    notesSoundVecWhite.emplace_back(noteG);
    notesSoundVecBlack.emplace_back(noteAb);
    notesSoundVecWhite.emplace_back(noteA);
    notesSoundVecBlack.emplace_back(noteBb);
    notesSoundVecWhite.emplace_back(noteB);

    // TODO: Think I can just keep adding note as octaves go up, indices should stay the same I think
}

void Piano::playSound() {
    // TODO: I don't like this here, move it to .h file maybe
    std::vector<std::string> whiteKeysTemp = {"C", "D", "E", "F", "G", "A", "B"};  // TODO: Just keep adding as octaves go up, indices should stay the same I think
    std::vector<std::string> blackKeysTemp = {"Db/C#", "Eb/D#", "Gb/F#", "Ab/G#", "Bb/A#"};
//    std::vector<std::string> blackKeysTemp = {"Db", "Eb", "Gb", "Ab", "Bb"};

    std::cout << "Playing Sound" << std::endl;
    for (const auto &note: getNotesShared()) {
        for (int i = 0; i < notesSoundVecWhite.size(); i++) {
            if (note == whiteKeysTemp[i]) {
                PlaySound(notesSoundVecWhite[i]);
            }
        }
        for (int i = 0; i < notesSoundVecBlack.size(); i++) {
            if (note == blackKeysTemp[i]) {
                PlaySound(notesSoundVecBlack[i]);
            }
        }
    }
}

void Piano::unloadAudio() {
    for (const auto & note : notesSoundVecWhite) {
        UnloadSound(note);
    }
    for (const auto & note : notesSoundVecBlack) {
        UnloadSound(note);
    }
}



