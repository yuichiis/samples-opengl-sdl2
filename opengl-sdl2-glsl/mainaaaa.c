#if _MSC_VER
#include <Windows.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>

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
static GLuint shading_program_id[4];
static GLint attrib_position;


GLuint compile_shader(GLenum type, GLsizei nsources, const char **sources)
{
    GLuint  shader;
    GLint   success, len;
    GLsizei i, srclens[nsources];

    for (i = 0; i < nsources; ++i) {
        srclens[i] = (GLsizei)strlen(sources[i]);
    }

    shader = glCreateShader(type);
    glShaderSource(shader, nsources, sources, srclens);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char *log;
            log = malloc(len);
            glGetShaderInfoLog(shader, len, NULL, log);
            fprintf(stderr, "%s\n\n", log);
            free(log);
        }
        SDL_Log("Error compiling shader.\n");
    }
    SDL_Log("shader: %u",shader);

    return shader;
}

GLuint program_check(GLuint program)
{
	//Error Checking
	GLint status;
	glValidateProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (!status){
		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		SDL_Log("log lenght: %d",len);
		if (len > 1){
			char *log;
			log = malloc(len);
			glGetProgramInfoLog(program, sizeof(log), &len, log);
			SDL_LogError(SDL_LOG_CATEGORY_ERROR,"%s\n\n", log);
			free(log);
		}
		glDeleteProgram(program);
		SDL_Log("Error linking shader default program.\n");
		return GL_FALSE;
	}

	return GL_TRUE;
}

GLuint default_vertex(void)
{
    GLuint vtx;
    const char *sources[] = {
        "#version 100\n"
        "precision mediump float;"
        "uniform float iTime;"
        "uniform vec2  iResolution;",

        "attribute vec4 position;"
        "void main(){"
            "gl_Position=position;"
    "}";

    };
    sources[0] = common_shader_header;
    sources[1] = vertex_shader_body;
    vtx = compile_shader(GL_VERTEX_SHADER, 2, sources);

    return vtx;
}

GLuint default_shaders(GLuint choice)
{
    SDL_Log("choice def: %d", choice);
    GLuint vtx;
    vtx = default_vertex();

    if (vtx==0) return 0;

    GLuint frag;
    const char *sources[4];

    sources[0] = common_shader_header;
    sources[1] = fragment_shader_header;
    switch(choice) {
        case 0:
            sources[2] = default_fragment_shader_0;
            break;
        case 1:
            sources[2] = default_fragment_shader_1;
            break;
        case 2:
            sources[2] = default_fragment_shader;
            break;
        default:
            // invalid choice number
            return 0;
    }
    sources[3] = fragment_shader_footer;

    frag = compile_shader(GL_FRAGMENT_SHADER, 4, sources);

    shading_program_id[choice] = glCreateProgram();
    glAttachShader(shading_program_id[choice], vtx);
    glAttachShader(shading_program_id[choice], frag);
    glLinkProgram(shading_program_id[choice]);

    //Error Checking
    GLuint status;
    status=program_check(shading_program_id[choice]);
    if (status==GL_FALSE) return 0;

    return shading_program_id[choice];
}

void initShader()
{
    SDL_Log("Trying to build default shaders");
    for (int i=0; i<3; i++){
        shading_program_id[i] = default_shaders(i);
        SDL_Log("i: %d", i);
    }
    if (shading_program_id[0] == 0){
        Running = 0;
        if (glGetError()!=0)
            SDL_Log("glError: %#08x\n", glGetError());
    }
    glUseProgram(shading_program_id[2]);
    glEnableVertexAttribArray    (attrib_position);
    glVertexAttribPointer        (attrib_position, 2, GL_FLOAT, GL_FALSE, 0, vertices);

    uniform_res   = glGetUniformLocation(shading_program_id[2], "iResolution");
    uniform_gtime = glGetUniformLocation(shading_program_id[2], "iTime");
    uniform_mouse = glGetUniformLocation(shading_program_id[2], "iMouse");

    glUniform3f(uniform_res, (float)ww, (float)wh, 0.0f);

}

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

    int maj;
    int min;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,   &maj);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,   &min);
    SDL_Log("Using OpenGL %d.%d", maj, min);

    SDL_Log(
        "\nGL_VERSION   : %s"
        "\nGL_VENDOR    : %s"
        "\nGL_RENDERER  : %s"
        "\nGLSL VERSION : %s"
        //"\nGL_EXTENSIONS  : %s"
        "\n",
        glGetString(GL_VERSION),
        glGetString(GL_VENDOR),
        glGetString(GL_RENDERER),
        glGetString(GL_SHADING_LANGUAGE_VERSION)
        //glGetString(GL_EXTENSIONS)
    );

    return 1;
}

static int finalize() {
    //if(Renderer) {
    //    SDL_DestroyRenderer(Renderer);
    //    Renderer = NULL;
    //}
    //
    //if(Window) {
    //    SDL_DestroyWindow(Window);
    //    Window = NULL;
    //}

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

