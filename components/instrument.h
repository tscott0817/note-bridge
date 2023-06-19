#ifndef GUITAR_APP_INSTRUMENT_H
#define GUITAR_APP_INSTRUMENT_H
#include "raylib.h"
#include "instrument.h"
#include <iostream>
#include <vector>
#include <memory>

class Instrument {
private:

protected:
    int screenWidth{};
    int screenHeight{};

    /** Every Instrument is Held in a Container **/
    Image containerImage{};
    Texture2D containerTexture{};
    Rectangle container{};
    Vector2 containerCenter{};
    Vector2 containerLoc{};
    bool containerLocAdded{};

    // TODO: Not sure if i will end up doing the bezier, might just stack instr in rows
    /** Connection Point **/
    Image connectImage{};
    Texture2D connectTexture{};
    Rectangle connectRectangle{};
    Vector2 connectCenter{};
    Vector2 connectLoc{};
    bool connectLocAdded{};

    /** Colors **/
    // Colors for individual notes
    Color rootColor = Color{238, 238, 238, 200};
    Color cNoteColor = Color{168, 50, 50, 255};
    Color dbNoteColor = Color{184, 171, 61, 255};
    Color dNoteColor = Color{38, 128, 80, 255};
    Color ebNoteColor = Color{168, 111, 50, 255};
    Color eNoteColor = Color{168, 50, 152, 255};
    Color fNoteColor = Color{48, 102, 150, 255};
    Color gbNoteColor = Color{60, 194, 183, 255};
    Color gNoteColor = Color{126, 74, 247, 255};
    Color abNoteColor = Color{49, 62, 158, 255};
    Color aNoteColor = Color{130, 176, 49, 255};
    Color bbNoteColor = Color{127, 50, 168, 255};
    Color bNoteColor = Color{179, 64, 117, 255};

    /** States Each Object Has **/
    bool active{};  // TODO: Make this private with a getter/setter;
    bool canDraw{};

    /** Track All Shared Notes **/
    static std::vector<std::string> sharedNotesVec;


public:
    Instrument();
    ~Instrument();

    /** Base Class Specific Methods **/
    static void addNoteShared(const std::string& item);
    static void printNotesShared();
    static void removeNoteShared(const std::string& note);

    /** Methods to be Overridden **/
    virtual void selectNote(Vector2 mousePos) = 0;
    virtual void draw(float windowScale) = 0;
    virtual void clickAndDrag(Vector2 mousePos) = 0;
    virtual void notesActivate() = 0;
    virtual void destroy() = 0;

    // Setters
    virtual void setCanDraw(bool state) = 0;

    // Getters
    virtual bool getCanDraw() = 0;

};


#endif //GUITAR_APP_INSTRUMENT_H
