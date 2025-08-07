#ifndef __GUILABEL_H__
#define __GUILABEL_H__

#include "GameUtil.h"
#include "GUIComponent.h"

class GUILabel : public GUIComponent
{
public:
    enum FontType {
		FONT_9_BY_15,// Use GLUT font for 9x15
		FONT_HELVETICA_18// Use GLUT font for Helvetica 18
    };

    GUILabel();
    GUILabel(const string& text, FontType font = FONT_9_BY_15);
    virtual ~GUILabel();
    virtual void Draw();
    void SetText(const string& text) { mText = text; }

protected:
    string mText;
    int mFontWidth;
    int mFontHeight;
    FontType mFont;
    void* mGlutFont; // Stores GLUT font pointer
};

#endif