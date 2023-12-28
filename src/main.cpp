#include <iostream>
#include "pch.hpp"
#include "utils.hpp"
#include "Batch.hpp"



const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#define MAX_BATCH_ELEMENTS  8192

SDL_Window *window;
SDL_GLContext context;
Texture2D texture;
RenderBatch batch;
Vector2 mousePosition;
int mouseButton;


bool m_shouldclose;
void Wait(float ms)
{
SDL_Delay((int)ms);
}
Uint32 GetTicks(void)
{
    return SDL_GetTicks();
}
double GetTime(void)
{
    return (double) SDL_GetTicks()/1000.0;
}
void Swap()
{
    SDL_GL_SwapWindow(window);
}

Vector2 GetMousePosition()
{
    return mousePosition;
} 

bool IsMouseDown(int button)
{
    return mouseButton & SDL_BUTTON(button);
}

void MouseUpdate()
{
    int x,y;
    mouseButton = SDL_GetMouseState(&x,&y) ;
    mousePosition.set(x,y);
}


class Bunny
{
public:
    Bunny()
    {
       

            position = GetMousePosition();
            speed.x = (float)Random_Float(-250, 250)/60.0f;
            speed.y = (float)Random_Float(-250, 250)/60.0f;
            color = (Color){ Random_Int(50, 240),
                                                       Random_Int(80, 240),
                                                       Random_Int(100, 240), 255 };
           
    }
    void Update()
    {
        position.x += speed.x;
        position.y += speed.y;
        if (position.x > SCR_WIDTH) speed.x *= -1;
        if (position.x < 0) speed.x *= -1;
        if (position.y > SCR_HEIGHT) speed.y *= -1;
        if (position.y < 0) speed.y *= -1;


        batch.DrawTexture(texture,position.x,position.y,color);

        
    }
    Vector2 position;
    Vector2 speed;
    Color color;
};

bool Run()
{


    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        
        switch (event.type)
        {
            case SDL_QUIT:
            {
                m_shouldclose = true;
                break;
            }
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym==SDLK_ESCAPE)
                {
                    m_shouldclose = true;
                    break;
                }
        
                break;
            }
            
            case SDL_KEYUP:
            {
            
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
        

            }break;
            case SDL_MOUSEBUTTONUP:
            {
                
                break;
            }
            case SDL_MOUSEMOTION:
            {

            break;   
            }
            
            case SDL_MOUSEWHEEL:
            {
            
            break;
            }
        }
    } 
    MouseUpdate();
    return !m_shouldclose;
}


int main()
{
 
       if (SDL_Init(SDL_INIT_VIDEO) < 0) 
        {
                Log(2,"SDL could not initialize! Error: %s", SDL_GetError());
            return 1;
        }
        m_shouldclose=false;

    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES)!=0)
    {
        SDL_Log( "ERROR loading context profile mask");
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)!=0)
    {
        SDL_Log("ERROR setting context major version");
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0)!=0)
    {
        SDL_Log("ERROR setting context minor version");
    }
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0){
        SDL_Log( " ERROR \n setting double buffer");
    } // I have tried without the dubble buffer and nothing changes
    if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE   , 24) != 0){
        SDL_Log( " ERROR \n setting depth buffer");
    }
    if(SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32)!=0){
        SDL_Log( "ERROR setting buffer size");
    }
    if(SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8)!=0){
        SDL_Log( " ERROR loading red");
    }
    if(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8)!=0){
        SDL_Log( " ERROR loading red");
    }if(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8)!=0){
        SDL_Log( " ERROR loading red");
    }if(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8)!=0){
        SDL_Log(" Error setting alpha");
    }

   if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) != 0){
        SDL_Log( " Error  setting stencil buffer");
    }



        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);



        window = SDL_CreateWindow("Batch", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
        if (!window) 
        {
            Log(2, "Window could not be created! SDL Error: %s" , SDL_GetError() );
            
            return false;
        }

    context = SDL_GL_CreateContext(window);
    if (!context) 
    {
        Log(2, "OpenGL context could not be created! %s ", SDL_GetError());
        SDL_DestroyWindow(window);
        return false; 
    }

    gladLoadGLES2Loader(SDL_GL_GetProcAddress);
    Log(0,"[DEVICE] Vendor  :  %s",glGetString(GL_VENDOR));
    Log(0,"[DEVICE] Renderer:  %s",glGetString(GL_RENDERER));
    Log(0,"[DEVICE] Version :  %s",glGetString(GL_VERSION));

    srand((unsigned int)SDL_GetTicks());              // Initialize random seed

  // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
    glCullFace(GL_BACK);                                    // Cull the back face (default)
    glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
    glEnable(GL_CULL_FACE);                                 // Enable backface culling

 
    Log(0, "GL: OpenGL device information:");
    Log(0, "    > Vendor:   %s", glGetString(GL_VENDOR));
    Log(0, "    > Renderer: %s", glGetString(GL_RENDERER));
    Log(0, "    > Version:  %s", glGetString(GL_VERSION));
    Log(0, "    > GLSL:     %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    
     SDL_GL_SetSwapInterval(0);

     
     batch.Init(12, MAX_BATCH_ELEMENTS);
     Matrix ortho;
     ortho.Ortho(0,SCR_WIDTH,SCR_HEIGHT,0,-1,1);
     
     batch.setMatrix(ortho);


     texture.Load("assets/wabbit_alpha.png");

    double lastTime = GetTime();
    int frameCount = 0;

    std::vector<Bunny> bunnies; 
    double fps = 0.0;

    //64500 30 8 calls

    while(Run())
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 


        //  batch.DrawLine(0,0,100,100,Color(255,0,0,255));
        //  batch.DrawRectangle(100,100,100,100,Color(0,255,0,255));

        //  batch.DrawCircleLines(200,400,50,Color(0,0,255,255));
        //   batch.DrawCircle(200,400,50,Color(0,0,255,255));

        //  batch.DrawTexture(texture,200,200,Color(255,255,255,255));

        // batch.DrawTexture(texture,300,400,Color(255,255,255,255));

        // batch.DrawTexture(texture,40,40,Color(255,255,255,255));
        // batch.DrawTexture(texture,80,40,Color(255,255,255,255));
        // batch.DrawTexture(texture,140,40,Color(255,255,255,255));

        if (IsMouseDown(1))
        {
             for (int i = 0; i < 100; i++)
            {
                bunnies.push_back(Bunny());
            }        
        }

        for (int i = 0; i < bunnies.size(); i++)
        {
            bunnies[i].Update();
        }



        batch.Render();


        

        double currentTime = GetTime();
        frameCount++;

        if (currentTime - lastTime >= 1.0)
         {
            fps = frameCount / (currentTime - lastTime);

            
        
            //SDL_SetWindowTitle(window,TextFormat("FPS: %i",GetFPS()));

            frameCount = 0;
            lastTime = currentTime;
        }


        Swap();

            int count = bunnies.size();
            int drawCall = 1 + count /MAX_BATCH_ELEMENTS;
            std::string title = "FPS: " + std::to_string(static_cast<int>(fps)) + " Bunnies: " + std::to_string(count) + " DrawCall: " + std::to_string(drawCall);
            SDL_SetWindowTitle(window, title.c_str());

    }   

    texture.Release();
    
    batch.Release();
    Log(0,"[DEVICE] Close and terminate .");
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}

