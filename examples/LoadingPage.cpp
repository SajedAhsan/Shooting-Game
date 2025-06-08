#include "iGraphics.h"

Image bg, mario;
Image loading[12]; // Image array for loading animation
int idle_idx = 0;  // Current frame of loading animation

void loadResources()
{
    // Load background and character image
    iLoadImage(&bg, "assets/images/LP.png");
    iLoadImage(&mario, "assets/images/LP.png");

    // Resize background to window size
    iResizeImage(&bg, 800, 400);

    // Resize Mario proportionally
    double marioAspectRatio = (double)mario.width / mario.height;
    iResizeImage(&mario, 48, 48.0 / marioAspectRatio);

    // Load loading animation frames into Image objects
    char path[200];
    for (int i = 0; i < 12; i++)
    {
        sprintf(path, "D:/Modern-iGraphics-main/Modern-iGraphics-main/assets/images/sprites/Loading/loading%03d.png", i);
        iLoadImage(&loading[i], path);
        iResizeImage(&loading[i], 64, 64); // Resize as needed
    }
}

void update_monster()
{
    idle_idx = (idle_idx + 1) % 12;
}
void iMouseMove(int mx, int my)
{
	// place your code here
	printf("x = %d, y = %d\n", mx, my);
}

void iDraw()
{
    iClear();

    iShowLoadedImage(0, 0, &bg);

    // iShowLoadedImage(225, 225, &mario); // Optional Mario

    // Display current loading frame
    iShowLoadedImage(368, 20, &loading[idle_idx]);
}

void iMouseDrag(int mx, int my) {}
// void iMouseMove(int mx, int my) {}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        iMirrorImage(&mario, HORIZONTAL);
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        iMirrorImage(&mario, VERTICAL);
    }
}

void iMouseWheel(int dir, int mx, int my) {}
void iKeyboard(unsigned char key) {}

void iSpecialKeyboard(unsigned char key)
{
    if (key == GLUT_KEY_END)
    {
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    loadResources();
    iSetTimer(100, update_monster);
    iInitialize(800, 400, "Loading...");
     // Start animation timer
    return 0;
}
