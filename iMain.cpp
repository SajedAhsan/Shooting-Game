#include "iGraphics.h"
#include "iSound.h"
#define BUTTON_COUNT 5
const char *menuItems[BUTTON_COUNT] = { "PLAY", "HIGH SCORE", "HELP", "CREDITS" ,"EXIT" };
int menuYStart = 350;
int menuX = 550;
int menuWidth = 150;
int menuHeight = 40;
int menuSpacing = 20;

int hoveredIndex = -1;

Image menu_background;


void loadResources()
{
    iLoadImage(&menu_background, "assets/images/menu.png");
    iResizeImage(&menu_background, 800, 450);
}

/*
function iDraw() is called again and again by the system.
*/

// Drawing buttons for game menu
void button_draw()
{
    for (int i = 0; i < BUTTON_COUNT; i++)
        {   
            // x coordinate is fixed
            // generating y coordinate for each button 
            int y = menuYStart - i * (menuHeight + menuSpacing);

            if (i == hoveredIndex)
                iSetColor(20, 250, 235); // when hovered
            else
                iSetColor(255, 255, 255); // when normal
            // showing button text
            iText(menuX, y, menuItems[i], GLUT_BITMAP_HELVETICA_18);
        }
}
void iDraw()
{
    // place your drawing codes here
    iClear();
    iShowLoadedImage(0, 0, &menu_background);
    button_draw();
    
    
}

/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    // place your codes here

    // hover button code
     hoveredIndex = -1;

    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        int y = menuYStart - i * (menuHeight + menuSpacing);
        if (mx >= menuX && mx <= menuX + menuWidth &&
            my >= y - 5 && my <= y + 20)
        {
            hoveredIndex = i;
            break;
        }
    }
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && hoveredIndex!=-1)
    {
        // place your codes here
        if (hoveredIndex != -1)
        {
            iPlaySound("assets/sounds/clicked.wav", false);
            if (hoveredIndex == 0)
            { 
                // PLAY clicked
            }
            else if (hoveredIndex == 1)
            {
                // High Score
            }else if(hoveredIndex == 2){
                // Help
            }else if(hoveredIndex == 3){
                // credit
            }else if(hoveredIndex == 4){
                exit(0);
            }
        }
        
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
}

/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    switch (key)
    {
    case 'q':
        // do something with 'q'
        break;
    // place your codes for other keys here
    default:
        break;
    }
}

/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction
keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */
void iSpecialKeyboard(unsigned char key)
{
    switch (key)
    {
    case GLUT_KEY_END:
        // do something
        break;
    // place your codes for other keys here
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    loadResources();
    // place your own initialization codes here.
    iInitializeSound();
    iInitialize(800, 450, "Game Menu");
    return 0;
}