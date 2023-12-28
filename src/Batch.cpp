#include "Batch.hpp"
#include "utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"         // Required for: stbi_load_from_file()
                                            // NOTE: Used to read image data (multiple formats support)


#define BATCH_BUFFER_ELEMENTS   8192    // Default internal render batch elements limits
#define BATCH_BUFFERS              1    // Default number of batch buffers (multi-buffering)
#define BATCH_DRAWCALLS          256    // Default number of batch draw calls (by state changes: mode, texture)
#define MAX_MATRIX_STACK_SIZE             32    // Maximum size of internal Matrix stack
 #define SMOOTH_CIRCLE_ERROR_RATE    0.5f

const Color Color::Black(0, 0, 0, 255);
const Color Color::White(255, 255, 255, 255);
const Color Color::Red(255, 0, 0, 255);
const Color Color::Green(0, 255, 0, 255);
const Color Color::Blue(0, 0, 255, 255);
const Color Color::Yellow(255, 255, 0, 255);
const Color Color::Orange(255, 127, 0, 255);
const Color Color::Pink(255, 0, 255, 255);
const Color Color::Magenta(255, 0, 255, 255);
const Color Color::SkyBlue(0, 255, 255, 255);
const Color Color::Lime(50, 205, 50, 255);
const Color Color::Brown(139, 69, 19, 255);
const Color Color::Gold(255, 215, 0, 255);
const Color Color::Violet(238, 130, 238, 255);
const Color Color::Purple(128, 0, 128, 255);
const Color Color::Beige(245, 245, 220, 255);
const Color Color::Transparent(0, 0, 0, 0);





unsigned int CompileShader(const char *shaderCode, int type)
{
    unsigned int shader = 0;


    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);

    GLint success = 0;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        switch (type)
        {
            case GL_VERTEX_SHADER: Log(2, "SHADER: [ID %i] Failed to compile vertex shader code", shader); break;
            case GL_FRAGMENT_SHADER: Log(2, "SHADER: [ID %i] Failed to compile fragment shader code", shader); break;
            default: break;
        }

        int maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        if (maxLength > 0)
        {
            int length = 0;
            char *log = (char *)std::calloc(maxLength, sizeof(char));
            glGetShaderInfoLog(shader, maxLength, &length, log);
            Log(2, "SHADER: [ID %i] Compile error: %s", shader, log);
            std::free(log);
        }
    }
    else
    {
        switch (type)
        {
            case GL_VERTEX_SHADER: Log(0, "SHADER: [ID %i] Vertex shader compiled successfully", shader); break;
            case GL_FRAGMENT_SHADER: Log(0, "SHADER: [ID %i] Fragment shader compiled successfully", shader); break;
            default: break;
        }
    }
    return shader;
}

unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId)
{
    unsigned int program = 0;
    GLint success = 0;
    program = glCreateProgram();
    glAttachShader(program, vShaderId);
    glAttachShader(program, fShaderId);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
       Log(2,"SHADER: [ID %i] Failed to link shader program", program);

        int maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        if (maxLength > 0)
        {
            int length = 0;
            char *log = (char *)std::calloc(maxLength, sizeof(char));
            glGetProgramInfoLog(program, maxLength, &length, log);
            Log(2, "SHADER: [ID %i] Link error: %s", program, log);
            std::free(log);
        }

        glDeleteProgram(program);

        program = 0;
    }
    else
    {
        
        Log(0, "SHADER: [ID %i] Program shader loaded successfully", program);
    }
    return program;
}

bool Shader::Load(const char *vsFileName, const char *fsFileName)
{
    bool result = false;
    unsigned int vShaderId = 0;
    unsigned int fShaderId = 0;

    char *vShaderStr = NULL;
    char *fShaderStr = NULL;
    vShaderStr = LoadFileText(vsFileName);
    fShaderStr = LoadFileText(fsFileName);
    if (vShaderStr != NULL && fShaderStr != NULL)
    {
        vShaderId = CompileShader(vShaderStr, GL_VERTEX_SHADER);
        fShaderId = CompileShader(fShaderStr, GL_FRAGMENT_SHADER);
        if (vShaderId != 0 && fShaderId != 0)
        {
            id = LoadShaderProgram(vShaderId, fShaderId);
            if (id != 0)
            {
                result = true;
                glDeleteShader(vShaderId);
                glDeleteShader(fShaderId);
            }
        }
    }
    if (vShaderStr != NULL) std::free(vShaderStr);
    if (fShaderStr != NULL) std::free(fShaderStr);
    return result;
    
}

bool Shader::Create(const char *vShaderStr, const char *fShaderStr)
{
    
     bool result = false;
    unsigned int vShaderId = 0;
    unsigned int fShaderId = 0;

    vShaderId = CompileShader(vShaderStr, GL_VERTEX_SHADER);
    fShaderId = CompileShader(fShaderStr, GL_FRAGMENT_SHADER);
    if (vShaderId != 0 && fShaderId != 0)
    {
        id = LoadShaderProgram(vShaderId, fShaderId);
        if (id != 0)
        {
            result = true;
            glDeleteShader(vShaderId);
            glDeleteShader(fShaderId);
        }
    }

    return result;
}

void Shader::Set()
{
    glUseProgram(id);
}

void Shader::Reset()
{
    glUseProgram(0);
}



void GetTextureFormats(PixelFormat format, unsigned int *glInternalFormat, unsigned int *glFormat, unsigned int *glType)
{
    *glInternalFormat = 0;
    *glFormat = 0;
    *glType = 0;

    switch (format)
    {
        case PixelFormat::GRAYSCALE: *glInternalFormat = GL_LUMINANCE; *glFormat = GL_LUMINANCE; *glType = GL_UNSIGNED_BYTE; break;
        case PixelFormat::GRAY_ALPHA: *glInternalFormat = GL_LUMINANCE_ALPHA; *glFormat = GL_LUMINANCE_ALPHA; *glType = GL_UNSIGNED_BYTE; break;
        case PixelFormat::R8G8B8: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case PixelFormat::R8G8B8A8: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
     
    }
}

unsigned int LoadTexture(const void *data, int width, int height, PixelFormat format)
{
    unsigned int id = 0;

    glBindTexture(GL_TEXTURE_2D, 0);    

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &id);          

    glBindTexture(GL_TEXTURE_2D, id);


    unsigned int glInternalFormat, glFormat, glType;
    GetTextureFormats(format, &glInternalFormat, &glFormat, &glType);
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glFormat, glType, data);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR


    glBindTexture(GL_TEXTURE_2D, 0);

    if (id > 0) Log(0, "TEXTURE: [ID %i] Texture loaded successfully (%ix%i) ", id, width, height);
    else Log(2,"TEXTURE: Failed to load texture");

    return id;
}


// Render batch management
//------------------------------------------------------------------------------------------------
RenderBatch::RenderBatch()
{
    
}
void RenderBatch::Init(int numBuffers, int bufferElements)
{

    

const char *defaultVShaderCode =
    "#version 320 es                       \n"
     "precision mediump float;           \n"
    "in vec3 vertexPosition;            \n"
    "in vec2 vertexTexCoord;            \n"
    "in vec4 vertexColor;               \n"
    "out vec2 fragTexCoord;             \n"
    "out vec4 fragColor;                \n"
    "uniform mat4 mvp;                  \n"
    "void main()                        \n"
    "{                                  \n"
    "    fragTexCoord = vertexTexCoord; \n"
    "    fragColor = vertexColor;       \n"
    "    gl_Position = mvp*vec4(vertexPosition, 1.0); \n"
    "}                                  \n";


    const char *defaultFShaderCode =
    "#version 320 es      \n"
    "precision mediump float;           \n"
    "in vec2 fragTexCoord;              \n"
    "in vec4 fragColor;                 \n"
    "out vec4 finalColor;               \n"
    "uniform sampler2D texture0;        \n"
    "void main()                        \n"
    "{                                  \n"
    "    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
    "    finalColor = texelColor*fragColor;        \n"
    "}                                  \n";

    unsigned int vShaderId = 0;
    unsigned int fShaderId = 0;

    vShaderId = CompileShader(defaultVShaderCode, GL_VERTEX_SHADER);
    fShaderId = CompileShader(defaultFShaderCode, GL_FRAGMENT_SHADER);
    if (vShaderId != 0 && fShaderId != 0)
    {
        defaultShaderId = LoadShaderProgram(vShaderId, fShaderId);
        if (defaultShaderId != 0)
        {
            Log(0, "SHADER: [ID %i] Default shader loaded successfully", defaultShaderId);

             glDeleteShader(vShaderId);
             glDeleteShader(fShaderId);
        }
    } else 
    {
        Log(2,"SHADER: Failed to load default shader");
        return ;
    }

    mpvId = glGetUniformLocation(defaultShaderId, "mvp");
    textId = glGetUniformLocation(defaultShaderId, "texture0");

    matrix.Ortho(0, 800, 600, 0, -0.1f, 1.0f);

    GLfloat mat[16]=
            { matrix.m0, matrix.m1, matrix.m2, matrix.m3,
                    matrix.m4, matrix.m5, matrix.m6, matrix.m7,
                    matrix.m8, matrix.m9, matrix.m10, matrix.m11,
                    matrix.m12, matrix.m13, matrix.m14, matrix.m15
            };
    glUniformMatrix4fv(mpvId, 1, false, mat);
    glUniform1i(textId, 0);

    unsigned char pixels[4] = { 255, 255, 255, 255 };  
    defaultTextureId = LoadTexture(pixels, 1, 1, PixelFormat::R8G8B8A8);

    if (defaultTextureId != 0) Log(0, "TEXTURE: [ID %i] Default texture loaded successfully", defaultTextureId);



    for (int i=0;i<numBuffers;i++)
    {
        vertexBuffer.push_back(new VertexBuffer());
        
    }

      vertexCounter = 0;
      currentBuffer = 0;
    

    for (int i = 0; i < numBuffers; i++)
    {
        vertexBuffer[i]->elementCount = bufferElements;

        int k = 0;


        for (int j = 0; j <= bufferElements; j ++)
        {
            
            vertexBuffer[i]->vertices.push_back(Vertex());
            vertexBuffer[i]->vertices.push_back(Vertex());
            vertexBuffer[i]->vertices.push_back(Vertex());
            vertexBuffer[i]->vertices.push_back(Vertex());
          

            


            vertexBuffer[i]->indices.push_back(k);
            vertexBuffer[i]->indices.push_back(k + 1);
            vertexBuffer[i]->indices.push_back(k + 2);
            vertexBuffer[i]->indices.push_back(k);
            vertexBuffer[i]->indices.push_back(k + 2);
            vertexBuffer[i]->indices.push_back(k + 3);
            

            k+=4;
        }

      
    }

  
    for (int i = 0; i <numBuffers; i++)
    {
        glGenVertexArrays(1,&vertexBuffer[i]->vaoId);
        glBindVertexArray(vertexBuffer[i]->vaoId);

        glGenBuffers(1, &vertexBuffer[i]->vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[i]->vboId);
        glBufferData(GL_ARRAY_BUFFER, vertexBuffer[i]->vertices.size()*sizeof(Vertex), vertexBuffer[i]->vertices.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texcoord)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));



        glGenBuffers(1, &vertexBuffer[i]->iboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer[i]->iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,  vertexBuffer[i]->indices.size() * sizeof(unsigned int), vertexBuffer[i]->indices.data(), GL_STATIC_DRAW);
    }


    
     glBindVertexArray(0);
    
    



    for (int i = 0; i < BATCH_DRAWCALLS; i++)
    {
        draws.push_back(new DrawCall());
        draws[i]->mode = QUADS;
        draws[i]->vertexCount = 0;
        draws[i]->vertexAlignment = 0;
        draws[i]->textureId =defaultTextureId;
    }

    bufferCount = numBuffers;    // Record buffer count
    drawCounter = 1;             // Reset draws counter
    currentDepth = -1.0f;         // Reset depth value
    
}


void UnloadVertexArray(unsigned int vaoId)
{
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vaoId);

}


void UnloadVertexBuffer(unsigned int vboId)
{
    glDeleteBuffers(1, &vboId);
}

void UnloadTexture(unsigned int id)
{
    glDeleteTextures(1, &id);
    Log(0, "TEXTURE: [ID %i] Unloaded texture data from VRAM (GPU)", id);
}
void RenderBatch::Release()
{
    if (vertexBuffer.size() == 0) return;
    for (int i = 0; i < (int)vertexBuffer.size(); i++)
    {
        UnloadVertexBuffer(vertexBuffer[i]->vboId);
        UnloadVertexBuffer(vertexBuffer[i]->iboId);
        UnloadVertexArray(vertexBuffer[i]->vaoId);
    }
    for (int i = 0; i < (int)draws.size(); i++)
    {
        SAFE_DELETE(draws[i]);
    }
    for (int i = 0; i < (int)vertexBuffer.size(); i++)
    {
        vertexBuffer[i]->vertices.clear();
        vertexBuffer[i]->indices.clear();
        SAFE_DELETE(vertexBuffer[i]);
    }
    vertexBuffer.clear();
    UnloadTexture(defaultTextureId);
    glDeleteProgram(defaultShaderId);
    Log(0, "Render batch vertex buffers unloaded successfully from VRAM (GPU)");
}

RenderBatch::~RenderBatch()
{
    Release();
}

void RenderBatch::Render()
{
        if (vertexCounter > 0)
        {
            glBindVertexArray(vertexBuffer[currentBuffer]->vaoId);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[currentBuffer]->vboId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCounter * sizeof(Vertex), vertexBuffer[currentBuffer]->vertices.data());
            glBindVertexArray(0);
        }
        if (vertexCounter > 0)
        {

            glUseProgram(defaultShaderId);
            GLfloat mat[16]=
            {
                    matrix.m0, matrix.m1, matrix.m2, matrix.m3,
                    matrix.m4, matrix.m5, matrix.m6, matrix.m7,
                    matrix.m8, matrix.m9, matrix.m10, matrix.m11,
                    matrix.m12, matrix.m13, matrix.m14, matrix.m15
            };


            glUniformMatrix4fv(mpvId, 1, false, mat);
            glUniform1i(textId, 0);
            
            glBindVertexArray(vertexBuffer[currentBuffer]->vaoId);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer[currentBuffer]->iboId);

            glActiveTexture(GL_TEXTURE0);

         //   Log(0,"draw counter %d vertex %d %d ",drawCounter,vertexCounter, draws[0]->vertexCount/4*6);   

            for (int i = 0, vertexOffset = 0; i < drawCounter; i++)
            {

                glBindTexture(GL_TEXTURE_2D, draws[i]->textureId);

                int mode =GL_LINES;
                if (draws[i]->mode == LINES) mode = GL_LINES;
                else if (draws[i]->mode == TRIANGLES) mode = GL_TRIANGLES;
                else if (draws[i]->mode == QUADS) mode = GL_TRIANGLES;
             

               if ((draws[i]->mode == LINES) || (draws[i]->mode == TRIANGLES)) glDrawArrays(mode, vertexOffset, draws[i]->vertexCount);
               else
               {
                      glDrawElements(GL_TRIANGLES, draws[i]->vertexCount/4*6, GL_UNSIGNED_INT,(GLvoid *)(vertexOffset/4*6*sizeof(unsigned int)));
               }

               vertexOffset += (draws[i]->vertexCount + draws[i]->vertexAlignment);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
        }

    glBindVertexArray(0); // Unbind VAO
    glUseProgram(0);    // Unbind shader program
    vertexCounter = 0;
    currentDepth = -1.0f;
    for (int i = 0; i < BATCH_DRAWCALLS; i++)
    {
        draws[i]->mode =QUADS;
        draws[i]->vertexCount = 0;
        draws[i]->textureId = defaultTextureId;
    }
    drawCounter = 1;
    currentBuffer++;
    if (currentBuffer >= bufferCount) currentBuffer = 0;

}

bool RenderBatch::CheckRenderBatchLimit(int vCount)
{
    bool overflow = false;


    if ((vertexCounter + vCount) >=    (vertexBuffer[currentBuffer]->elementCount*4))
    {
        overflow = true;
        int currentMode = draws[drawCounter - 1]->mode;
        int currentTexture = draws[drawCounter - 1]->textureId;

        Render();
        draws[drawCounter - 1]->mode = currentMode;
        draws[drawCounter - 1]->textureId = currentTexture;
    }


    return overflow;
}


void RenderBatch::Begin(int mode)
{
    if (draws[drawCounter - 1]->mode != mode)
    {
        if (draws[drawCounter - 1]->vertexCount > 0)
        {
            if (draws[drawCounter - 1]->mode == LINES) draws[drawCounter - 1]->vertexAlignment = ((draws[drawCounter - 1]->vertexCount < 4)? draws[drawCounter - 1]->vertexCount : draws[drawCounter - 1]->vertexCount%4);
            else if (draws[drawCounter - 1]->mode == TRIANGLES) draws[drawCounter - 1]->vertexAlignment = ((draws[drawCounter - 1]->vertexCount < 4)? 1 : (4 - (draws[drawCounter - 1]->vertexCount%4)));
            else draws[drawCounter - 1]->vertexAlignment = 0;

            if (!CheckRenderBatchLimit(draws[drawCounter - 1]->vertexAlignment))
            {
                vertexCounter += draws[drawCounter - 1]->vertexAlignment;
                drawCounter++;
            }
        }

        if (drawCounter >= BATCH_DRAWCALLS) Render();

        draws[drawCounter - 1]->mode = mode;
        draws[drawCounter - 1]->vertexCount = 0;
        draws[drawCounter - 1]->textureId = defaultTextureId;
    }
}



void RenderBatch::Vertex3f(float x, float y, float z)
{
    float tx = x;
    float ty = y;
    float tz = z;


    // if (transformRequired)
    // {
    //     tx = transform.m0*x + transform.m4*y + transform.m8*z + transform.m12;
    //     ty = transform.m1*x + transform.m5*y + transform.m9*z + transform.m13;
    //     tz = transform.m2*x + transform.m6*y + transform.m10*z + transform.m14;
    // }
  //  Log(0," vertex %d  buffer %d",vertexCounter,currentBuffer);
    
    if (vertexCounter > (vertexBuffer[currentBuffer]->elementCount*4 - 4))
    {
        if ((draws[drawCounter - 1]->mode == LINES) &&            (draws[drawCounter - 1]->vertexCount%2 == 0))
        {
            CheckRenderBatchLimit(2 + 1);
        }
        else if ((draws[drawCounter - 1]->mode == TRIANGLES) &&            (draws[drawCounter - 1]->vertexCount%3 == 0))
        {
            CheckRenderBatchLimit(3 + 1);
        }
        else if ((draws[drawCounter - 1]->mode == QUADS) &&            (draws[drawCounter - 1]->vertexCount%4 == 0))
        {
            CheckRenderBatchLimit(4 + 1);
        }
    }

    vertexBuffer[currentBuffer]->vertices[vertexCounter].position.set(tx,ty,tz);
    vertexBuffer[currentBuffer]->vertices[vertexCounter].texcoord.set(texcoordx,texcoordy);
    vertexBuffer[currentBuffer]->vertices[vertexCounter].color.set(colorr,colorg,colorb,colora);

    vertexCounter++;
    draws[drawCounter - 1]->vertexCount++;
}


void RenderBatch::Vertex2f(float x, float y)
{
    Vertex3f(x, y, currentDepth);
}


void RenderBatch::Vertex2i(int x, int y)
{
    Vertex3f((float)x, (float)y, currentDepth);
}


void RenderBatch::TexCoord2f(float x, float y)
{
    texcoordx = x;
    texcoordy = y;
}



void RenderBatch::Color4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
    colorr = x;
    colorg = y;
    colorb = z;
    colora = w;
}


void RenderBatch::Color4f(float r, float g, float b, float a)
{
    Color4ub((unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255));
}

void RenderBatch::setMatrix(const Matrix &matrix)
{
    this->matrix = matrix;
}

 
void RenderBatch::Color3f(float x, float y, float z)
{
    Color4ub((unsigned char)(x*255), (unsigned char)(y*255), (unsigned char)(z*255), 255);
}

void RenderBatch::SetTexture(unsigned int id)
{
    if (id == 0)
    {
        if (vertexCounter >=    vertexBuffer[currentBuffer]->elementCount*4)
        {
            Render();
        }
    }
    else
    {
        if (draws[drawCounter - 1]->textureId != id)
        {
            if (draws[drawCounter - 1]->vertexCount > 0)
            {
                if (draws[drawCounter - 1]->mode == LINES) draws[drawCounter - 1]->vertexAlignment = ((draws[drawCounter - 1]->vertexCount < 4)? draws[drawCounter - 1]->vertexCount : draws[drawCounter - 1]->vertexCount%4);
                else if (draws[drawCounter - 1]->mode == TRIANGLES) draws[drawCounter - 1]->vertexAlignment = ((draws[drawCounter - 1]->vertexCount < 4)? 1 : (4 - (draws[drawCounter - 1]->vertexCount%4)));
                else draws[drawCounter - 1]->vertexAlignment = 0;

                if (!CheckRenderBatchLimit(draws[drawCounter - 1]->vertexAlignment))
                {
                    vertexCounter += draws[drawCounter - 1]->vertexAlignment;
                    drawCounter++;
                }
            }

            if (drawCounter >= BATCH_DRAWCALLS) Render();

            draws[drawCounter - 1]->textureId = id;
            draws[drawCounter - 1]->vertexCount = 0;
        }

    }
}



void RenderBatch::End(void)
{
    currentDepth=0.0f;

   // currentDepth += (1.0f/20000.0f);
}


void RenderBatch::DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, const Color &color)
{
    Begin(LINES);
        Color4ub(color.r, color.g, color.b, color.a);
        Vertex2f((float)startPosX, (float)startPosY);
        Vertex2f((float)endPosX, (float)endPosY);
    End();
}


void RenderBatch::DrawCircleSector(const Vector2 &center, float radius, float startAngle, float endAngle, int segments, const Color &color)
{
    if (radius <= 0.0f) radius = 0.1f;  // Avoid div by zero

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle)
    {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle)/90);

    if (segments < minSegments)
    {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2*powf(1 - SMOOTH_CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = (int)((endAngle - startAngle)*ceilf(2*PI/th)/360);

        if (segments <= 0) segments = minSegments;
    }

    float stepLength = (endAngle - startAngle)/(float)segments;
    float angle = startAngle;




    Begin(TRIANGLES);
        for (int i = 0; i < segments; i++)
        {
            Color4ub(color.r, color.g, color.b, color.a);

            Vertex2f(center.x, center.y);
            Vertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*radius, center.y + sinf(DEG2RAD*(angle + stepLength))*radius);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*radius, center.y + sinf(DEG2RAD*angle)*radius);

            angle += stepLength;
        }
    End();

}


// Draw a piece of a circle outlines
void RenderBatch::DrawCircleSectorLines(const Vector2 &center, float radius, float startAngle, float endAngle, int segments, const Color &color)
{
    if (radius <= 0.0f) radius = 0.1f;  // Avoid div by zero issue

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle)
    {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle)/90);

    if (segments < minSegments)
    {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2*powf(1 - SMOOTH_CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = (int)((endAngle - startAngle)*ceilf(2*PI/th)/360);

        if (segments <= 0) segments = minSegments;
    }

    float stepLength = (endAngle - startAngle)/(float)segments;
    float angle = startAngle;
    bool showCapLines = false;

    Begin(LINES);
        if (showCapLines)
        {
            Color4ub(color.r, color.g, color.b, color.a);
            Vertex2f(center.x, center.y);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*radius, center.y + sinf(DEG2RAD*angle)*radius);
        }

        for (int i = 0; i < segments; i++)
        {
            Color4ub(color.r, color.g, color.b, color.a);

            Vertex2f(center.x + cosf(DEG2RAD*angle)*radius, center.y + sinf(DEG2RAD*angle)*radius);
            Vertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*radius, center.y + sinf(DEG2RAD*(angle + stepLength))*radius);

            angle += stepLength;
        }

        if (showCapLines)
        {
            Color4ub(color.r, color.g, color.b, color.a);
            Vertex2f(center.x, center.y);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*radius, center.y + sinf(DEG2RAD*angle)*radius);
        }
    End();
}

void RenderBatch::DrawCircleGradient(int centerX, int centerY, float radius, const Color &color1, const Color &color2)
{
    Begin(TRIANGLES);
        for (int i = 0; i < 360; i += 10)
        {
            Color4ub(color1.r, color1.g, color1.b, color1.a);
            Vertex2f((float)centerX, (float)centerY);
            Color4ub(color2.r, color2.g, color2.b, color2.a);
            Vertex2f((float)centerX + cosf(DEG2RAD*(i + 10))*radius, (float)centerY + sinf(DEG2RAD*(i + 10))*radius);
            Color4ub(color2.r, color2.g, color2.b, color2.a);
            Vertex2f((float)centerX + cosf(DEG2RAD*i)*radius, (float)centerY + sinf(DEG2RAD*i)*radius);
        }
    End();
}


void RenderBatch::DrawCircleLines(int centerX, int centerY, float radius, const Color &color)
{
    DrawCircleLinesV(Vector2((float)centerX, (float)centerY ), radius, color);
}


void RenderBatch::DrawCircleLinesV(const Vector2 &center, float radius, const Color &color)
{
    Begin(LINES);
        Color4ub(color.r, color.g, color.b, color.a);

        // NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
        for (int i = 0; i < 360; i += 10)
        {
            Vertex2f(center.x + cosf(DEG2RAD*i)*radius, center.y + sinf(DEG2RAD*i)*radius);
            Vertex2f(center.x + cosf(DEG2RAD*(i + 10))*radius, center.y + sinf(DEG2RAD*(i + 10))*radius);
        }
    End();
}

void RenderBatch::DrawCircle(int centerX, int centerY, float radius, const Color &color)
{
    DrawCircleV(Vector2((float)centerX, (float)centerY ), radius, color);
}


void RenderBatch::DrawCircleV(const Vector2 &center, float radius, const Color &color)
{
    DrawCircleSector(center, radius, 0, 360, 36, color);
}


// Draw ellipse
void RenderBatch::DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, const Color &color)
{
    Begin(TRIANGLES);
        for (int i = 0; i < 360; i += 10)
        {
            Color4ub(color.r, color.g, color.b, color.a);
            Vertex2f((float)centerX, (float)centerY);
            Vertex2f((float)centerX + cosf(DEG2RAD*(i + 10))*radiusH, (float)centerY + sinf(DEG2RAD*(i + 10))*radiusV);
            Vertex2f((float)centerX + cosf(DEG2RAD*i)*radiusH, (float)centerY + sinf(DEG2RAD*i)*radiusV);
        }
    End();
}


void RenderBatch::DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, const Color &color)
{
    Begin(LINES);
        for (int i = 0; i < 360; i += 10)
        {
            Color4ub(color.r, color.g, color.b, color.a);
            Vertex2f(centerX + cosf(DEG2RAD*(i + 10))*radiusH, centerY + sinf(DEG2RAD*(i + 10))*radiusV);
            Vertex2f(centerX + cosf(DEG2RAD*i)*radiusH, centerY + sinf(DEG2RAD*i)*radiusV);
        }
    End();
}


void RenderBatch::DrawRing(const Vector2 &center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, const Color &color)
{
    if (startAngle == endAngle) return;

    // Function expects (outerRadius > innerRadius)
    if (outerRadius < innerRadius)
    {
        float tmp = outerRadius;
        outerRadius = innerRadius;
        innerRadius = tmp;

        if (outerRadius <= 0.0f) outerRadius = 0.1f;
    }

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle)
    {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle)/90);

    if (segments < minSegments)
    {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2*powf(1 - SMOOTH_CIRCLE_ERROR_RATE/outerRadius, 2) - 1);
        segments = (int)((endAngle - startAngle)*ceilf(2*PI/th)/360);

        if (segments <= 0) segments = minSegments;
    }

    // Not a ring
    if (innerRadius <= 0.0f)
    {
        DrawCircleSector(center, outerRadius, startAngle, endAngle, segments, color);
        return;
    }

    float stepLength = (endAngle - startAngle)/(float)segments;
    float angle = startAngle;

    Begin(TRIANGLES);
        for (int i = 0; i < segments; i++)
        {
            Color4ub(color.r, color.g, color.b, color.a);

            Vertex2f(center.x + cosf(DEG2RAD*angle)*innerRadius, center.y + sinf(DEG2RAD*angle)*innerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*innerRadius, center.y + sinf(DEG2RAD*(angle + stepLength))*innerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*outerRadius, center.y + sinf(DEG2RAD*angle)*outerRadius);

            Vertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*innerRadius, center.y + sinf(DEG2RAD*(angle + stepLength))*innerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*outerRadius, center.y + sinf(DEG2RAD*(angle + stepLength))*outerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*outerRadius, center.y + sinf(DEG2RAD*angle)*outerRadius);

            angle += stepLength;
        }
    End();

}

// Draw ring outline
void RenderBatch::DrawRingLines(const Vector2 &center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, const Color &color)
{
    if (startAngle == endAngle) return;

    // Function expects (outerRadius > innerRadius)
    if (outerRadius < innerRadius)
    {
        float tmp = outerRadius;
        outerRadius = innerRadius;
        innerRadius = tmp;

        if (outerRadius <= 0.0f) outerRadius = 0.1f;
    }

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle)
    {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle)/90);

    if (segments < minSegments)
    {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2*powf(1 - SMOOTH_CIRCLE_ERROR_RATE/outerRadius, 2) - 1);
        segments = (int)((endAngle - startAngle)*ceilf(2*PI/th)/360);

        if (segments <= 0) segments = minSegments;
    }

    if (innerRadius <= 0.0f)
    {
        DrawCircleSectorLines(center, outerRadius, startAngle, endAngle, segments, color);
        return;
    }

    float stepLength = (endAngle - startAngle)/(float)segments;
    float angle = startAngle;
    bool showCapLines = true;

    Begin(LINES);
        if (showCapLines)
        {
            Color4ub(color.r, color.g, color.b, color.a);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*outerRadius, center.y + sinf(DEG2RAD*angle)*outerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*innerRadius, center.y + sinf(DEG2RAD*angle)*innerRadius);
        }

        for (int i = 0; i < segments; i++)
        {
            Color4ub(color.r, color.g, color.b, color.a);

            Vertex2f(center.x + cosf(DEG2RAD*angle)*outerRadius, center.y + sinf(DEG2RAD*angle)*outerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*outerRadius, center.y + sinf(DEG2RAD*(angle + stepLength))*outerRadius);

            Vertex2f(center.x + cosf(DEG2RAD*angle)*innerRadius, center.y + sinf(DEG2RAD*angle)*innerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*(angle + stepLength))*innerRadius, center.y + sinf(DEG2RAD*(angle + stepLength))*innerRadius);

            angle += stepLength;
        }

        if (showCapLines)
        {
            Color4ub(color.r, color.g, color.b, color.a);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*outerRadius, center.y + sinf(DEG2RAD*angle)*outerRadius);
            Vertex2f(center.x + cosf(DEG2RAD*angle)*innerRadius, center.y + sinf(DEG2RAD*angle)*innerRadius);
        }
    End();
}


void RenderBatch::DrawRectangle(int posX, int posY, int width, int height,const Color &color)
{

    DrawRectanglePro(Rectangle(posX, posY, width, height ), Vector2( 0.0f, 0.0f ), 0.0f, color);
}


void RenderBatch::DrawRectangleV(const Vector2 &position, const Vector2 &size, const Color &color)
{
    DrawRectanglePro(Rectangle(position.x, position.y, size.x, size.y ), Vector2( 0.0f, 0.0f ), 0.0f, color);
}


void RenderBatch::DrawRectangleRec(const Rectangle &rec, const Color &color)
{
    DrawRectanglePro(rec, Vector2(0.0f, 0.0f ), 0.0f, color);
}


void RenderBatch::DrawRectanglePro(const Rectangle &rec, const Vector2 &origin, float rotation, const Color &color)
{
    Vector2 topLeft ;
    Vector2 topRight;
    Vector2 bottomLeft;
    Vector2 bottomRight;

    // Only calculate rotation if needed
    if (rotation == 0.0f)
    {
        float x = rec.x - origin.x;
        float y = rec.y - origin.y;
        topLeft.set(x, y);
        topRight.set( x + rec.width, y );
        bottomLeft.set(x, y + rec.height );
        bottomRight.set( x + rec.width, y + rec.height );
    }
    else
    {
        float sinRotation = sinf(rotation*DEG2RAD);
        float cosRotation = cosf(rotation*DEG2RAD);
        float x = rec.x;
        float y = rec.y;
        float dx = -origin.x;
        float dy = -origin.y;

        topLeft.x = x + dx*cosRotation - dy*sinRotation;
        topLeft.y = y + dx*sinRotation + dy*cosRotation;

        topRight.x = x + (dx + rec.width)*cosRotation - dy*sinRotation;
        topRight.y = y + (dx + rec.width)*sinRotation + dy*cosRotation;

        bottomLeft.x = x + dx*cosRotation - (dy + rec.height)*sinRotation;
        bottomLeft.y = y + dx*sinRotation + (dy + rec.height)*cosRotation;

        bottomRight.x = x + (dx + rec.width)*cosRotation - (dy + rec.height)*sinRotation;
        bottomRight.y = y + (dx + rec.width)*sinRotation + (dy + rec.height)*cosRotation;
    }

    Begin(TRIANGLES);

        Color4ub(color.r, color.g, color.b, color.a);

        Vertex2f(topLeft.x, topLeft.y);
        Vertex2f(bottomLeft.x, bottomLeft.y);
        Vertex2f(topRight.x, topRight.y);

        Vertex2f(topRight.x, topRight.y);
        Vertex2f(bottomLeft.x, bottomLeft.y);
        Vertex2f(bottomRight.x, bottomRight.y);

    End();

}

// Draw rectangle outline
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues
void RenderBatch::DrawRectangleLines(int posX, int posY, int width, int height, const Color &color)
{

    Begin(LINES);
        Color4ub(color.r, color.g, color.b, color.a);
        Vertex2f(posX + 1, posY + 1);
        Vertex2f(posX + width, posY + 1);

        Vertex2f(posX + width, posY + 1);
        Vertex2f(posX + width, posY + height);

        Vertex2f(posX + width, posY + height);
        Vertex2f(posX + 1, posY + height);

        Vertex2f(posX + 1, posY + height);
        Vertex2f(posX + 1, posY + 1);
    End();

}


void RenderBatch::DrawRectangleLinesEx(const Rectangle &rec, float lineThick, const Color &color)
{
    if ((lineThick > rec.width) || (lineThick > rec.height))
    {
        if (rec.width > rec.height) lineThick = rec.height/2;
        else if (rec.width < rec.height) lineThick = rec.width/2;
    }


    Rectangle top (rec.x, rec.y, rec.width, lineThick );
    Rectangle bottom  ( rec.x, rec.y - lineThick + rec.height, rec.width, lineThick );
    Rectangle left  ( rec.x, rec.y + lineThick, lineThick, rec.height - lineThick*2.0f );
    Rectangle right  ( rec.x - lineThick + rec.width, rec.y + lineThick, lineThick, rec.height - lineThick*2.0f );

    DrawRectangleRec(top, color);
    DrawRectangleRec(bottom, color);
    DrawRectangleRec(left, color);
    DrawRectangleRec(right, color);
}


void RenderBatch::DrawTexturePro(Texture2D &texture,  Rectangle &source, const Rectangle &dest, const Vector2 &origin, float rotation, const Color &tint)
{
    // // Check if texture is valid
    if (texture.id > 0)
    {
        float width  = (float)texture.width;
        float height = (float)texture.height;



        bool flipX = false;

        if (source.width < 0) { flipX = true; source.width *= -1; }
        if (source.height < 0) source.y -= source.height;


        



        Vector2 topLeft;
        Vector2 topRight;
        Vector2 bottomLeft;
        Vector2 bottomRight;

        // Only calculate rotation if needed
        if (rotation == 0.0f)
        {
            float x = dest.x - origin.x;
            float y = dest.y - origin.y;
            topLeft.set( x, y );
            topRight.set( x + dest.width, y );
            bottomLeft.set(x, y + dest.height );
            bottomRight.set( x + dest.width, y + dest.height );
        }
        else
        {
            float sinRotation = sinf(rotation*DEG2RAD);
            float cosRotation = cosf(rotation*DEG2RAD);
            float x = dest.x;
            float y = dest.y;
            float dx = -origin.x;
            float dy = -origin.y;

            topLeft.x = x + dx*cosRotation - dy*sinRotation;
            topLeft.y = y + dx*sinRotation + dy*cosRotation;

            topRight.x = x + (dx + dest.width)*cosRotation - dy*sinRotation;
            topRight.y = y + (dx + dest.width)*sinRotation + dy*cosRotation;

            bottomLeft.x = x + dx*cosRotation - (dy + dest.height)*sinRotation;
            bottomLeft.y = y + dx*sinRotation + (dy + dest.height)*cosRotation;

            bottomRight.x = x + (dx + dest.width)*cosRotation - (dy + dest.height)*sinRotation;
            bottomRight.y = y + (dx + dest.width)*sinRotation + (dy + dest.height)*cosRotation;
        }
    

         SetTexture(texture.id);
         Begin(QUADS);

         Color4ub(tint.r, tint.g, tint.b, tint.a);
            

            // Top-left corner for texture and quad
            if (flipX) TexCoord2f((source.x + source.width)/width, source.y/height);
            else TexCoord2f(source.x/width, source.y/height);
            Vertex2f(topLeft.x, topLeft.y);

            // Bottom-left corner for texture and quad
            if (flipX) TexCoord2f((source.x + source.width)/width, (source.y + source.height)/height);
            else TexCoord2f(source.x/width, (source.y + source.height)/height);
            Vertex2f(bottomLeft.x, bottomLeft.y);

            // Bottom-right corner for texture and quad
            if (flipX) TexCoord2f(source.x/width, (source.y + source.height)/height);
            else TexCoord2f((source.x + source.width)/width, (source.y + source.height)/height);
            Vertex2f(bottomRight.x, bottomRight.y);

            // Top-right corner for texture and quad
            if (flipX) TexCoord2f(source.x/width, source.y/height);
            else TexCoord2f((source.x + source.width)/width, source.y/height);
            Vertex2f(topRight.x, topRight.y);

         End();
        SetTexture(0);

     }
}
void RenderBatch::DrawTexture(Texture2D &texture, int posX, int posY, const Color &tint)
{
    DrawTextureEx(texture, Vector2((float)posX, (float)posY ), 0.0f, 1.0f, tint);
}

void RenderBatch::DrawTextureV(Texture2D &texture, const Vector2 &position, const Color &tint)
{
    DrawTextureEx(texture, position, 0, 1.0f, tint);
}

void RenderBatch::DrawTextureEx(Texture2D &texture,const  Vector2 &position, float rotation, float scale, const Color &tint)
{
    Rectangle source ( 0.0f, 0.0f, (float)texture.width, (float)texture.height );
    Rectangle dest (position.x, position.y, (float)texture.width*scale, (float)texture.height*scale );
    Vector2 origin ;

    DrawTexturePro(texture, source, dest, origin, rotation, tint);
}

void RenderBatch::DrawTextureRec(Texture2D &texture,  Rectangle &source, const Vector2 &position, const Color &tint)
{
    Rectangle dest(position.x, position.y, fabsf(source.width), fabsf(source.height) );
    Vector2 origin;

    DrawTexturePro(texture, source, dest, origin, 0.0f, tint);
}


bool Texture2D::Load(const char *fileName)
{
    unsigned char *fileData = NULL;
    unsigned int fileSize = 0;
    fileData = LoadFileData(fileName, &fileSize);

    if (fileData != NULL)
    {
        int comp = 0;
    //    stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load_from_memory(fileData, fileSize, &width, &height, &comp, 0);

        if (data != NULL)
        {
            if (comp == 1) format = GRAYSCALE;
            else if (comp == 2) format = GRAY_ALPHA;
            else if (comp == 3) format = R8G8B8;
            else if (comp == 4) format = R8G8B8A8;

            id = LoadTexture(data, width, height, format);
            stbi_image_free(data);
            return true;

        }
        else
        {
            Log(2, "[%s] Texture could not be loaded", fileName);
        }
    } 
    else
    {
        Log(2, "[%s] Texture could not be loaded", fileName);
    }
    return false;
}


bool Texture2D::LoadFromMemory(const unsigned char *fileData, int dataSize)
{
        
    if (fileData != NULL)
    {
            int comp = 0;
        //    stbi_set_flip_vertically_on_load(true);
            unsigned char *data = stbi_load_from_memory(fileData, dataSize, &width, &height, &comp, 0);

            if (data != NULL)
            {
                if (comp == 1) format = GRAYSCALE;
                else if (comp == 2) format = GRAY_ALPHA;
                else if (comp == 3) format = R8G8B8;
                else if (comp == 4) format = R8G8B8A8;

                id = LoadTexture(data, width, height, format);
                stbi_image_free(data);
                return true;

            }
            else
            {
                Log(2, "Texture could not be loaded");
            }      


    }
    else
    {
        Log(2, "Texture could not be loaded");
    }

    return false;
}

void Texture2D::Release()
{
    if (id > 0) UnloadTexture(id);
    id =0;
    
}
