#if _MSC_VER
#include <Windows.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <stdio.h>
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MIN
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#endif
#ifndef MAX
#define MAX(i, j) (((i) > (j)) ? (i) : (j))
#endif

const int FPS = 60;
static int windowWidth = 640;
static int windowHeight = 480;
static SDL_Window*   window = NULL;
static SDL_GLContext context;
static SDL_Renderer* renderer = NULL;
static SDL_Surface*  primarySurface = NULL;
static float rotation = 0.0;
static GLuint image_texture_id = 0;

static int init() {

    //// Init Window
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to Init SDL: %s", SDL_GetError());
        return 0;
    }

    // enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow(
        "My SDL Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight, 
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if(window == NULL) {
        printf("Unable to create SDL Window: %s", SDL_GetError());
        return 0;
    }
    context = SDL_GL_CreateContext(window);
    if (!context) {
        printf("Unable to Init GL Context: %s", SDL_GetError());
        return 0;
    }

    // setup viewport
    glViewport(0, 0, windowWidth, windowHeight); // Default
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glEnable(GL_DEPTH_TEST);

    // setup projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(45.0, (GLdouble) windowWidth / (GLdouble) windowHeight, 2.0, 200.0);

    // setup light
    //static GLfloat position[] = {-10.0f, 10.0f, 10.0f, 1.0f};
    //static GLfloat ambient [] = { 1.0f, 1.0f, 1.0f, 1.0f};
    //static GLfloat diffuse [] = { 1.0f, 1.0f, 1.0f, 1.0f};
    //static GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 0.0f};
    //glLightfv(GL_LIGHT0, GL_POSITION, position);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    //glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    return 1;
}

static int loadImage(const char* filename,GLuint *texture_id) {
    SDL_Surface* image;
    GLint img_fmt = GL_RGBA;
    image = IMG_Load(filename);
    if(!image) {
        printf("Failed to load image\n");
        return FALSE;
    }
    printf("image byte per pixel=%d\n",image->format->BytesPerPixel);
    if(image->format->BytesPerPixel==3) {
        img_fmt = GL_RGB;
    }

    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_2D, *texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 
        GL_RGBA, image->w, image->h, 0, 
        img_fmt, GL_UNSIGNED_BYTE, image->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(image);
    return TRUE;
}

static int finalize() {
	//if(Renderer) {
	//	SDL_DestroyRenderer(Renderer);
	//	Renderer = NULL;
	//}
    //
	//if(Window) {
	//	SDL_DestroyWindow(Window);
	//	Window = NULL;
	//}

    // finalize SDL_image
    IMG_Quit();
    // finalize SDL
    SDL_Quit();

    return TRUE;
}

static void resize(int width, int height) {
    if(windowWidth==width && windowHeight==height) {
        return;
    }
    printf("resize %d,%d\n",width,height);
    glViewport(0,0,width,height);
    windowWidth = width;
    windowHeight = height;
}

static void update() {
    rotation = rotation + 1.0f;
    if(rotation>=90.0) {
        rotation = 0.0;
    }
}

static void draw() {
    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup view
    float minviewf = (float)(MIN(windowWidth, windowHeight));
    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotatef(0.0,0.0,0.0,1.0);
    glScalef((minviewf/(float)windowWidth),(minviewf/(float)windowHeight),1.0);

    // draw
    glColor4f(0.0, 1.0,  1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f( -1.0 ,  -1.0);
    glVertex2f(  1.0 ,  -1.0);
    glVertex2f(  1.0 ,   1.0);
    glVertex2f( -1.0 ,   1.0);
    glEnd();

    glColor4f(0.0, 1.0,  0.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(  0.0 ,   0.0);
    glVertex2f(  1.0 ,   0.0);
    glVertex2f(  0.5 ,   1.0);
    glEnd();

    glColor4f(1.0, 0.0,  0.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(  0.0 ,   0.0);
    glVertex2f( -1.0 ,   0.0);
    glVertex2f( -0.5 ,  -1.0);
    glEnd();

    glColor4f(1.0, 1.0,  1.0, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex2f(  0.2f ,   0.2f);
    glVertex2f(  0.3f ,   0.2f);
    glEnd();
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex2f(  0.3f ,   0.2f);
    glVertex2f(  0.3f ,   0.3f);
    glEnd();
    glLineWidth(4.0);
    glBegin(GL_LINES);
    glVertex2f(  0.3f ,   0.3f);
    glVertex2f(  0.2f ,   0.3f);
    glEnd();
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glVertex2f(  0.2f ,   0.3f);
    glVertex2f(  0.2f ,   0.2f);
    glEnd();
    glLineWidth(1.0);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00ff);
    glBegin(GL_LINES);
    glVertex2f(  0.1f ,   0.06f);
    glVertex2f(  0.5f ,   0.06f);
    glEnd();
    glLineStipple(1, 0x0f0f);
    glBegin(GL_LINES);
    glVertex2f(  0.1f ,   0.09f);
    glVertex2f(  0.5f ,   0.09f);
    glEnd();
    glLineStipple(1, 0x5555);
    glBegin(GL_LINES);
    glVertex2f(  0.1f ,   0.12f);
    glVertex2f(  0.5f ,   0.12f);
    glEnd();
    glLineStipple(1, 0x18ff);
    glBegin(GL_LINES);
    glVertex2f(  0.1f ,   0.15f);
    glVertex2f(  0.5f ,   0.15f);
    glEnd();
    glColor4f(1.0, 1.0,  0.0, 1.0);
    glLineStipple(2, 0x00ff);
    glBegin(GL_LINES);
    glVertex2f(  0.5f ,   0.06f);
    glVertex2f(  0.9f ,   0.06f);
    glEnd();
    glLineStipple(2, 0x0f0f);
    glBegin(GL_LINES);
    glVertex2f(  0.5f ,   0.09f);
    glVertex2f(  0.9f ,   0.09f);
    glEnd();
    glLineStipple(2, 0x5555);
    glBegin(GL_LINES);
    glVertex2f(  0.5f ,   0.12f);
    glVertex2f(  0.9f ,   0.12f);
    glEnd();
    glLineStipple(2, 0x18ff);
    glBegin(GL_LINES);
    glVertex2f(  0.5f ,   0.15f);
    glVertex2f(  0.9f ,   0.15f);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    glLineWidth(1.0);
    glPushMatrix();
    glTranslatef(-0.5,0.5,0.0);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glScalef(0.5,0.5,0.5);
    glColor4f(0.0, 0.0,  1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f( -0.5f ,  -0.5f);
    glVertex2f(  0.5f ,  -0.5f);
    glVertex2f(  0.5f ,   0.5f);
    glVertex2f( -0.5f ,   0.5f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5,-0.5,0.0);
    glRotatef(45.0, 0.0, 0.0, 1.0);
    glScalef(0.5,0.5,0.5);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, image_texture_id);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 1.0);
    glVertex2f( -0.5f ,  -0.5f);
    glTexCoord2d(1.0, 1.0);
    glVertex2f(  0.5f ,  -0.5f);
    glTexCoord2d(1.0, 0.0);
    glVertex2f(  0.5f ,   0.5f);
    glTexCoord2d(0.0, 0.0);
    glVertex2f( -0.5f ,   0.5f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor4f(0.0, 1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f( -0.5f ,  -0.5f);
    glVertex2f(  0.5f ,  -0.5f);
    glVertex2f(  0.5f ,   0.5f);
    glVertex2f( -0.5f ,   0.5f);
    glEnd();
    glPopMatrix();

    // pop view matrix
    glPopMatrix();
}

// polling event and execute actions
static int pollingEvent()
{
    SDL_Event ev;
    while ( SDL_PollEvent(&ev) )
    {
        switch(ev.type){
            case SDL_QUIT: {
                // raise when exit event is occur
                printf("QUIT\n");
                return FALSE;
            }
            case SDL_KEYDOWN: {
                SDL_Keycode key;
                printf(".");
                // raise when key down
                key = ev.key.keysym.sym;
                // ESC
                if(key == SDLK_ESCAPE){
                    printf("ESC\n");
                    return FALSE;
                }
                break;
            }
            case SDL_WINDOWEVENT: {
                SDL_WindowEventID event;
                event = (SDL_WindowEventID)ev.window.event;
                if(event == SDL_WINDOWEVENT_RESIZED) {
                    resize(ev.window.data1,ev.window.data2);
                }
            }
        }
    }
    return TRUE;
}

int main(int argc, char *argv[])
{
    static Uint64 interval;
    static Uint64 nextTime;
    if(!init()) {
        return 0;
    }
    if(!loadImage("testimg.png", &image_texture_id)) {
        return 0;
    }
    printf("Exit with ESC key.\n");

    // mainloop
    interval = 1000 / FPS;
    nextTime = SDL_GetTicks64() + interval;
    int skipDraw = FALSE;

    while (TRUE) {
        // check event
        if (!pollingEvent()) {
            break;
        }

        // update and draw
        update();
        if (!skipDraw) {
            draw();
            SDL_GL_SwapWindow(window);
        }

        int delayTime = (int)(nextTime - SDL_GetTicks64());
        if (delayTime > 0) {
            SDL_Delay(delayTime);
            skipDraw = FALSE;
        } else {
            // skip next draw step because of no time
            skipDraw = TRUE;
        }

        nextTime += interval;
    }

    // finalize
    finalize();
  
    return 0;
}

