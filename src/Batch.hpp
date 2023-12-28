#include "pch.hpp"

#define PI 3.14159265358979323846f
#define EPSILON 0.000001f
#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)

enum PixelFormat
{
    GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    GRAY_ALPHA,        // 8*2 bpp (2 channels)
    R8G8B8,            // 24 bpp
    R8G8B8A8,          // 32 bpp    
};

#define LINES                                0x0001     
#define TRIANGLES                            0x0004      
#define QUADS                                0x0008  

struct Vector2
{

    Vector2()
    {
        x = 0;
        y = 0;
    }
    Vector2(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
    void set(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
    float x, y;
};

struct Vector3
{
    Vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    Vector3(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
    void set(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
    
    float x, y, z;
};



struct Rectangle
{
    Rectangle()
    {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
    }
    Rectangle(float _x, float _y, float _width, float _height)
    {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
    }
    float x, y, width, height;
};

struct Color 
{
    Color()
    {
        r = 255;
        g = 255;
        b = 255;
        a = 255;
    }
    Color (unsigned char v)
    {
        r = v;
        g = v;
        b = v;
        a = 255;
    }
    Color (unsigned char _r, unsigned char _g, unsigned char _b)
    {
        r = _r;
        g = _g;
        b = _b;
        a = 255;
    }
    Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    void set(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
    unsigned char r, g, b, a;

    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Orange;
    static const Color Pink;
    static const Color Magenta;
    static const Color Purple;
    static const Color Brown;
    static const Color LightGray;
    static const Color LightGrey;
    static const Color DarkGray;
    static const Color DarkGrey;
    static const Color Transparent;
    static const Color Lime;
    static const Color Gold;
    static const Color SkyBlue;
    static const Color Beige;
    static const Color Violet;
    


    
};


struct Vertex
{
    Vertex()
    {
        position.x = 0;
        position.y = 0;
        position.z = 0;
        texcoord.x = 0;
        texcoord.y = 0;
        color.r = 255;
        color.g = 255;
        color.b = 255;
        color.a = 255;
    }
    Vertex(float x, float y, float z, float u, float v, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        position.x = x;
        position.y = y;
        position.z = z;
        texcoord.x = u;
        texcoord.y = v;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
    }


    Vector3 position;
    Vector2 texcoord;
    Color color;
};


struct Quaternion
{
    float x, y, z, w;
};

struct Matrix 
{


    Matrix()
    {
                 set( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );

    }
        Matrix( float m0, float m4, float m8, float m12,
            float m1, float m5, float m9, float m13,
            float m2, float m6, float m10, float m14,
            float m3, float m7, float m11, float m15 )
    {
        set( m0, m4, m8, m12,
             m1, m5, m9, m13,
             m2, m6, m10, m14,
             m3, m7, m11, m15 );
    }
    Matrix(const Matrix &mat)
    {
        set( mat.m0, mat.m4, mat.m8, mat.m12,
             mat.m1, mat.m5, mat.m9, mat.m13,
             mat.m2, mat.m6, mat.m10, mat.m14,
             mat.m3, mat.m7, mat.m11, mat.m15 );
    }


    Matrix& operator=(const Matrix &mat)
    {
        set( mat.m0, mat.m4, mat.m8, mat.m12,
             mat.m1, mat.m5, mat.m9, mat.m13,
             mat.m2, mat.m6, mat.m10, mat.m14,
             mat.m3, mat.m7, mat.m11, mat.m15 );
        return *this;
    }

    Matrix operator*(const Matrix &mat) const
    {
        Matrix result;
        result.m0 = m0*mat.m0 + m4*mat.m1 + m8*mat.m2 + m12*mat.m3;
        result.m1 = m1*mat.m0 + m5*mat.m1 + m9*mat.m2 + m13*mat.m3;
        result.m2 = m2*mat.m0 + m6*mat.m1 + m10*mat.m2 + m14*mat.m3;
        result.m3 = m3*mat.m0 + m7*mat.m1 + m11*mat.m2 + m15*mat.m3;
        result.m4 = m0*mat.m4 + m4*mat.m5 + m8*mat.m6 + m12*mat.m7;
        result.m5 = m1*mat.m4 + m5*mat.m5 + m9*mat.m6 + m13*mat.m7;
        result.m6 = m2*mat.m4 + m6*mat.m5 + m10*mat.m6 + m14*mat.m7;
        result.m7 = m3*mat.m4 + m7*mat.m5 + m11*mat.m6 + m15*mat.m7;
        result.m8 = m0*mat.m8 + m4*mat.m9 + m8*mat.m10 + m12*mat.m11;
        result.m9 = m1*mat.m8 + m5*mat.m9 + m9*mat.m10 + m13*mat.m11;
        result.m10 = m2*mat.m8 + m6*mat.m9 + m10*mat.m10 + m14*mat.m11;
        result.m11 = m3*mat.m8 + m7*mat.m9 + m11*mat.m10 + m15*mat.m11;
        result.m12 = m0*mat.m12 + m4*mat.m13 + m8*mat.m14 + m12*mat.m15;
        result.m13 = m1*mat.m12 + m5*mat.m13 + m9*mat.m14 + m13*mat.m15;
        result.m14 = m2*mat.m12 + m6*mat.m13 + m10*mat.m14 + m14*mat.m15;
        result.m15 = m3*mat.m12 + m7*mat.m13 + m11*mat.m14 + m15*mat.m15;
        return result;
    }
    void Identity()
    {
        set( 1.0f, 0.0f, 0.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.0f,
             0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, 0.0f, 0.0f, 1.0f );
    }
  
     void set(float _m0, float _m4, float _m8, float _m12,
           float _m1, float _m5, float _m9, float _m13,
           float _m2, float _m6, float _m10, float _m14,
           float _m3, float _m7, float _m11, float _m15)
    {
        m0 = _m0; m4 = _m4; m8 = _m8; m12 = _m12;
        m1 = _m1; m5 = _m5; m9 = _m9; m13 = _m13;
        m2 = _m2; m6 = _m6; m10 = _m10; m14 = _m14;
        m3 = _m3; m7 = _m7; m11 = _m11; m15 = _m15;
    }
    void Ortho(double left, double right, double bottom, double top, double nearPlane, double farPlane)
    {
        float rl = (float)(right - left);
        float tb = (float)(top - bottom);
        float fn = (float)(farPlane - nearPlane);

        m0 = 2.0f/rl;
        m1 = 0.0f;
        m2 = 0.0f;
        m3 = 0.0f;
        m4 = 0.0f;
        m5 = 2.0f/tb;
        m6 = 0.0f;
        m7 = 0.0f;
        m8 = 0.0f;
        m9 = 0.0f;
        m10 = -2.0f/fn;
        m11 = 0.0f;
        m12 = -((float)left + (float)right)/rl;
        m13 = -((float)top + (float)bottom)/tb;
        m14 = -((float)farPlane + (float)nearPlane)/fn;
        m15 = 1.0f;
    }


  
    float m0, m4, m8, m12;      
    float m1, m5, m9, m13;      
    float m2, m6, m10, m14;     
    float m3, m7, m11, m15;     
} ;

struct Shader
{

    bool Load(const char *vsFileName, const char *fsFileName); 
    bool Create(const char *vsCode, const char *fsCode);

    void Set();
    void Reset();

    unsigned int id;
};


struct Texture2D
{
    Texture2D()
    {
        id = 0;
        width = 0;
        height = 0;
        format = PixelFormat::R8G8B8A8;
    }
    ~Texture2D()
    {
        Release();
    }

    bool Load(const char *fileName);
    bool LoadFromMemory(const unsigned char *fileData, int dataSize);

    void Release();

    unsigned int id;        
    int width;              
    int height;              
    PixelFormat format;             
};


// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
struct VertexBuffer 
{
    int elementCount;           // Number of elements in the buffer (QUADS)
    std::vector<Vertex>  vertices;            // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    std::vector<unsigned int>  indices;    // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
    unsigned int vaoId;         // OpenGL Vertex Array Object id
    unsigned int vboId;      
    unsigned int iboId;      
} ;

struct DrawCall 
{
    int mode;                   // Drawing mode: LINES, TRIANGLES, QUADS
    int vertexCount;            // Number of vertex of the draw
    int vertexAlignment;        // Number of vertex required for index alignment (LINES, TRIANGLES)
    unsigned int textureId;     // Texture id to be used on the draw -> Use to create new draw call if changes
};


struct RenderBatch 
{
 
    RenderBatch();
    ~RenderBatch();

    void Init(int numBuffers, int bufferElements);
    void Release();


    void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, const Color &color);
    
    void DrawCircleSector(const Vector2 &center, float radius, float startAngle, float endAngle, int segments, const Color &color);
    void DrawCircleSectorLines(const Vector2 &center, float radius, float startAngle, float endAngle, int segments, const Color &color);

    void DrawCircleGradient(int centerX, int centerY, float radius, const Color &color1, const Color &color2);

    void DrawCircle(int centerX, int centerY, float radius, const Color &color);
    void DrawCircleV(const Vector2 &center, float radius, const Color &color);

    
    void DrawCircleLines(int centerX, int centerY, float radius, const Color &color);
    void DrawCircleLinesV(const Vector2 &center, float radius, const Color &color);
    void DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, const Color &color);
    void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, const Color &color);
    void DrawRing(const Vector2 &center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, const Color &color);
    void DrawRingLines(const Vector2 &center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, const Color &color);
    void DrawRectangle(int posX, int posY, int width, int height, const Color &color);
    void DrawRectangleV(const Vector2 &position, const Vector2 &size, const Color &color);
    void DrawRectangleRec(const Rectangle &rec, const Color &color);
    void DrawRectanglePro(const Rectangle &rec, const Vector2 &origin, float rotation, const Color &color);
    void DrawRectangleLines(int posX, int posY, int width, int height, const Color &color);
    void DrawRectangleLinesEx(const Rectangle &rec, float lineThick, const Color &color);

    void DrawTexture(Texture2D &texture, int posX, int posY, const Color &tint);
    void DrawTextureV(Texture2D &texture, const Vector2 &position, const Color &tint);
    void DrawTextureEx(Texture2D &texture,const  Vector2 &position, float rotation, float scale, const Color &tint);
    void DrawTextureRec(Texture2D &texture,  Rectangle &source, const Vector2 &position, const Color &tint);
    void DrawTexturePro(Texture2D &texture,  Rectangle &source, const Rectangle &dest, const Vector2 &origin, float rotation, const Color &tint);



    void Render();

    void Begin(int mode);                        
    void End(void);          

    void Vertex2i(int x, int y);                 
    void Vertex2f(float x, float y);          
    void Vertex3f(float x, float y, float z);     
    void TexCoord2f(float x, float y);          

    void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);  
    void Color3f(float x, float y, float z);          
    void Color4f(float x, float y, float z, float w); 

    void setMatrix(const Matrix &matrix);


    private:
        bool CheckRenderBatchLimit(int vCount);
        void SetTexture(unsigned int id);

    int bufferCount;            // Number of vertex buffers (multi-buffering support)
    int currentBuffer;          // Current buffer tracking in case of multi-buffering
    int drawCounter;            // Draw calls counter
    float currentDepth;         // Current depth value for next draw
    int vertexCounter;
    unsigned int defaultTextureId;  
    unsigned int defaultShaderId;

    unsigned int mpvId;
    unsigned int textId;

    std::vector<DrawCall*> draws;
    std::vector<VertexBuffer*> vertexBuffer;

    float texcoordx, texcoordy;         
    unsigned char colorr, colorg, colorb, colora;

    Matrix matrix;   


    

};