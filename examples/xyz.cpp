#include "iGraphics.h"

bool flag1 = false,flag2 = false;


#define BUTTON_COUNT 5

const char *menuItems[BUTTON_COUNT] = {
    "PLAY", "HIGH SCORE", "HELP", "CREDITS" ,"EXIT"
};

int menuYStart = 350;
int menuX = 550;
int menuWidth = 200;
int menuHeight = 40;
int menuSpacing = 20;

int hoveredIndex = -1;


Image mario,bg;
// Image loading[12]; // Image array for loading animation
 // Current frame of loading animation


void loadResources()
{
//     // Load background and character image
//     // iLoadImage(&bg, "assets/images/LP.png");
     

//     // // Resize background to window size
//     // iResizeImage(&bg, 800, 400);
    

//     // Resize Mario proportionally

//     // Load loading animation frames into Image objects
//     // char path[200];
//     // for (int i = 0; i < 12; i++)
//     // {
//     //     sprintf(path, "D:/Modern-iGraphics-main/Modern-iGraphics-main/assets/images/sprites/Loading/loading%03d.png", i);
//     //     iLoadImage(&loading[i], path);
//     //     iResizeImage(&loading[i], 64, 64); // Resize as needed
//     // }
}

void iDraw()
{
    if(flag1){
        iClear();
        iSetColor(255, 255, 0); // Yellow
        iText(500, 520, "Shooting Games", GLUT_BITMAP_TIMES_ROMAN_24);
        iLoadImage(&mario, "assets/images/menu.png");
        iResizeImage(&mario, 800, 450); 
        iShowLoadedImage(0, 0, &mario);
    // Draw menu buttons
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            int y = menuYStart - i * (menuHeight + menuSpacing);

            if (i == hoveredIndex)
                iSetColor(255, 255, 0); // Yellow when hovered
            else
                iSetColor(200, 200, 255); // Light blue when normal

            iText(menuX, y, menuItems[i], GLUT_BITMAP_HELVETICA_18);
        }
    }
    
    
    // iShowLoadedImage(0, 0, &bg);
    
    // iShowLoadedImage(368, 20, &loading[idle_idx]);
    if(flag2){
        iClear();
        iLoadImage(&bg, "assets/images/bg.png");
        iResizeImage(&bg, 800, 450); 
        iShowLoadedImage(0, 0, &bg);
    }
    
}

void iMouseMove(int mx, int my)
{
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

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (hoveredIndex != -1)
        {
            if (hoveredIndex == 0)
            {
                // PLAY clicked
                flag2 = true;
                flag1 = false;
                

            }
            else if (hoveredIndex == 5)
            {
                exit(0); // EXIT
            }
        }
    }
}

void iKeyboard(unsigned char key)
{
    if (key == 'q')
        exit(0);
}


void iSpecialKeyboard(unsigned char){}
void iMouseDrag(int mx, int my) {}
void iMouseWheel(int dir, int mx, int my) {}
void iMouseMovePassive(int mx, int my) {}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    flag1 = true;
    iInitialize(800, 450, "Shooting Game Menu");

    return 0;
}
