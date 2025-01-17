#if _MSC_VER
#include <Windows.h>
#endif
#include <SDL.h>
//#include <SDL_opengl.h>
#include <GL/glew.h>

#include <math.h>
#include <stdio.h>
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


typedef struct _program {
    GLuint vtx;
    GLuint frag;
    GLuint id;
} program_t;

const int SUCCESS = 1;
const int FAILED = 0;

const int FPS = 60;
static int windowWidth = 640;
static int windowHeight = 480;
static SDL_Window*   window = NULL;
static SDL_GLContext context;
static SDL_Renderer* renderer = NULL;
static SDL_Surface*  primarySurface = NULL;
static float rotation = 0.0;
static program_t shadingProgram = { 0, 0, 0 };

// vertices
const GLint location_position = 0;
static GLenum drawMode = GL_TRIANGLE_STRIP;
static GLfloat vertices[] = {
    -1.0f,   -1.0f,
     1.0f,   -1.0f,
     0.0f,    1.0f,
};
static GLsizei nvertices = 0;

// transformation matrices
static GLfloat model[4][4];
static GLfloat view[4][4];
static GLfloat projection[4][4];

// object names
static GLint buffer_vertices;
static GLint uniform_res;
static GLint uniform_model;
static GLint uniform_view;
static GLint uniform_projection;

GLuint compile_shader(GLenum type, GLsizei nsources, const char **sources)
{
    GLuint  shader;
    GLint   success, len;
    GLsizei i, *srclens;

    srclens = malloc(sizeof(GLsizei)*nsources);
    for (i = 0; i < nsources; ++i) {
        srclens[i] = (GLsizei)strlen(sources[i]);
    }
    SDL_Log("done setuping source length.");

    shader = glCreateShader(type);
    SDL_Log("shader is created.");
    glShaderSource(shader, nsources, sources, srclens);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	free(srclens);

    if (!success) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char *log;
            log = malloc(len);
            glGetShaderInfoLog(shader, len, NULL, log);
            SDL_Log("%s\n\n", log);
            free(log);
        }
        SDL_Log("Error compiling shader.\n");
        glDeleteShader(shader);
        return 0;
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

program_t load_shaders(
    const GLsizei n_vtx, const char **vtx_sources,
    const GLsizei n_frag, const char **frag_sources
    )
{
    GLuint vtx;
    GLuint frag;
    GLuint program;
    program_t shader_program = { 0, 0, 0 };

    SDL_Log("Trying to compile vertex shader");
    vtx = compile_shader(GL_VERTEX_SHADER, 1, vtx_sources);
    if(vtx==0) {
        SDL_Log("Compile Error in vertex shader");
        return shader_program;
    }

    SDL_Log("Trying to compile fragment shader");
    frag = compile_shader(GL_FRAGMENT_SHADER, 1, frag_sources);
    if(frag==0) {
        SDL_Log("Compile Error in fragment shader");
        glDeleteShader(vtx);
        return shader_program;
    }

    program = glCreateProgram();
    glAttachShader(program, vtx);
    glAttachShader(program, frag);
    glLinkProgram(program);

    //Error Checking
    GLuint status;
    status=program_check(program);
    if(status==GL_FALSE) {
        glDeleteShader(vtx);
        glDeleteShader(frag);
        return shader_program;
    }

    shader_program.vtx = vtx;
    shader_program.frag = frag;
    shader_program.id = program;

    return shader_program;
}


static void createTransformationMatrix(
    GLfloat angle,
    GLfloat tx,
    GLfloat ty,
    GLfloat tz,
    GLfloat matrix[4][4]
    ) {
    GLfloat radians = angle * (GLfloat)M_PI / 180.0f; // from degree to radian
    GLfloat cosTheta = cosf(radians);
    GLfloat sinTheta = sinf(radians);

    // A transformation matrix that combines a rotation around the Z axis and a translation
    matrix[0][0] = cosTheta;
    matrix[0][1] = -sinTheta;
    matrix[0][2] = 0.0;
    matrix[0][3] = tx;
    matrix[1][0] = sinTheta;
    matrix[1][1] = cosTheta;
    matrix[1][2] = 0.0;
    matrix[1][3] = ty;
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = 1.0;
    matrix[2][3] = tz;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
}

static int init() {

    //// Init Window
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to Init SDL: %s", SDL_GetError());
        return FAILED;
    }

    // enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow(
        "My SDL Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight, 
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if(window == NULL) {
        SDL_Log("Unable to create SDL Window: %s", SDL_GetError());
        return FAILED;
    }
    context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_Log("Unable to Init GL Context: %s", SDL_GetError());
        return FAILED;
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

    GLenum status;
    status = glewInit();
    
    if (status != GLEW_OK){
        SDL_Log("glewInit error: %s\n", glewGetErrorString (status));
        return FAILED;
    }

    return SUCCESS;
}

static int finalize() {
    if(glIsProgram(shadingProgram.id)) {
        glDeleteProgram(shadingProgram.id);
    }
    if(glIsShader(shadingProgram.vtx)) {
        glDeleteShader(shadingProgram.vtx);
    }
    if(glIsShader(shadingProgram.frag)) {
        glDeleteShader(shadingProgram.frag);
    }
    glDeleteBuffers(1, &buffer_vertices);

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

static int initShader()
{
    const char *vtx_sources[] = {
        "#version 100\n"
        "precision mediump float;\n"
        "attribute vec4 position;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "void main(){\n"
            "gl_Position=projection*view*model*position;\n"
        "}\n"
    };

    const char *frag_sources[] = {
        "#version 100\n"
        "precision mediump float;\n"
        "uniform vec2  resolution;\n"
        "void main(){\n"
            "float red = abs(gl_FragCoord.x/resolution.x);\n"
            "gl_FragColor=vec4(red, 0.0, 0.0, 0.0);\n"
        "}\n"
    };

    SDL_Log("Trying to build default shaders");

    program_t program = load_shaders(1, vtx_sources, 1, frag_sources);
    SDL_Log("Program ID: %d",program.id);

    if(program.id == 0){
        if(glGetError()!=0) {
            SDL_Log("glError: %#08x\n", glGetError());
        }
        return FAILED;
    }
    // save shared shadingProgram variable
    shadingProgram = program;

    // use shader program
    glUseProgram(program.id);

    // bind vertex array
    glGenBuffers(1, &buffer_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // GLAPI void APIENTRY glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
    glVertexAttribPointer(
        location_position,  // location of position attribute
        2,                  // size = vec2
        GL_FLOAT,           // type float
        GL_FALSE,           // normalized
        0,                  // stride:   0: default = size
        (void*)0            // vertex offset address
    );
    glEnableVertexAttribArray(location_position);
    nvertices = sizeof(vertices)/sizeof(float);

    // setup uniforms
    uniform_res = glGetUniformLocation(program.id, "resolution");
    uniform_model = glGetUniformLocation(program.id, "model");
    uniform_view = glGetUniformLocation(program.id, "view");
    uniform_projection = glGetUniformLocation(program.id, "projection");

    // ininitialize uniform values
    glUniform2f(uniform_res, (float)windowWidth, (float)windowHeight);

    //                          rotate, move_x, move_y, move_z, matrix 
    createTransformationMatrix(    0.0,    0.0,    0.0,    0.0, model);
    createTransformationMatrix(    0.0,    0.0,    0.0,    0.0, view);
    createTransformationMatrix(    0.0,    0.0,    0.0,    0.0, projection);
    // void glUniformMatrix4fv( GLint location,GLsizei count,GLboolean transpose,const GLfloat *value);
    glUniformMatrix4fv(uniform_model, 1, GL_FALSE, (const GLfloat*)model);
    glUniformMatrix4fv(uniform_view, 1, GL_FALSE, (const GLfloat*)view);
    glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, (const GLfloat*)projection);

    return SUCCESS;
}

static void resize(int width, int height) {
    if(windowWidth==width && windowHeight==height) {
        return;
    }
    windowWidth = width;
    windowHeight = height;
    SDL_Log("resize %d,%d\n",width,height);
    glViewport(0,0,width,height);
    glUniform2f(uniform_res, (float)windowWidth, (float)windowHeight);
}

static void update() {
    rotation = rotation + 1.0f;
    if(rotation>=360.0) {
        rotation = 0.0;
    }
    //                            rotate, move_x, move_y, move_z, matrix 
    createTransformationMatrix( rotation,    0.0,    0.0,    0.0, model);
    // void glUniformMatrix4fv( GLint location,GLsizei count,GLboolean transpose,const GLfloat *value);
    glUniformMatrix4fv(uniform_model, 1, GL_FALSE, (const GLfloat*)model);

}

static void draw() {
    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw from array
    // void glDrawArrays(GLenum mode, GLint first, GLsizei count)
    glDrawArrays(drawMode, 0, nvertices);
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
                SDL_Log("QUIT\n");
                return FALSE;
            }
            case SDL_KEYDOWN: {
                SDL_Keycode key;
                SDL_Log(".");
                // raise when key down
                key = ev.key.keysym.sym;
                // ESC
                if(key == SDLK_ESCAPE){
                    SDL_Log("ESC\n");
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
    if(!initShader()) {
        return 0;
    }

    SDL_Log("Exit with ESC key.\n");

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

