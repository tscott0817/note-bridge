#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION  // Avoid including raygui implementation again
#include <iostream>
#include <utility>
#include <algorithm>
#include <string>
#include "components/guitar.h"
#include "components/piano.h"
#include "components/modal_chart.h"
#include "controller/menu.h"
#include "controller/scalesMenu.h"
#include "controller/chordMenu.h"


// TODO: If using webassembly, probably don't need to check OS
/**
 * Check OS
 */
//#include <Windows.h>
//
//#ifdef _WIN32
//    const std::string operatingSystem = "Windows";
//#elif __APPLE__
//    const std::string operatingSystem = "macOS";
//#elif __linux__
//    const std::string operatingSystem = "Linux";
//#elif __unix__ // all unices not caught above
//    const std::string operatingSystem = "Unix";
//#else
//    const std::string operatingSystem = "Unknown";
//#endif


/**
 * For window resizing
 */
constexpr int MIN(int a, int b) {
    return (a < b) ? a : b;
}

constexpr int MAX(int a, int b) {
    return (a > b) ? a : b;
}

// TODO: These won't work if in separate class?
// void playSound(const std::vector<std::unique_ptr<Instrument>>& instrumentsVec, const std::vector<Sound>& notesSoundVec);
std::vector<Sound> initPianoAudio();
void unloadPianoAudio(std::vector<Sound> notesSoundVec);

int main()
{
    /** Window Setup **/
    // TODO: Check user's computer for their screen size and set window size accordingly
    // 16x9 Resolutions:
    //    640x360
    //    854x480
    //    1024x576
    //    1152x648
    //    1280x720 (720p)
    //    1366x768
    //    1600x900
    //    1920x1080 (1080p)
    //    2048x1152
    //    2304x1296
    //    2560x1440 (1440p)
    //    2880x1620
    //    3200x1800
    //    3840x2160 (4K UHD)
    const int screenWidth =  1600;
    const int screenHeight = 900;

    // Color backgroundColor = RAYWHITE;  // Ignore error; I prefer knowing the type instead of 'auto'
//    Color backgroundColor = Color({25, 43, 66, 255});
//    Color backgroundColor = Color({204, 204, 204, 255});
    Color backgroundColor = Color({192, 192, 192, 255});
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);  // Allows window to be resized
    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Anti-aliasing for shape edges
    InitWindow(screenWidth, screenHeight, "Guitar App");
    SetExitKey(KEY_ZERO);  // Frees up escape key for menu, makes '0' exit program

    /** Object Inits **/
    Menu menu(screenWidth, screenHeight, 0, screenHeight * .0001f, 1, .055f);
    ScalesMenu scalesMenu(screenWidth, screenHeight, screenWidth * .01f, screenHeight * .06, .4f, .85f);
    ChordMenu chordMenu(screenWidth, screenHeight, screenWidth * .01f, screenHeight * .06, .4f, .85f);

    std::vector<std::unique_ptr<Instrument>> instrumentsVec;  // Holds all instruments
    float guitarWidth = .5f;
    float guitarHeight = .3f;
    float guitarPosX = .5f;  // The ( * 0.5f) are basically scalars for the guitar's position
    float guitarPosY = .8f;
    std::unique_ptr<Guitar> guitar = std::make_unique<Guitar>(screenWidth, screenHeight, guitarPosX, guitarPosY, guitarWidth, guitarHeight);
    instrumentsVec.emplace_back(std::move(guitar));

    float pianoWidth = .4f;
    float pianoHeight = .3f;
    float pianoPosX = .5f;
    float pianoPosY = .3f;
    std::unique_ptr<Piano> piano = std::make_unique<Piano>(screenWidth, screenHeight, pianoPosX, pianoPosY, pianoWidth, pianoHeight);
    instrumentsVec.emplace_back(std::move(piano));

    // InitAudioDevice();  // TODO: Not sure if best here in main, or if each class should have one

    /** Audio **/
    instrumentsVec[1]->initAudio();  // TODO: Just Piano has audio currently

    std::string instrSoundChoice = "Piano";  // TODO: Make this user assignable to choose sound

    bool canDrawScaleMenu = false;  // TODO: Preer to not have this here
    bool canDrawChordMenu = false;
    int prevButton = -1;

    /** Main Loop **/
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // TODO: This can be used to resize objects dynamically with the window size,
        // TODO: Needs to be called each frame, so I think it should be param for object methods
        float scale = MIN((float)GetScreenWidth()/screenWidth, (float)GetScreenHeight()/screenHeight);

        // For mouse interactions
        Vector2 mousePos = GetMousePosition();
        bool leftMouseClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        /** Handles appropriate instrument functions to call **/
        for (const auto & instrument : instrumentsVec) {
            if (instrument->getCanDraw()) {  // Only allow interactions if menu has activated object
                instrument->selectNote(mousePos);  // Highlights chosen not on selected instrument
                instrument->notesActivate();  // Activates notes on the other non-selected instruments
                instrument->clickAndDrag(mousePos);
            }
        }

        // Check keyboard for escape key press
        if (IsKeyPressed(KEY_ESCAPE)) {
            menu.canDraw = !menu.canDraw;
        }
        menu.hover(mousePos);

        /** Determines Which Objects Are Shown **/
        // Menu stuff
        // TODO: Need to make some boolean check in instruments to add locations or
        //  mouse interactions still work even if instruments is not visible
        for (int i = 0; i < 4; i++) {
            if (menu.getActiveButtons()[i] == 0) {
                if (i == 2) {
                    canDrawScaleMenu = false;
                }
                else if (i == 3) {
                    canDrawChordMenu = false;
                }
                else {
                    instrumentsVec[i]->setCanDraw(false);
                }
            }
            if (menu.getActiveButtons()[i] == 1) {
                if (i == 2) {
                    canDrawScaleMenu = true;
                    canDrawChordMenu = false;
                    if (prevButton == 3) {
                        canDrawChordMenu = false;
                        menu.deactivateButton(3);  // Deactivate chord button if scales button is active
                    }
                }
                else if (i == 3) {
                    canDrawChordMenu = true;
                    canDrawScaleMenu = false;
                    if (prevButton == 2) {
                        canDrawScaleMenu = false;
                        menu.deactivateButton(2);  // Deactivate scales button if chord button is active
                    }
                }
                else {
                    instrumentsVec[i]->setCanDraw(true);
                }
                prevButton = i;
            }
        }
//        bool canDrawScaleMenu = false;  // TODO: Preer to not have this here
//        bool canDrawChordMenu = false;
//        for (int i = 0; i < 4; i++) {
//            if (menu.getActiveButtons()[i] == 0) {  // This checks if the current button is active or not (0 = inactive, 1 = active)
//                if (i == 2) {  // TODO: 2 is the scale chart, need to take different approach
//                    canDrawScaleMenu = false;
//                }
//                else if (i == 3) {  // TODO: 3 is the chord chart, need to take different approach
//                    canDrawChordMenu = false;
//                }
//                else {
//                    instrumentsVec[i]->setCanDraw(false);
//                }
//            }
//            if (menu.getActiveButtons()[i] == 1) {
//                if (i == 2) {
//                    canDrawScaleMenu = true;
//                    canDrawChordMenu = false;
//                }
//                else if (i == 3) {
//                    canDrawChordMenu = true;
//                    canDrawScaleMenu = false;
//                }
//                else {
//                    instrumentsVec[i]->setCanDraw(true);
//                }
//            }
//        }

        /** Object Drawing Here  **/
        BeginDrawing();

        ClearBackground(backgroundColor);
        menu.setBackground(screenWidth, screenHeight);  // TODO: Probably don't want this in menu class

        // Draw Objects
        for (const auto & instrument : instrumentsVec) {
            if (instrument->getCanDraw()) {
                instrument->draw(scale);  // Scale is not being used currently
            }
        }

        // TODO: Put these before each GUI element to style them individually
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0x000000FF);  // TODO: Don't hard code this
        // Change the button color, not the text color
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xE2E2E2FF);   // TODO: Don't hard code this
        GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
        /** This will be a separate object, just for testing now **/
        if (GuiButton((Rectangle){screenWidth * .0f, screenHeight * .92f, screenWidth * .08f, screenHeight * .08f}, "Clear")) {
            for (const auto & instrument : instrumentsVec) {
                instrument->clearNotesShared();
            }
        }
        /** Create a second button that will read the sharedNotesVec and use PlaySound to play appropriate notes **/
        if (GuiButton((Rectangle){screenWidth * .08f, screenHeight * .92f, screenWidth * .08f, screenHeight * .08f}, "Play")) {
            if (instrSoundChoice == "Guitar") {
                instrumentsVec[0]->playSound();
            }
            else if (instrSoundChoice == "Piano") {
                instrumentsVec[1]->playSound();
            }
        }

        // TODO: Refactor all of this into menu state machine
        if (canDrawChordMenu) {
            chordMenu.draw();
            chordMenu.setChord(screenWidth, screenHeight, instrumentsVec);
            menu.deactivateButton(2);
        }
        if (canDrawScaleMenu) {
            scalesMenu.draw();
            scalesMenu.setScale(screenWidth, screenHeight, instrumentsVec);  // TODO: Maybe separate this into two functions
            menu.deactivateButton(3);
        }

        // Want Menu to be drawn last so it's on top
        menu.drawTopMenu(screenWidth, screenHeight);

        EndDrawing();
    }

    /** Object Destruction **/
    for (const auto & inst : instrumentsVec) {
        inst->destroy();
    }
    // CloseAudioDevice();
    instrumentsVec[1]->unloadAudio();
    CloseWindow();        // Close window and OpenGL context

    return 0;
}