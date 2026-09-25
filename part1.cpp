// ============================================================
//  后世 Afterlife v2 — Backrooms 风格无限迷宫生存游戏
//  平台: Windows (Win32 API + OpenGL 3.3 Core)
//  特性: 无限迷宫 / 荧光灯动态光影 / 手电筒实时阴影 / 真实物理
//        人形建模(可见双腿双脚五指) / 主菜单+创建存档 / E背包
//        AKM步枪 / T聊天 / /giop @s akm / LAN+穿透联机
//        程序化音效 / 480FPS / 全程存档
//  构建: MinGW-w64: -static -lopengl32 -lgdi32 -luser32 -lwinmm -lws2_32
// ============================================================
#include <winsock2.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

typedef long long GLsizeiptr; typedef long long GLintptr;
#define PI 3.14159265358979323846f

// ---------------- GL function pointers (core 3.3 dynamic) ----------------
typedef void (APIENTRY*PFN_glGenVertexArrays)(GLsizei,GLuint*);
typedef void (APIENTRY*PFN_glBindVertexArray)(GLuint);
typedef void (APIENTRY*PFN_glDeleteVertexArrays)(GLsizei,const GLuint*);
typedef void (APIENTRY*PFN_glGenBuffers)(GLsizei,GLuint*);
typedef void (APIENTRY*PFN_glBindBuffer)(GLenum,GLuint);
typedef void (APIENTRY*PFN_glBufferData)(GLenum,GLsizeiptr,const void*,GLenum);
typedef void (APIENTRY*PFN_glBufferSubData)(GLenum,GLintptr,GLsizeiptr,const void*);
typedef void (APIENTRY*PFN_glDeleteBuffers)(GLsizei,const GLuint*);
typedef void (APIENTRY*PFN_glVertexAttribPointer)(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*);
typedef void (APIENTRY*PFN_glEnableVertexAttribArray)(GLuint);
typedef void (APIENTRY*PFN_glDisableVertexAttribArray)(GLuint);
typedef GLuint (APIENTRY*PFN_glCreateShader)(GLenum);
typedef void (APIENTRY*PFN_glShaderSource)(GLuint,GLsizei,const char* const*,const GLint*);
typedef void (APIENTRY*PFN_glCompileShader)(GLuint);
typedef void (APIENTRY*PFN_glGetShaderiv)(GLuint,GLenum,GLint*);
typedef void (APIENTRY*PFN_glGetShaderInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
typedef void (APIENTRY*PFN_glDeleteShader)(GLuint);
typedef GLuint (APIENTRY*PFN_glCreateProgram)(void);
typedef void (APIENTRY*PFN_glAttachShader)(GLuint,GLuint);
typedef void (APIENTRY*PFN_glLinkProgram)(GLuint);
typedef void (APIENTRY*PFN_glGetProgramiv)(GLuint,GLenum,GLint*);
typedef void (APIENTRY*PFN_glGetProgramInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
typedef void (APIENTRY*PFN_glDeleteProgram)(GLuint);
typedef void (APIENTRY*PFN_glUseProgram)(GLuint);
typedef void (APIENTRY*PFN_glBindAttribLocation)(GLuint,GLuint,const char*);
typedef GLint (APIENTRY*PFN_glGetUniformLocation)(GLuint,const char*);
typedef void (APIENTRY*PFN_glUniform1i)(GLint,GLint);
typedef void (APIENTRY*PFN_glUniform1f)(GLint,GLfloat);
typedef void (APIENTRY*PFN_glUniform2f)(GLint,GLfloat,GLfloat);
typedef void (APIENTRY*PFN_glUniform3f)(GLint,GLfloat,GLfloat,GLfloat);
typedef void (APIENTRY*PFN_glUniform4f)(GLint,GLfloat,GLfloat,GLfloat,GLfloat);
typedef void (APIENTRY*PFN_glUniformMatrix4fv)(GLint,GLsizei,GLboolean,const GLfloat*);
typedef void (APIENTRY*PFN_glActiveTexture)(GLenum);
typedef void (APIENTRY*PFN_glGenTextures)(GLsizei,GLuint*);
typedef void (APIENTRY*PFN_glBindTexture)(GLenum,GLuint);
typedef void (APIENTRY*PFN_glTexImage2D)(GLenum,GLint,GLint,GLsizei,GLsizei,GLint,GLenum,GLenum,const void*);
typedef void (APIENTRY*PFN_glTexParameteri)(GLenum,GLenum,GLint);
typedef void (APIENTRY*PFN_glGenerateMipmap)(GLenum);
typedef void (APIENTRY*PFN_glDeleteTextures)(GLsizei,const GLuint*);
typedef void (APIENTRY*PFN_glDrawArrays)(GLenum,GLint,GLsizei);
typedef void (APIENTRY*PFN_glDrawElements)(GLenum,GLsizei,GLenum,const void*);
typedef void (APIENTRY*PFN_glGenFramebuffers)(GLsizei,GLuint*);
typedef void (APIENTRY*PFN_glBindFramebuffer)(GLenum,GLuint);
typedef void (APIENTRY*PFN_glFramebufferTexture2D)(GLenum,GLenum,GLenum,GLuint,GLint);
typedef GLenum (APIENTRY*PFN_glCheckFramebufferStatus)(GLenum);
typedef void (APIENTRY*PFN_glDeleteFramebuffers)(GLsizei,const GLuint*);
typedef void (APIENTRY*PFN_glDrawBuffer)(GLenum);
typedef void (APIENTRY*PFN_glReadBuffer)(GLenum);
typedef HGLRC (APIENTRY*PFN_wglCreateContextAttribsARB)(HDC,HGLRC,const int*);
typedef BOOL (APIENTRY*PFN_wglSwapIntervalEXT)(int);

#define glGenVertexArrays glGenVertexArrays_pp
#define glBindVertexArray glBindVertexArray_pp
#define glDeleteVertexArrays glDeleteVertexArrays_pp
#define glGenBuffers glGenBuffers_pp
#define glBindBuffer glBindBuffer_pp
#define glBufferData glBufferData_pp
#define glBufferSubData glBufferSubData_pp
#define glDeleteBuffers glDeleteBuffers_pp
#define glVertexAttribPointer glVertexAttribPointer_pp
#define glEnableVertexAttribArray glEnableVertexAttribArray_pp
#define glDisableVertexAttribArray glDisableVertexAttribArray_pp
#define glCreateShader glCreateShader_pp
#define glShaderSource glShaderSource_pp
#define glCompileShader glCompileShader_pp
#define glGetShaderiv glGetShaderiv_pp
#define glGetShaderInfoLog glGetShaderInfoLog_pp
#define glDeleteShader glDeleteShader_pp
#define glCreateProgram glCreateProgram_pp
#define glAttachShader glAttachShader_pp
#define glLinkProgram glLinkProgram_pp
#define glGetProgramiv glGetProgramiv_pp
#define glGetProgramInfoLog glGetProgramInfoLog_pp
#define glDeleteProgram glDeleteProgram_pp
#define glUseProgram glUseProgram_pp
#define glBindAttribLocation glBindAttribLocation_pp
#define glGetUniformLocation glGetUniformLocation_pp
#define glUniform1i glUniform1i_pp
#define glUniform1f glUniform1f_pp
#define glUniform2f glUniform2f_pp
#define glUniform3f glUniform3f_pp
#define glUniform4f glUniform4f_pp
#define glUniformMatrix4fv glUniformMatrix4fv_pp
#define glActiveTexture glActiveTexture_pp
#define glGenTextures glGenTextures_pp
#define glBindTexture glBindTexture_pp
#define glTexImage2D glTexImage2D_pp
#define glTexParameteri glTexParameteri_pp
#define glGenerateMipmap glGenerateMipmap_pp
#define glDeleteTextures glDeleteTextures_pp
#define glDrawArrays glDrawArrays_pp
#define glDrawElements glDrawElements_pp
#define glGenFramebuffers glGenFramebuffers_pp
#define glBindFramebuffer glBindFramebuffer_pp
#define glFramebufferTexture2D glFramebufferTexture2D_pp
#define glCheckFramebufferStatus glCheckFramebufferStatus_pp
#define glDeleteFramebuffers glDeleteFramebuffers_pp
#define glDrawBuffer glDrawBuffer_pp
#define glReadBuffer glReadBuffer_pp

static PFN_glGenVertexArrays glGenVertexArrays=0;
static PFN_glBindVertexArray glBindVertexArray=0;
static PFN_glDeleteVertexArrays glDeleteVertexArrays=0;
static PFN_glGenBuffers glGenBuffers=0;
static PFN_glBindBuffer glBindBuffer=0;
static PFN_glBufferData glBufferData=0;
static PFN_glBufferSubData glBufferSubData=0;
static PFN_glDeleteBuffers glDeleteBuffers=0;
static PFN_glVertexAttribPointer glVertexAttribPointer=0;
static PFN_glEnableVertexAttribArray glEnableVertexAttribArray=0;
static PFN_glDisableVertexAttribArray glDisableVertexAttribArray=0;
static PFN_glCreateShader glCreateShader=0;
static PFN_glShaderSource glShaderSource=0;
static PFN_glCompileShader glCompileShader=0;
static PFN_glGetShaderiv glGetShaderiv=0;
static PFN_glGetShaderInfoLog glGetShaderInfoLog=0;
static PFN_glDeleteShader glDeleteShader=0;
static PFN_glCreateProgram glCreateProgram=0;
static PFN_glAttachShader glAttachShader=0;
static PFN_glLinkProgram glLinkProgram=0;
static PFN_glGetProgramiv glGetProgramiv=0;
static PFN_glGetProgramInfoLog glGetProgramInfoLog=0;
static PFN_glDeleteProgram glDeleteProgram=0;
static PFN_glUseProgram glUseProgram=0;
static PFN_glBindAttribLocation glBindAttribLocation=0;
static PFN_glGetUniformLocation glGetUniformLocation=0;
static PFN_glUniform1i glUniform1i=0;
static PFN_glUniform1f glUniform1f=0;
static PFN_glUniform2f glUniform2f=0;
static PFN_glUniform3f glUniform3f=0;
static PFN_glUniform4f glUniform4f=0;
static PFN_glUniformMatrix4fv glUniformMatrix4fv=0;
static PFN_glActiveTexture glActiveTexture=0;
static PFN_glGenTextures glGenTextures=0;
static PFN_glBindTexture glBindTexture=0;
static PFN_glTexImage2D glTexImage2D=0;
static PFN_glTexParameteri glTexParameteri=0;
static PFN_glGenerateMipmap glGenerateMipmap=0;
static PFN_glDeleteTextures glDeleteTextures=0;
static PFN_glDrawArrays glDrawArrays=0;
static PFN_glDrawElements glDrawElements=0;
static PFN_glGenFramebuffers glGenFramebuffers=0;
static PFN_glBindFramebuffer glBindFramebuffer=0;
static PFN_glFramebufferTexture2D glFramebufferTexture2D=0;
static PFN_glCheckFramebufferStatus glCheckFramebufferStatus=0;
static PFN_glDeleteFramebuffers glDeleteFramebuffers=0;
static PFN_glDrawBuffer glDrawBuffer=0;
static PFN_glReadBuffer glReadBuffer=0;
static PFN_wglCreateContextAttribsARB wglCreateContextAttribsARB=0;
static PFN_wglSwapIntervalEXT wglSwapIntervalEXT=0;

static void* GetGLProc(const char* n){
    void* p=(void*)wglGetProcAddress(n);
    if(!p){ HMODULE h=GetModuleHandleA("opengl32.dll"); if(h) p=(void*)GetProcAddress(h,n); }
    return p;
}
static bool LoadGL(){
    #define GET(n) { n=(PFN_##n)GetGLProc(#n); if(!n) return false; }
    GET(glGenVertexArrays);GET(glBindVertexArray);GET(glDeleteVertexArrays);
    GET(glGenBuffers);GET(glBindBuffer);GET(glBufferData);GET(glBufferSubData);GET(glDeleteBuffers);
    GET(glVertexAttribPointer);GET(glEnableVertexAttribArray);GET(glDisableVertexAttribArray);
    GET(glCreateShader);GET(glShaderSource);GET(glCompileShader);GET(glGetShaderiv);GET(glGetShaderInfoLog);GET(glDeleteShader);
    GET(glCreateProgram);GET(glAttachShader);GET(glLinkProgram);GET(glGetProgramiv);GET(glGetProgramInfoLog);GET(glDeleteProgram);
    GET(glUseProgram);GET(glBindAttribLocation);GET(glGetUniformLocation);
    GET(glUniform1i);GET(glUniform1f);GET(glUniform2f);GET(glUniform3f);GET(glUniform4f);GET(glUniformMatrix4fv);
    GET(glActiveTexture);GET(glGenTextures);GET(glBindTexture);GET(glTexImage2D);GET(glTexParameteri);GET(glGenerateMipmap);GET(glDeleteTextures);
    GET(glDrawArrays);GET(glDrawElements);
    GET(glGenFramebuffers);GET(glBindFramebuffer);GET(glFramebufferTexture2D);GET(glCheckFramebufferStatus);GET(glDeleteFramebuffers);
    GET(glDrawBuffer);GET(glReadBuffer);
    #undef GET
    wglCreateContextAttribsARB=(PFN_wglCreateContextAttribsARB)wglGetProcAddress("wglCreateContextAttribsARB");
    wglSwapIntervalEXT=(PFN_wglSwapIntervalEXT)wglGetProcAddress("wglSwapIntervalEXT");
    return wglCreateContextAttribsARB!=0;
}

// ==================== math ====================
struct Vec3 {
    float x,y,z;
    Vec3():x(0),y(0),z(0){}
    Vec3(float a,float b,float c):x(a),y(b),z(c){}
    Vec3 operator+(const Vec3&o)const{return Vec3(x+o.x,y+o.y,z+o.z);}
    Vec3 operator-(const Vec3&o)const{return Vec3(x-o.x,y-o.y,z-o.z);}
    Vec3 operator*(float s)const{return Vec3(x*s,y*s,z*s);}
    Vec3 operator-()const{return Vec3(-x,-y,-z);}
    float dot(const Vec3&o)const{return x*o.x+y*o.y+z*o.z;}
    Vec3 cross(const Vec3&o)const{return Vec3(y*o.z-z*o.y, z*o.x-x*o.z, x*o.y-y*o.x);}
    float len()const{return sqrtf(x*x+y*y+z*z);}
    Vec3 norm()const{float l=len();return l>1e-6f?Vec3(x/l,y/l,z/l):Vec3(0,1,0);}
};
struct Mat4 { float m[16]; };
static float LerpF(float a,float b,float t){return a+(b-a)*t;}
static float ClampF(float v,float a,float b){return v<a?a:(v>b?b:v);}
static Mat4 M4Id(){ Mat4 r; memset(r.m,0,sizeof(r.m)); r.m[0]=r.m[5]=r.m[10]=r.m[15]=1; return r; }
static Mat4 M4Mul(const Mat4&a,const Mat4&b){
    Mat4 r; for(int c=0;c<4;c++)for(int ro=0;ro<4;ro++){
        float s=0; for(int k=0;k<4;k++) s+=a.m[k*4+ro]*b.m[c*4+k]; r.m[c*4+ro]=s; }
    return r;
}
static Mat4 M4Persp(float fovy,float asp,float zn,float zf){
    Mat4 r=M4Id(); float f=1.0f/tanf(fovy*0.5f);
    r.m[0]=f/asp; r.m[5]=f; r.m[10]=(zf+zn)/(zn-zf); r.m[11]=-1; r.m[14]=2*zf*zn/(zn-zf); r.m[15]=0; return r;
}
static Mat4 M4Ortho(float l,float r,float b,float t,float n,float f){
    Mat4 m=M4Id(); m.m[0]=2/(r-l); m.m[5]=2/(t-b); m.m[10]=-2/(f-n);
    m.m[12]=-(r+l)/(r-l); m.m[13]=-(t+b)/(t-b); m.m[14]=-(f+n)/(f-n); return m;
}
static Mat4 M4LookAt(Vec3 e,Vec3 c,Vec3 up){
    Vec3 f=(c-e).norm(); Vec3 s=f.cross(up).norm(); Vec3 u=s.cross(f);
    Mat4 r=M4Id(); r.m[0]=s.x; r.m[4]=s.y; r.m[8]=s.z;
    r.m[1]=u.x; r.m[5]=u.y; r.m[9]=u.z;
    r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;
    r.m[12]=-s.dot(e); r.m[13]=-u.dot(e); r.m[14]=f.dot(e); return r;
}
static Mat4 M4T(Vec3 t){ Mat4 r=M4Id(); r.m[12]=t.x; r.m[13]=t.y; r.m[14]=t.z; return r; }
static Mat4 M4RX(float a){ Mat4 r=M4Id(); float c=cosf(a),s=sinf(a); r.m[5]=c; r.m[6]=s; r.m[9]=-s; r.m[10]=c; return r; }
static Mat4 M4RY(float a){ Mat4 r=M4Id(); float c=cosf(a),s=sinf(a); r.m[0]=c; r.m[2]=-s; r.m[8]=s; r.m[10]=c; return r; }
static Mat4 M4RZ(float a){ Mat4 r=M4Id(); float c=cosf(a),s=sinf(a); r.m[0]=c; r.m[1]=s; r.m[4]=-s; r.m[5]=c; return r; }
static Mat4 M4S(Vec3 s){ Mat4 r=M4Id(); r.m[0]=s.x; r.m[5]=s.y; r.m[10]=s.z; return r; }

// ==================== hash / noise ====================
static int g_seed=20260925;
static unsigned g_wr=1;
static float WRand(){ g_wr=g_wr*1664525u+1013904223u; return (float)((g_wr>>8)&0xffffff)/16777216.0f; }
static float Hash01(int x,int z){
    unsigned h=(unsigned)(x*374761393u+z*668265263u);
    h^=h>>13; h*=1274126177u; h^=h>>16;
    h^=(unsigned)g_seed*1013904223u;
    h^=h>>13; h*=1274126177u; h^=h>>16;
    return (float)(h&0xffffff)/16777216.0f;
}
static float Noise1(float x){
    int i=(int)floorf(x); float f=x-i; float u=f*f*(3-2*f);
    return LerpF(Hash01(i,0),Hash01(i+1,0),u);
}

// ==================== mesh ====================
struct Mesh{ GLuint vao,vbo,ebo; int count; bool indexed; };
static Mesh M0(){ Mesh m; m.vao=m.vbo=m.ebo=0; m.count=0; m.indexed=false; return m; }
static void BuildMesh(Mesh&m,const vector<float>&v,const vector<unsigned int>&idx,int stride){
    glGenVertexArrays(1,&m.vao); glBindVertexArray(m.vao);
    glGenBuffers(1,&m.vbo); glBindBuffer(GL_ARRAY_BUFFER,m.vbo);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(float),&v[0],GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,stride*sizeof(float),0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(3*sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(6*sizeof(float))); glEnableVertexAttribArray(2);
    glGenBuffers(1,&m.ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,idx.size()*sizeof(unsigned int),&idx[0],GL_STATIC_DRAW);
    m.count=(int)idx.size(); m.indexed=true;
}
static void MeshQuadXY(Mesh&m,float w,float h){
    float v[4*8]={0,0,0, 0,0,1, 0,1,0, w,0,1,0, w,h,1,1, 0,h,0,1};
    float vv[4*8]={0,0,0, 0,0,1, w,0,0, 1,0,1, w,h,0, 1,1,1, 0,h,0, 0,1,1};
    unsigned int idx[6]={0,1,2,0,2,3};
    BuildMesh(m,(stride==2?v:vv),idx,4);
}
static Mesh meshQuad;
static Mesh MakeBox(float sx,float sy,float sz){
    // unit box at origin, scaled by caller
    vector<float> v; vector<unsigned int> idx;
    float P[8][3]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
    float N[6][3]={{0,0,-1},{0,0,1},{-1,0,0},{1,0,0},{0,-1,0},{0,1,0}};
    int F[6][4]={{0,1,2,3},{5,4,7,6},{4,0,3,7},{1,5,6,2},{4,5,1,0},{3,2,6,7}};
    for(int fi=0;fi<6;fi++){
        for(int k=0;k<4;k++){
            int pi=F[fi][k];
            v.push_back(P[pi][0]*sx); v.push_back(P[pi][1]*sy); v.push_back(P[pi][2]*sz);
            v.push_back(N[fi][0]); v.push_back(N[fi][1]); v.push_back(N[fi][2]);
            float uu=(k==1||k==2)?1.0f:0.0f, vv=(k==2||k==3)?1.0f:0.0f;
            v.push_back(uu); v.push_back(vv);
        }
        unsigned int b=fi*4;
        idx.push_back(b);idx.push_back(b+1);idx.push_back(b+2);
        idx.push_back(b);idx.push_back(b+2);idx.push_back(b+3);
    }
    Mesh m; BuildMesh(m,v,idx,8); return m;
}
static Mesh MakeCyl(float r,float h,int seg){
    vector<float> v; vector<unsigned int> idx;
    auto push=[&](float x,float y,float z,float nx,float ny,float nz,float u,float vv){ v.push_back(x);v.push_back(y);v.push_back(z);v.push_back(nx);v.push_back(ny);v.push_back(nz);v.push_back(u);v.push_back(vv); };
    int n=seg;
    for(int i=0;i<n;i++){
        float a0=(float)i/n*2*PI, a1=(float)(i+1)/n*2*PI;
        float c0=cosf(a0),s0=sinf(a0),c1=cosf(a1),s1=sinf(a1);
        push(0,h,0,0,1,0,0.5f,1); push(r*c0,h,r*s0,0,1,0,c0*0.5f+0.5f,1); push(r*c1,h,r*s1,0,1,0,c1*0.5f+0.5f,1);
        push(0,0,0,0,-1,0,0.5f,0); push(r*c1,0,r*s1,0,-1,0,c1*0.5f+0.5f,0); push(r*c0,0,r*s0,0,-1,0,c0*0.5f+0.5f,0);
        push(r*c0,h,r*s0,c0,0,s0,(float)i/n,1); push(r*c1,h,r*s1,c1,0,s1,(float)(i+1)/n,1);
        push(r*c1,0,r*s1,c1,0,s1,(float)(i+1)/n,0); push(r*c0,h,r*s0,c0,0,s0,(float)i/n,1);
        push(r*c0,0,r*s0,c0,0,s0,(float)i/n,0); push(r*c1,0,r*s1,c1,0,s1,(float)(i+1)/n,0);
    }
    unsigned int b0=0;
    for(int i=0;i<n;i++){ unsigned int b=i*12;
        idx.push_back(b);idx.push_back(b+1);idx.push_back(b+2);
        idx.push_back(b+3);idx.push_back(b+4);idx.push_back(b+5);
        idx.push_back(b+6);idx.push_back(b+7);idx.push_back(b+8);
        idx.push_back(b+9);idx.push_back(b+10);idx.push_back(b+11);
    }
    (void)b0;
    Mesh m; BuildMesh(m,v,idx,8); return m;
}
static Mesh MakeSphere(float r,int s,int t){
    vector<float> v; vector<unsigned int> idx;
    auto push=[&](float x,float y,float z,float nx,float ny,float nz,float u,float vv){ v.push_back(x);v.push_back(y);v.push_back(z);v.push_back(nx);v.push_back(ny);v.push_back(nz);v.push_back(u);v.push_back(vv); };
    for(int j=0;j<=t;j++){
        float p0=(float)j/t*PI, p1=(float)(j+1)/t*PI;
        for(int i=0;i<s;i++){
            float a0=(float)i/s*2*PI, a1=(float)(i+1)/s*2*PI;
            Vec3 v00(r*sinf(p0)*cosf(a0), r*cosf(p0), r*sinf(p0)*sinf(a0));
            Vec3 v10(r*sinf(p0)*cosf(a1), r*cosf(p0), r*sinf(p0)*sinf(a1));
            Vec3 v01(r*sinf(p1)*cosf(a0), r*cosf(p1), r*sinf(p1)*sinf(a0));
            Vec3 v11(r*sinf(p1)*cosf(a1), r*cosf(p1), r*sinf(p1)*sinf(a1));
            push(v00.x,v00.y,v00.z, v00.x/r,v00.y/r,v00.z/r, (float)i/s,(float)j/t);
            push(v10.x,v10.y,v10.z, v10.x/r,v10.y/r,v10.z/r, (float)(i+1)/s,(float)j/t);
            push(v01.x,v01.y,v01.z, v01.x/r,v01.y/r,v01.z/r, (float)i/s,(float)(j+1)/t);
            push(v10.x,v10.y,v10.z, v10.x/r,v10.y/r,v10.z/r, (float)(i+1)/s,(float)j/t);
            push(v11.x,v11.y,v11.z, v11.x/r,v11.y/r,v11.z/r, (float)(i+1)/s,(float)(j+1)/t);
            push(v01.x,v01.y,v01.z, v01.x/r,v01.y/r,v01.z/r, (float)i/s,(float)(j+1)/t);
        }
    }
    for(int i=0;i<s*t;i++){ unsigned int b=i*6;
        idx.push_back(b);idx.push_back(b+1);idx.push_back(b+2);
        idx.push_back(b+3);idx.push_back(b+4);idx.push_back(b+5);
    }
    Mesh m; BuildMesh(m,v,idx,8); return m;
}
static void DrawMesh(const Mesh&m){ glBindVertexArray(m.vao); glDrawElements(GL_TRIANGLES,m.count,GL_UNSIGNED_INT,0); }

static Mesh meshBox,meshCyl,meshSphere,meshBottle,meshCap,meshMeat,meshBook,meshFlashB,meshFlashH,meshAKM;
static void MakeBaseMeshes(){
    meshBox=MakeBox(0.5f,0.5f,0.5f); // half-size so scale=full size
    meshCyl=MakeCyl(0.5f,1.0f,14);
    meshSphere=MakeSphere(0.5f,14,8);
    meshQuad=M0();
    { float v[4*8]={0,0,0, 0,0,1, 1,0,0, 1,0,1, 1,1,0, 1,1,1, 0,1,0, 0,1,1};
      unsigned int idx[6]={0,1,2,0,2,3};
      vector<float> vv(v,v+32); vector<unsigned int> ii(idx,idx+6);
      BuildMesh(meshQuad,vv,ii,4); }
    meshBottle=MakeCyl(0.06f,0.24f,10);
    meshCap=MakeCyl(0.045f,0.06f,10);
    meshMeat=MakeBox(0.11f,0.09f,0.09f);
    meshBook=MakeBox(0.13f,0.02f,0.17f);
    meshFlashB=MakeCyl(0.032f,0.18f,10);
    meshFlashH=MakeCyl(0.05f,0.07f,10);
    meshAKM=MakeBox(0.5f,0.5f,0.5f);
}

// ==================== textures (procedural) ====================
struct Canvas { int w,h; vector<unsigned char> px;
    Canvas(int W,int H):w(W),h(H),px((size_t)W*H*4,0){}
    void Set(int x,int y,unsigned char r,unsigned char g,unsigned char b,unsigned char a=255){
        if(x<0||y<0||x>=w||y>=h)return; size_t i=((size_t)y*w+x)*4; px[i]=r;px[i+1]=g;px[i+2]=b;px[i+3]=a; }
    GLuint Upload(){
        GLuint t=0; glGenTextures(1,&t); glBindTexture(GL_TEXTURE_2D,t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,&px[0]);
        return t; }
};
static GLuint g_texWall,g_texFloor,g_texCeil,g_texPanel,g_texWhite;
static GLuint g_texSkin,g_texShirt,g_texPants,g_texWood,g_texMetal,g_texMeatTex,g_texBook,g_texBottle;
static GLuint g_iconWater,g_iconMeat,g_iconFlash,g_iconBook,g_iconAKM;
static void MakeAllTextures(){
    { Canvas c(256,256); // 黄色墙纸: 竖条纹 + 污渍 + 底边
        for(int y=0;y<256;y++)for(int x=0;x<256;x++){
            float n=Fbm2(x*0.08f,y*0.08f,4,2.0f,0.5f);
            float stripe=(sinf(x*0.28f)+1)*0.5f*0.10f;
            unsigned char r=(unsigned char)ClampF(198+stripe*255+n*14,0,255);
            unsigned char g=(unsigned char)ClampF(182+stripe*255+n*12,0,255);
            unsigned char b=(unsigned char)ClampF(122+n*16,0,255);
            if(y<20){ r=(unsigned char)(r*0.55); g=(unsigned char)(g*0.55); b=(unsigned char)(b*0.55); } // baseboard
            c.Set(x,y,r,g,b);
        }
        // stains
        for(int i=0;i<26;i++){ int sx=rand()%256,sy=80+rand()%140, rr=6+rand()%14;
            for(int dy=-rr;dy<=rr;dy++)for(int dx=-rr;dx<=rr;dx++){
                if(dx*dx+dy*dy<=rr*rr){
                    int x=sx+dx,y=sy+dy; if(x>=0&&y>=0&&x<256&&y<256){
                        size_t j=((size_t)y*256+x)*4;
                        c.px[j]=(unsigned char)(c.px[j]*0.82); c.px[j+1]=(unsigned char)(c.px[j+1]*0.85); c.px[j+2]=(unsigned char)(c.px[j+2]*0.72); } } }
        }
        g_texWall=c.Upload(); }
    { Canvas c(256,256); // 潮湿地毯: 暗棕 + 霉斑
        for(int y=0;y<256;y++)for(int x=0;x<256;x++){
            float n=Fbm2(x*0.12f,y*0.12f,4,2.0f,0.5f);
            unsigned char r=(unsigned char)ClampF(96+n*40,0,255);
            unsigned char g=(unsigned char)ClampF(84+n*36,0,255);
            unsigned char b=(unsigned char)ClampF(58+n*26,0,255);
            c.Set(x,y,r,g,b);
        }
        for(int i=0;i<30;i++){ int sx=rand()%256,sy=rand()%256, rr=4+rand()%10;
            for(int dy=-rr;dy<=rr;dy++)for(int dx=-rr;dx<=rr;dx++){
                if(dx*dx+dy*dy<=rr*rr){ int x=sx+dx,y=sy+dy; if(x>=0&&y>=0&&x<256&&y<256){
                    size_t j=((size_t)y*256+x)*4;
                    c.px[j]=(unsigned char)(c.px[j]*0.7); c.px[j+1]=(unsigned char)(c.px[j+1]*0.78); c.px[j+2]=(unsigned char)(c.px[j+2]*0.6); } } }
        }
        g_texFloor=c.Upload(); }
    { Canvas c(256,256); // 天花板: 白色吸音板 + 网格
        for(int y=0;y<256;y++)for(int x=0;x<256;x++){
            float n=Fbm2(x*0.1f,y*0.1f,3,2.0f,0.5f);
            unsigned char v=(unsigned char)ClampF(205+n*26,0,255);
            c.Set(x,y,v,v,v);
        }
        for(int i=0;i<32;i+=4){ for(int y=0;y<256;y++){ c.Set(i,y,160,160,160); c.Set(y,i,160,160,160); } }
        g_texCeil=c.Upload(); }
    { Canvas c(64,64); for(int y=0;y<64;y++)for(int x=0;x<64;x++) c.Set(x,y,250,250,245);
        g_texPanel=c.Upload(); }
    { Canvas c(8,8); c.FillRect(0,0,8,8,255,255,255); g_texWhite=c.Upload(); }
    { Canvas c(64,64); for(int y=0;y<64;y++)for(int x=0;x<64;x++){
        float n=Fbm2(x*0.1f,y*0.1f,3,2.0f,0.5f);
        c.Set(x,y,(unsigned char)(210+n*30),(unsigned char)(170+n*26),(unsigned char)(140+n*20)); }
        g_texSkin=c.Upload(); }
    { Canvas c(64,64); for(int y=0;y<64;y++)for(int x=0;x<64;x++){
        c.Set(x,y,92,104,128); } g_texShirt=c.Upload(); }
    { Canvas c(64,64); for(int y=0;y<64;y++)for(int x=0;x<64;x++){
        c.Set(x,y,66,58,52); } g_texPants=c.Upload(); }
    { Canvas c(128,128); for(int y=0;y<128;y++)for(int x=0;x<128;x++){
        float n=Fbm2(x*0.08f,y*0.08f,4,2.0f,0.5f);
        c.Set(x,y,(unsigned char)(140+n*50),(unsigned char)(104+n*40),(unsigned char)(62+n*30)); }
        g_texWood=c.Upload(); }
    { Canvas c(128,128); for(int y=0;y<128;y++)for(int x=0;x<128;x++){
        float n=Fbm2(x*0.12f,y*0.12f,3,2.0f,0.5f);
        unsigned char v=(unsigned char)(120+n*60);
        c.Set(x,y,v,v,(unsigned char)(v*0.95)); }
        g_texMetal=c.Upload(); }
    { Canvas c(64,64); for(int y=0;y<64;y++)for(int x=0;x<64;x++){
        float n=Fbm2(x*0.09f,y*0.09f,4,2.0f,0.5f);
        c.Set(x,y,(unsigned char)(130+n*50),(unsigned char)(60+n*30),(unsigned char)(52+n*26)); }
        g_texMeatTex=c.Upload(); }
    { Canvas c(64,64); for(int y=0;y<64;y++)for(int x=0;x<64;x++){
        c.Set(x,y,150,112,72); } g_texBook=c.Upload(); }
    { Canvas c(64,64); for(int y=0;y<64;y++)for(int x=0;x<64;x++){
        c.Set(x,y,120,180,230); } g_texBottle=c.Upload(); }
    // icons 64x64
    { Canvas c(64,64); c.FillRect(0,0,64,64,0,0,0,0);
        c.FillRect(16,10,48,52,70,150,220); c.FillRect(22,2,42,12,150,220,255);
        c.FillRect(16,36,48,52,90,180,240); g_iconWater=c.Upload(); }
    { Canvas c(64,64); c.FillRect(0,0,64,64,0,0,0,0);
        c.FillCircle(32,34,20,150,60,46); c.FillCircle(32,34,10,100,36,28); c.FillCircle(32,16,10,150,60,46);
        g_iconMeat=c.Upload(); }
    { Canvas c(64,64); c.FillRect(0,0,64,64,0,0,0,0);
        c.FillRect(22,8,44,54,70,70,80); c.FillCircle(32,10,9,250,240,150); c.FillRect(26,52,40,58,40,40,50);
        g_iconFlash=c.Upload(); }
    { Canvas c(64,64); c.FillRect(0,0,64,64,0,0,0,0);
        c.FillRect(10,14,54,50,150,112,72); c.FillRect(14,10,50,20,120,90,55); c.FillRect(28,10,36,14,255,255,255);
        g_iconBook=c.Upload(); }
    { Canvas c(64,64); c.FillRect(0,0,64,64,0,0,0,0);
        c.FillRect(12,14,52,54,60,60,64); c.FillRect(28,8,36,18,110,80,50); c.FillRect(12,40,52,50,50,50,54);
        c.FillRect(20,18,26,44,40,40,44); g_iconAKM=c.Upload(); }
}
