// ============================================================
//  后世 Afterlife v2 — Backrooms 风格无限迷宫生存游戏
//  平台: Windows (Win32 API + OpenGL 3.3 Core)
//  特性: 无限迷宫 / 荧光灯动态光影 / 手电筒实时阴影 / 真实物理
//        人形建模(可见双腿双脚五指) / 主菜单+创建存档 / E背包
//        AKM步枪 / T聊天 / /giop @s akm / LAN+穿透联机
//        程序化音效 / 480FPS / 全程存档
//  构建: MinGW-w64: -static -lopengl32 -lgdi32 -luser32 -lwinmm -lws2_32
// ============================================================
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
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


// ==================== globals & types (part0) ====================
enum GameState{ GAME_MENU, GAME_PLAY, GAME_PAUSE, GAME_DEAD };
enum CamMode{ CM_FP=0, CM_TP, CM_GOD };
struct InvItem{ int id; int cnt; };
struct AABB{ Vec3 mn,mx; };
struct WorldItem{ int type; Vec3 pos; float rot; bool taken; };
struct MsgT{ wstring s; float t; };
static HINSTANCE g_hInst=0;
static HWND g_hWnd=0; static HDC g_hDC=0; static HGLRC g_hRC=0;
static int g_W=1280,g_H=720;
static wstring g_exeDir;
static int g_gamestate=GAME_MENU;
static int g_cam=CM_FP;
static Vec3 g_pos(0,1.7f,0); static float g_yaw=0,g_pitch=0,g_velY=0; static bool g_onGround=true;
static float g_hp=100,g_san=100,g_hun=100,g_thr=100,g_sta=100;
static float g_xp=0; static int g_level=1,g_capacity=40;
static vector<InvItem> g_inv;
static vector<WorldItem> g_items;
static bool g_flashOn=true;
static bool g_invOpen=false,g_chatOpen=false,g_dead=false;
static int g_hotbarSel=0,g_selSlot=-1,g_hoverSlot=-1;
static int g_dragId=0,g_dragCnt=0;
static int g_ammo=30; static float g_reloadT=0;
static float g_time=0,g_dt=0,g_fps=0,g_walkT=0,g_stepT=0;
static float g_eatAnim=0,g_drinkAnim=0;
static int g_menuSel=0,g_pauseSel=0;
static wstring g_playerName=L"幸存者",g_nameBuf,g_chatBuf;
static vector<wstring> g_msgs,g_chatLog;
static vector<MsgT> g_views;
static Vec3 g_camPos; static Vec3 g_camFwd; static Mat4 g_view,g_proj;
static wstring g_loadText=L"正在加载...";
static int simKey[256]={0};
static void GetExeDir(){ wchar_t buf[1024]; GetModuleFileNameW(NULL,buf,1023);
    wchar_t* sl=wcsrchr(buf,L'\\'); if(sl)*sl=0; g_exeDir=buf; }
static wstring ItemName(int id){
    switch(id){case 1:return L"银杏水(杏仁水)";case 2:return L"腐肉块";case 3:return L"手电筒";
    case 4:return L"生存手册";case 5:return L"AKM 步枪";default:return L"空";}
}
static float MaxF(float a,float b){return a>b?a:b;}

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


// ==================== canvas ====================
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

// ==================== canvas helpers (fill methods) ====================
inline void CanvasFillRect(Canvas&c,int x0,int y0,int x1,int y1,unsigned char r,unsigned char g,unsigned char b,unsigned char a=255){
    if(x0<0)x0=0;if(y0<0)y0=0;if(x1>c.w)x1=c.w;if(y1>c.h)y1=c.h;
    for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++) c.Set(x,y,r,g,b,a);
}
inline void CanvasFillCircle(Canvas&c,int cx,int cy,int rr,unsigned char r,unsigned char g,unsigned char b){
    for(int dy=-rr;dy<=rr;dy++)for(int dx=-rr;dx<=rr;dx++)
        if(dx*dx+dy*dy<=rr*rr) c.Set(cx+dx,cy+dy,r,g,b);
}
inline void CanvasLine(Canvas&c,int x0,int y0,int x1,int y1,int th,unsigned char r,unsigned char g,unsigned char b){
    int dx=abs(x1-x0),dy=abs(y1-y0),sx=x0<x1?1:-1,sy=y0<y1?1:-1,err=dx-dy;
    for(;;){ for(int t=-th/2;t<=th/2;t++){ c.Set(x0,y0+t,r,g,b); c.Set(x0+t,y0,r,g,b); }
        if(x0==x1&&y0==y1)break; int e2=2*err;
        if(e2>-dy){err-=dy;x0+=sx;} if(e2<dx){err+=dx;y0+=sy;} }
}

// ==================== GDI text texture (with CJK font fallback) ====================
struct TextTex{ GLuint tex; int w,h; };
static map<wstring,TextTex> g_textCache;
static TextTex MakeTextTex(const wstring& s,int px){
    TextTex t; t.w=0;t.h=0;t.tex=0; if(s.empty()) return t;
    const wchar_t* fonts[]={L"Microsoft YaHei",L"WenQuanYi Zen Hei",L"SimHei",L"SimSun",L"NSimSun",L"KaiTi",L"Arial"};
    HDC dc=CreateCompatibleDC(NULL);
    int best=-1; vector<unsigned char> bestBits; int bw=0,bh=0;
    for(int fi=0;fi<7;fi++){
        HFONT f=CreateFontW(-px,0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,fonts[fi]);
        HGDIOBJ of=SelectObject(dc,f);
        RECT rc={0,0,0,0}; DrawTextW(dc,s.c_str(),-1,&rc,DT_CALCRECT|DT_NOPREFIX);
        int w=max(1,(int)rc.right), h=max(1,(int)rc.bottom);
        BITMAPINFO bmi; memset(&bmi,0,sizeof(bmi));
        bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth=w; bmi.bmiHeader.biHeight=-h;
        bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32; bmi.bmiHeader.biCompression=BI_RGB;
        void* bits=NULL; HBITMAP hbm=CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,&bits,NULL,0);
        if(bits) memset(bits,0,(size_t)w*h*4);
        HGDIOBJ ob=SelectObject(dc,hbm);
        SetBkMode(dc,TRANSPARENT); SetTextColor(dc,RGB(255,255,255));
        RECT dr={0,0,w,h}; DrawTextW(dc,s.c_str(),-1,&dr,DT_NOPREFIX);
        vector<unsigned char> buf((size_t)w*h*4);
        GetDIBits(dc,hbm,0,h,&buf[0],&bmi,DIB_RGB_COLORS);
        int cov=0; for(size_t i=2;i<buf.size();i+=4) if(buf[i]>60) cov++;
        float ratio=(float)cov/(float)(w*h);
        if(ratio>0.03f&&ratio<0.85f){ best=fi; bestBits=buf; bw=w; bh=h; }
        SelectObject(dc,ob); DeleteObject(hbm); SelectObject(dc,of); DeleteObject(f);
        if(best>=0) break;
    }
    DeleteDC(dc);
    if(best<0) return t;
    t.w=bw; t.h=bh;
    vector<unsigned char> rgba((size_t)bw*bh*4);
    for(int y=0;y<bh;y++)for(int x=0;x<bw;x++){
        size_t i=((size_t)y*bw+x)*4; unsigned char a=bestBits[i+2];
        rgba[i]=255;rgba[i+1]=255;rgba[i+2]=255;rgba[i+3]=a;
    }
    glGenTextures(1,&t.tex); glBindTexture(GL_TEXTURE_2D,t.tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,bw,bh,0,GL_RGBA,GL_UNSIGNED_BYTE,&rgba[0]);
    return t;
}
static TextTex GetText(const wstring& s,int px){
    wstring key=s+L"|"+to_wstring(px);
    auto it=g_textCache.find(key);
    if(it!=g_textCache.end()) return it->second;
    TextTex t=MakeTextTex(s,px); g_textCache[key]=t; return t;
}

// ==================== chunks: infinite backrooms ====================
static const float CELLSZ=8.0f, ROOMH=3.0f, CHUNKSZ=32.0f;
static const int CELLS=4;

struct GeoBuild{ vector<float> v; vector<unsigned int> idx; };
static void GeoQuad(GeoBuild&g,Vec3 a,Vec3 b,Vec3 c,Vec3 d,Vec3 n,float u0,float v0,float u1,float v1){
    unsigned int base=(unsigned int)(g.v.size()/8);
    Vec3 p[4]={a,b,c,d};
    for(int i=0;i<4;i++){
        g.v.push_back(p[i].x);g.v.push_back(p[i].y);g.v.push_back(p[i].z);
        g.v.push_back(n.x);g.v.push_back(n.y);g.v.push_back(n.z);
        float u=(i==1||i==2)?u1:u0, v=(i==2||i==3)?v1:v0;
        g.v.push_back(u);g.v.push_back(v);
    }
    g.idx.push_back(base);g.idx.push_back(base+1);g.idx.push_back(base+2);
    g.idx.push_back(base);g.idx.push_back(base+2);g.idx.push_back(base+3);
}
// solid cell test (infinite maze): corridors every 3rd row/col guarantee connectivity
static bool CellSolid(int i,int j){
    if(j%3==0||i%3==0) return false;      // corridors always open
    if(Hash01(i,j)>0.80f) return false;   // occasional rooms
    return true;
}
static float CellFloor(int i,int j){ return Hash01(i,j)*0.12f; } // slight damp variation
static bool CellFlicker(int i,int j){ return Hash01(i+777,j+555)>0.80f; }
struct Chunk{
    int cx,cz; bool gen;
    vector<AABB> walls;
    vector<Vec3> panels;
    Mesh meshWall, meshFC, meshPanel;
    Chunk():cx(0),cz(0),gen(false){}
};
static map<int,Chunk> g_chunks;
static int ChunkKey(int cx,int cz){ return cx*100003+cz; }
static int g_lhx=0,g_lhz=0; // last player chunk for respawn anchor
static void BuildChunk(int cx,int cz){
    int key=ChunkKey(cx,cz);
    Chunk&ch=g_chunks[key]; ch.cx=cx; ch.cz=cz;
    ch.walls.clear(); ch.panels.clear();
    GeoBuild gw,gf,gp;
    float ox=cx*CHUNKSZ, oz=cz*CHUNKSZ;
    for(int j=0;j<CELLS;j++)for(int i=0;i<CELLS;i++){
        int gi=cx*CELLS+i, gj=cz*CELLS+j;
        float x0=ox+i*CELLSZ, z0=oz+j*CELLSZ;
        float floorY=CellFloor(gi,gj);
        bool solid=CellSolid(gi,gj);
        // floor & ceiling (all cells)
        GeoQuad(gf,Vec3(x0,floorY,z0),Vec3(x0+CELLSZ,floorY,z0),Vec3(x0+CELLSZ,floorY,z0+CELLSZ),Vec3(x0,floorY,z0+CELLSZ),Vec3(0,1,0),gi*2.0f,gj*2.0f,(gi+1)*2.0f,(gj+1)*2.0f);
        GeoQuad(gf,Vec3(x0,ROOMH,z0+CELLSZ),Vec3(x0+CELLSZ,ROOMH,z0+CELLSZ),Vec3(x0+CELLSZ,ROOMH,z0),Vec3(x0,ROOMH,z0),Vec3(0,-1,0),gi*2.0f,gj*2.0f,(gi+1)*2.0f,(gj+1)*2.0f);
        ch.panels.push_back(Vec3(x0+CELLSZ/2,ROOMH-0.06f,z0+CELLSZ/2));
        float pw=3.2f;
        Vec3 pc(x0+CELLSZ/2,ROOMH-0.03f,z0+CELLSZ/2);
        GeoQuad(gp,Vec3(pc.x-pw,pc.y,pc.z-pw*0.5f),Vec3(pc.x+pw,pc.y,pc.z-pw*0.5f),Vec3(pc.x+pw,pc.y,pc.z+pw*0.5f),Vec3(pc.x-pw,pc.y,pc.z+pw*0.5f),Vec3(0,-1,0),0,0,1,1);
        if(solid){
            bool broken=Hash01(gi*3+1,gj*7+2)>0.90f;
            float top=broken?1.0f:ROOMH;
            AABB w; w.mn=Vec3(x0,0,z0); w.mx=Vec3(x0+CELLSZ,top,z0+CELLSZ);
            ch.walls.push_back(w);
            if(!CellSolid(gi-1,gj)) GeoQuad(gw,Vec3(x0,0,z0),Vec3(x0,top,z0),Vec3(x0,top,z0+CELLSZ),Vec3(x0,0,z0+CELLSZ),Vec3(-1,0,0),0,0,CELLSZ*0.25f,ROOMH*0.25f);
            if(!CellSolid(gi+1,gj)) GeoQuad(gw,Vec3(x0+CELLSZ,0,z0+CELLSZ),Vec3(x0+CELLSZ,top,z0+CELLSZ),Vec3(x0+CELLSZ,top,z0),Vec3(x0+CELLSZ,0,z0),Vec3(1,0,0),0,0,CELLSZ*0.25f,ROOMH*0.25f);
            if(!CellSolid(gi,gj-1)) GeoQuad(gw,Vec3(x0+CELLSZ,0,z0),Vec3(x0+CELLSZ,top,z0),Vec3(x0,top,z0),Vec3(x0,0,z0),Vec3(0,0,-1),0,0,CELLSZ*0.25f,ROOMH*0.25f);
            if(!CellSolid(gi,gj+1)) GeoQuad(gw,Vec3(x0,0,z0+CELLSZ),Vec3(x0,top,z0+CELLSZ),Vec3(x0+CELLSZ,top,z0+CELLSZ),Vec3(x0+CELLSZ,0,z0+CELLSZ),Vec3(0,0,1),0,0,CELLSZ*0.25f,ROOMH*0.25f);
        }
    }
    if(!gw.v.empty()){ Mesh m=M0(); BuildMesh(m,gw.v,gw.idx,8); ch.meshWall=m; }
    if(!gf.v.empty()){ Mesh m=M0(); BuildMesh(m,gf.v,gf.idx,8); ch.meshFC=m; }
    if(!gp.v.empty()){ Mesh m=M0(); BuildMesh(m,gp.v,gp.idx,8); ch.meshPanel=m; }
    ch.gen=true;
}
static void UpdateChunks(){
    int pcx=(int)floorf(g_pos.x/CHUNKSZ), pcz=(int)floorf(g_pos.z/CHUNKSZ);
    const int R=3;
    vector<int> need;
    for(int dz=-R;dz<=R;dz++)for(int dx=-R;dx<=R;dx++){
        int key=ChunkKey(pcx+dx,pcz+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()||!it->second.gen) BuildChunk(pcx+dx,pcz+dz);
    }
    for(auto it=g_chunks.begin();it!=g_chunks.end();){
        if(abs(it->second.cx-pcx)>R+2||abs(it->second.cz-pcz)>R+2){
            if(it->second.meshWall.vao) glDeleteVertexArrays(1,&it->second.meshWall.vao);
            if(it->second.meshFC.vao) glDeleteVertexArrays(1,&it->second.meshFC.vao);
            if(it->second.meshPanel.vao) glDeleteVertexArrays(1,&it->second.meshPanel.vao);
            it=g_chunks.erase(it);
        } else ++it;
    }
    g_lhx=pcx; g_lhz=pcz;
}
static float FloorAt(float x,float z){
    int gi=(int)floorf(x/CELLSZ), gj=(int)floorf(z/CELLSZ);
    return CellFloor(gi,gj);
}
static void CollectNearWalls(const Vec3&p,vector<AABB>&out,float margin){
    out.clear();
    int pcx=(int)floorf(p.x/CHUNKSZ), pcz=(int)floorf(p.z/CHUNKSZ);
    for(int dz=-1;dz<=1;dz++)for(int dx=-1;dx<=1;dx++){
        int key=ChunkKey(pcx+dx,pcz+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()||!it->second.gen) continue;
        const Chunk&ch=it->second;
        for(size_t i=0;i<ch.walls.size();i++){
            const AABB&w=ch.walls[i];
            if(p.x+margin>w.mn.x&&p.x-margin<w.mx.x&&p.z+margin>w.mn.z&&p.z-margin<w.mx.z)
                out.push_back(w);
        }
    }
}
// player physics (AABB capsule, axis separated, gravity, no flying)
static float PLR_HALF=0.35f, PLR_H=1.75f;
static void MovePlayer(float dx,float dz){
    vector<AABB> ws;
    float y0=g_pos.y;
    CollectNearWalls(g_pos,ws,PLR_HALF+0.1f);
    // X axis
    g_pos.x+=dx;
    for(size_t i=0;i<ws.size();i++){
        const AABB&w=ws[i];
        if(g_pos.x+PLR_HALF>w.mn.x&&g_pos.x-PLR_HALF<w.mx.x&&y0<w.mx.y&&y0+PLR_H>w.mn.y){
            if(g_pos.z+PLR_HALF>w.mn.z&&g_pos.z-PLR_HALF<w.mx.z){
                if(dx>0) g_pos.x=w.mn.x-PLR_HALF; else if(dx<0) g_pos.x=w.mx.x+PLR_HALF;
            }
        }
    }
    // Z axis
    g_pos.z+=dz;
    CollectNearWalls(g_pos,ws,PLR_HALF+0.1f);
    for(size_t i=0;i<ws.size();i++){
        const AABB&w=ws[i];
        if(g_pos.x+PLR_HALF>w.mn.x&&g_pos.x-PLR_HALF<w.mx.x&&y0<w.mx.y&&y0+PLR_H>w.mn.y){
            if(g_pos.z+PLR_HALF>w.mn.z&&g_pos.z-PLR_HALF<w.mx.z){
                if(dz>0) g_pos.z=w.mn.z-PLR_HALF; else if(dz<0) g_pos.z=w.mx.z+PLR_HALF;
            }
        }
    }
}
static void StepPhysics(float dt){
    float fl=FloorAt(g_pos.x,g_pos.z);
    if(g_pos.y<fl){ g_pos.y=fl; g_velY=0; g_onGround=true; }
    // ceiling clamp
    if(g_pos.y+PLR_H>ROOMH-0.1f){ g_pos.y=ROOMH-0.1f-PLR_H; if(g_velY>0) g_velY=0; }
    if(!g_onGround){
        g_velY-=19.5f*dt; g_pos.y+=g_velY*dt;
        if(g_pos.y<=fl){ g_pos.y=fl; g_velY=0; g_onGround=true; }
    }
}
// ray vs AABB (for AKM hit)
static bool RayAABB(Vec3 o,Vec3 d,AABB b,float&t){
    float tmin=0,tmax=1e9f;
    for(int i=0;i<3;i++){
        float oi=(i==0)?o.x:(i==1)?o.y:o.z;
        float di=(i==0)?d.x:(i==1)?d.y:d.z;
        float mn=(i==0)?b.mn.x:(i==1)?b.mn.y:b.mn.z;
        float mx=(i==0)?b.mx.x:(i==1)?b.mx.y:b.mx.z;
        if(fabsf(di)<1e-6f){ if(oi<mn||oi>mx) return false; }
        else{
            float t1=(mn-oi)/di, t2=(mx-oi)/di;
            if(t1>t2){ float tt=t1; t1=t2; t2=tt; }
            tmin=max(tmin,t1); tmax=min(tmax,t2);
            if(tmin>tmax) return false;
        }
    }
    t=tmin; return t<500.0f;
}

// ==================== 2D value noise / fbm ====================
static float Noise2(float x,float z){
    int ix=(int)floorf(x), iz=(int)floorf(z);
    float fx=x-ix, fz=z-iz;
    float u=fx*fx*(3-2*fx), v=fz*fz*(3-2*fz);
    float a=Hash01(ix,iz), b=Hash01(ix+1,iz), c=Hash01(ix,iz+1), d=Hash01(ix+1,iz+1);
    return LerpF(LerpF(a,b,u),LerpF(c,d,u),v);
}
static float Fbm2(float x,float z,int oct,float lac,float gain){
    float amp=1,fr=1,sum=0,tot=0;
    for(int i=0;i<oct;i++){ sum+=Noise2(x*fr,z*fr)*amp; tot+=amp; amp*=gain; fr*=lac; }
    return sum/tot;
}

// ==================== textures (procedural) ====================
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
    { Canvas c(8,8); CanvasFillRect(c,0,0,8,8,255,255,255); g_texWhite=c.Upload(); }
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
    { Canvas c(64,64); CanvasFillRect(c,0,0,64,64,0,0,0,0);
        CanvasFillRect(c,16,10,48,52,70,150,220); CanvasFillRect(c,22,2,42,12,150,220,255);
        CanvasFillRect(c,16,36,48,52,90,180,240); g_iconWater=c.Upload(); }
    { Canvas c(64,64); CanvasFillRect(c,0,0,64,64,0,0,0,0);
        CanvasFillCircle(c,32,34,20,150,60,46); CanvasFillCircle(c,32,34,10,100,36,28); CanvasFillCircle(c,32,16,10,150,60,46);
        g_iconMeat=c.Upload(); }
    { Canvas c(64,64); CanvasFillRect(c,0,0,64,64,0,0,0,0);
        CanvasFillRect(c,22,8,44,54,70,70,80); CanvasFillCircle(c,32,10,9,250,240,150); CanvasFillRect(c,26,52,40,58,40,40,50);
        g_iconFlash=c.Upload(); }
    { Canvas c(64,64); CanvasFillRect(c,0,0,64,64,0,0,0,0);
        CanvasFillRect(c,10,14,54,50,150,112,72); CanvasFillRect(c,14,10,50,20,120,90,55); CanvasFillRect(c,28,10,36,14,255,255,255);
        g_iconBook=c.Upload(); }
    { Canvas c(64,64); CanvasFillRect(c,0,0,64,64,0,0,0,0);
        CanvasFillRect(c,12,14,52,54,60,60,64); CanvasFillRect(c,28,8,36,18,110,80,50); CanvasFillRect(c,12,40,52,50,50,50,54);
        CanvasFillRect(c,20,18,26,44,40,40,44); g_iconAKM=c.Upload(); }
}

// ==================== audio (procedural WAV) ====================
static void GenSoundWav(const wchar_t* name,int type){
    wstring path=g_exeDir+L"\\"+name;
    FILE* f=_wfopen(path.c_str(),L"rb");
    if(f){ fclose(f); return; } // already exists
    f=_wfopen(path.c_str(),L"wb");
    if(!f) return;
    const int SR=22050, N= (type==0)?SR/6:(type==1)?SR/2:(type==2)?SR/3:(type==3)?SR/5:(type==4)?SR/4:(type==5)?SR/10:(type==6)?SR/20:(type==7)?SR/3:(type==8)?SR/12:SR/4;
    vector<short> pcm(N,0);
    unsigned rnd=12345u;
    for(int i=0;i<N;i++){
        float t=(float)i/SR; float s=0;
        if(type==0){ // carpet step: muffled thud
            float env=expf(-t*22.0f);
            s=(sinf(t*95.0f)*0.6f+sinf(t*180.0f)*0.3f)*env*(t>0.09f?-1.0f:1.0f);
        } else if(type==1){ // drink: water-ish bubbles
            float env=expf(-t*3.0f);
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            s=(sinf(t*420.0f)*0.3f+n*0.7f)*env*0.7f;
        } else if(type==2){ // eat: crunch
            float env=expf(-t*9.0f);
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            s=(n*0.9f+sinf(t*700.0f)*0.15f)*env;
        } else if(type==3){ // pickup: blip
            s=sinf(t*880.0f*(1.0f-t*2.0f))*expf(-t*8.0f)*0.7f;
        } else if(type==4){ // drop: thud
            s=sinf(t*130.0f)*expf(-t*15.0f)*0.8f;
        } else if(type==5){ // levelup: arpeggio
            float fr[4]={523.0f,659.0f,784.0f,1046.0f};
            int seg=(int)(t*8.0f); if(seg<4) s=sinf(2*PI*fr[seg]*t)*0.5f*expf(-t*2.0f);
        } else if(type==6){ // click
            s=sinf(t*1200.0f)*expf(-t*60.0f)*0.6f;
        } else if(type==7){ // gunshot: noise burst + crack
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            float env=t<0.01f?1.0f:expf(-(t-0.01f)*30.0f);
            s=(n*0.9f+sinf(t*1800.0f)*0.2f)*env;
        } else if(type==8){ // drywall hit
            float env=expf(-t*18.0f);
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            s=(n*0.6f+sinf(t*240.0f)*0.4f)*env;
        } else if(type==9){ // reload click-clack
            if(t<0.05f) s=sinf(t*1000.0f)*expf(-t*80.0f)*0.6f;
            else if(t>0.12f&&t<0.16f) s=sinf((t-0.12f)*1400.0f)*expf(-(t-0.12f)*90.0f)*0.7f;
        }
        float v=ClampF(s*32767.0f,-32767,32767); pcm[i]=(short)v;
    }
    unsigned char hdr[44]={0};
    memcpy(hdr,"RIFF",4); unsigned sz=(unsigned)(36+N*2);
    memcpy(hdr+4,&sz,4); memcpy(hdr+8,"WAVE",4); memcpy(hdr+12,"fmt ",4);
    unsigned fsz=16; memcpy(hdr+16,&fsz,4);
    unsigned short audiofmt=1,ch=1; unsigned rate=SR, byterate=SR*2, align=2, bits=16;
    memcpy(hdr+20,&audiofmt,2); memcpy(hdr+22,&ch,2); memcpy(hdr+24,&rate,4);
    memcpy(hdr+28,&byterate,4); memcpy(hdr+32,&align,2); memcpy(hdr+34,&bits,2);
    memcpy(hdr+36,"data",4); unsigned dsz=(unsigned)(N*2); memcpy(hdr+40,&dsz,4);
    fwrite(hdr,1,44,f); fwrite(&pcm[0],2,N,f); fclose(f);
}
static void GenSounds(){
    GenSoundWav(L"sfx_step.wav",0); GenSoundWav(L"sfx_drink.wav",1); GenSoundWav(L"sfx_eat.wav",2);
    GenSoundWav(L"sfx_pick.wav",3); GenSoundWav(L"sfx_drop.wav",4); GenSoundWav(L"sfx_level.wav",5);
    GenSoundWav(L"sfx_click.wav",6); GenSoundWav(L"sfx_gun.wav",7); GenSoundWav(L"sfx_wall.wav",8);
    GenSoundWav(L"sfx_reload.wav",9);
}
static void Sfx(const wchar_t* n){ PlaySoundW((g_exeDir+L"\\"+n).c_str(),NULL,SND_ASYNC|SND_FILENAME|SND_NODEFAULT); }

// ==================== networking (UDP LAN + STUN hole punch) ====================
static SOCKET g_sock=INVALID_SOCKET;
static sockaddr_in g_lanAddr;
static bool g_broadcastOn=true;
static float g_netTimer=0; static float g_stateTimer=0;
struct RemotePlayer{ char name[32]; Vec3 pos; float yaw,pitch; int cam,hp,sel; float last; };
static map<string,RemotePlayer> g_peers;
struct LanPeer{ sockaddr_in addr; char name[32]; float last; };
static vector<LanPeer> g_lanPeers;
static bool g_stunDone=false; static string g_pubIP="--"; static int g_pubPort=0;
static float g_stunTimer=0; static SOCKET g_stunSock=INVALID_SOCKET;
static bool g_connected=false;

static void NetInit(){
    WSADATA wd; WSAStartup(MAKEWORD(2,2),&wd);
    g_sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    u_long nb=1; ioctlsocket(g_sock,FIONBIO,&nb);
    BOOL br=TRUE; setsockopt(g_sock,SOL_SOCKET,SO_BROADCAST,(const char*)&br,sizeof(br));
    sockaddr_in b; memset(&b,0,sizeof(b)); b.sin_family=AF_INET; b.sin_addr.s_addr=htonl(INADDR_ANY); b.sin_port=0;
    bind(g_sock,(sockaddr*)&b,sizeof(b));
    memset(&g_lanAddr,0,sizeof(g_lanAddr)); g_lanAddr.sin_family=AF_INET;
    g_lanAddr.sin_addr.s_addr=htonl(INADDR_BROADCAST); g_lanAddr.sin_port=htons(47778);
    // STUN socket
    g_stunSock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    ioctlsocket(g_stunSock,FIONBIO,&nb);
}
static void NetSendTo(const sockaddr_in&addr,const char*data,int len){
    if(g_sock!=INVALID_SOCKET) sendto(g_sock,data,len,0,(sockaddr*)&addr,sizeof(addr));
}
static void StunProbe(){
    // async resolve once
    static bool resolving=false;
    if(!resolving&&!g_stunDone){ resolving=true;
        addrinfo hints; memset(&hints,0,sizeof(hints)); hints.ai_family=AF_INET; hints.ai_socktype=SOCK_DGRAM;
        addrinfo* res=0;
        if(getaddrinfo("stun.l.google.com","19302",&hints,&res)==0&&res){
            sockaddr_in sa; memcpy(&sa,res->ai_addr,res->ai_addrlen);
            char req[20]; memset(req,0,20);
            req[0]=0x00;req[1]=0x01; // binding request
            unsigned id=(unsigned)(GetTickCount()^0x5C87A);
            memcpy(req+4,&id,4); memcpy(req+8,&id,4); memcpy(req+12,&id,4);
            sendto(g_stunSock,req,20,0,(sockaddr*)&sa,sizeof(sa));
            g_stunTimer=2.0f;
            freeaddrinfo(res);
        }
        resolving=false;
    }
    // read response
    char buf[512]; sockaddr_in from; int fl=sizeof(from);
    int n=recvfrom(g_stunSock,buf,sizeof(buf),0,(sockaddr*)&from,&fl);
    if(n>=20){
        unsigned char* p=(unsigned char*)buf;
        unsigned msgtype=(p[0]<<8)|p[1];
        if(msgtype==0x0101){ // success response
            for(int off=20;off+4<=n;){
                unsigned atype=(p[off]<<8)|p[off+1]; unsigned alen=(p[off+2]<<8)|p[off+3];
                if(atype==0x0020&&alen>=8){
                    unsigned char fam=p[off+4+1];
                    if(fam==0x01){
                        unsigned xport=(p[off+4+2]<<8)|p[off+4+3];
                        unsigned xaddr=(p[off+4+4]<<24)|(p[off+4+5]<<16)|(p[off+4+6]<<8)|p[off+4+7];
                        unsigned port=xport^0x2112; unsigned addr=xaddr^0x2112A442;
                        char ipbuf[32]; sprintf(ipbuf,"%u.%u.%u.%u",(addr>>24)&255,(addr>>16)&255,(addr>>8)&255,addr&255);
                        g_pubIP=ipbuf; g_pubPort=(int)port; g_stunDone=true;
                    }
                }
                off+=4+alen;
            }
        }
    }
    if(g_stunTimer>0){ g_stunTimer-=g_dt; if(g_stunTimer<=0){ g_stunDone=true; } }
}
static void NetUpdate(float dt){
    StunProbe();
    if(g_sock==INVALID_SOCKET) return;
    g_netTimer-=dt;
    g_stateTimer-=dt;
    if(g_broadcastOn&&g_netTimer<=0){ g_netTimer=2.0f;
        char pkt[256]; sprintf(pkt,"HSB|%S",g_playerName.c_str());
        NetSendTo(g_lanAddr,pkt,(int)strlen(pkt));
    }
    // read packets
    char buf[2048]; sockaddr_in from; int fl=sizeof(from);
    int n=recvfrom(g_sock,buf,sizeof(buf),0,(sockaddr*)&from,&fl);
    while(n>0){
        buf[n]=0;
        if(strncmp(buf,"HSB|",4)==0){
            bool found=false;
            for(size_t i=0;i<g_lanPeers.size();i++){
                if(g_lanPeers[i].addr.sin_addr.s_addr==from.sin_addr.s_addr&&g_lanPeers[i].addr.sin_port==from.sin_port){
                    g_lanPeers[i].last=g_time; found=true; break; }
            }
            if(!found){ LanPeer lp; lp.addr=from; lp.last=g_time;
                strncpy(lp.name,buf+4,31); lp.name[31]=0;
                g_lanPeers.push_back(lp); }
        } else if(strncmp(buf,"P|",2)==0){ // state packet
            char name[32]; float px,py,pz,yaw,pitch; int cam,hp,sel;
            if(sscanf(buf+2,"%31[^|]|%f|%f|%f|%f|%f|%d|%d|%d",name,&px,&py,&pz,&yaw,&pitch,&cam,&hp,&sel)>=9){
                char keybuf[64]; sprintf(keybuf,"%s:%d",inet_ntoa(from.sin_addr),ntohs(from.sin_port));
                RemotePlayer&rp=g_peers[string(keybuf)];
                strncpy(rp.name,name,31); rp.pos=Vec3(px,py,pz); rp.yaw=yaw; rp.pitch=pitch;
                rp.cam=cam; rp.hp=hp; rp.sel=sel; rp.last=g_time;
                g_connected=true;
            }
        } else if(strncmp(buf,"C|",2)==0){ // chat
            char name[32],txt[512];
            if(sscanf(buf+2,"%31[^|]|%511[^\n]",name,txt)==2){
                g_chatLog.push_back(wstring(L"[")+wstring(name,name+strlen(name))+L"] "+wstring(txt,txt+strlen(txt)));
                if((int)g_chatLog.size()>80) g_chatLog.erase(g_chatLog.begin());
            }
        }
        n=recvfrom(g_sock,buf,sizeof(buf),0,(sockaddr*)&from,&fl);
    }
    // send state to connected peers (10 Hz)
    if(g_connected&&g_stateTimer<=0){
        g_stateTimer=0.1f;
        char pkt[256];
        sprintf(pkt,"P|%S|%.2f|%.2f|%.2f|%.2f|%.2f|%d|%d|%d",g_playerName.c_str(),
            g_pos.x,g_pos.y,g_pos.z,g_yaw,g_pitch,g_cam,(int)g_hp,g_hotbarSel);
        for(auto&kv:g_peers){
            const char* k=kv.first.c_str();
            const char* colon=strchr(k,':'); if(!colon) continue;
            char ip[64]; int len=(int)(colon-k); if(len>63) len=63;
            memcpy(ip,k,len); ip[len]=0;
            int port=atoi(colon+1); if(port<=0) continue;
            sockaddr_in sa; memset(&sa,0,sizeof(sa)); sa.sin_family=AF_INET;
            sa.sin_addr.s_addr=inet_addr(ip); sa.sin_port=htons((unsigned short)port);
            NetSendTo(sa,pkt,(int)strlen(pkt));
        }
    }
}
static void NetChat(const wstring&txt){
    if(txt.empty()) return;
    g_chatLog.push_back(L"你: "+txt);
    if((int)g_chatLog.size()>80) g_chatLog.erase(g_chatLog.begin());
    if(g_connected){
        char pkt[1024];
        sprintf(pkt,"C|%S|%S",g_playerName.c_str(),txt.c_str());
        for(auto&kv:g_peers){
            const char* k=kv.first.c_str();
            const char* colon=strchr(k,':'); if(!colon) continue;
            char ip[64]; int len=(int)(colon-k); if(len>63) len=63;
            memcpy(ip,k,len); ip[len]=0;
            int port=atoi(colon+1); if(port<=0) continue;
            sockaddr_in sa; memset(&sa,0,sizeof(sa)); sa.sin_family=AF_INET;
            sa.sin_addr.s_addr=inet_addr(ip); sa.sin_port=htons((unsigned short)port);
            NetSendTo(sa,pkt,(int)strlen(pkt));
        }
    }
}
static void NetJoin(const char* ip,int port){
    sockaddr_in sa; memset(&sa,0,sizeof(sa)); sa.sin_family=AF_INET;
    sa.sin_addr.s_addr=inet_addr(ip); sa.sin_port=htons((unsigned short)port);
    char pkt[64]; sprintf(pkt,"HSB|%S",g_playerName.c_str());
    for(int i=0;i<20;i++) NetSendTo(sa,pkt,(int)strlen(pkt)); // hole punch probes
}

// ==================== messages ====================
static void AddMsg(const wstring& s){ g_msgs.push_back(s); if((int)g_msgs.size()>6) g_msgs.erase(g_msgs.begin()); }
static void AddView(const wstring& s){ MsgT m; m.s=s; m.t=4.0f; g_views.push_back(m); if((int)g_views.size()>8) g_views.erase(g_views.begin()); }

// ==================== inventory & items ====================
static int HasItem(int id){ for(size_t i=0;i<g_inv.size();i++) if(g_inv[i].id==id&&g_inv[i].cnt>0) return (int)i; return -1; }
static bool AddItem(int id,int cnt){
    for(size_t i=0;i<g_inv.size();i++) if(g_inv[i].id==id&&g_inv[i].cnt<99){ g_inv[i].cnt+=cnt; return true; }
    for(size_t i=0;i<g_inv.size();i++) if(g_inv[i].id==0){ g_inv[i].id=id; g_inv[i].cnt=cnt; return true; }
    return false;
}
static void SpawnWorld(){
    g_items.clear();
    int nw=14, nm=10;
    for(int i=0;i<nw;i++){
        int gi=(int)(WRand()*40-20), gj=(int)(WRand()*40-20);
        if(CellSolid(gi,gj)) continue;
        WorldItem it; it.type=1;
        it.pos=Vec3(gi*CELLSZ+1.5f+WRand()*5.0f, FloorAt(gi*CELLSZ+4, gj*CELLSZ+4), gj*CELLSZ+1.5f+WRand()*5.0f);
        it.rot=WRand()*6.28f; it.taken=false; g_items.push_back(it);
    }
    for(int i=0;i<nm;i++){
        int gi=(int)(WRand()*40-20), gj=(int)(WRand()*40-20);
        if(CellSolid(gi,gj)) continue;
        WorldItem it; it.type=2;
        it.pos=Vec3(gi*CELLSZ+1.5f+WRand()*5.0f, FloorAt(gi*CELLSZ+4, gj*CELLSZ+4), gj*CELLSZ+1.5f+WRand()*5.0f);
        it.rot=WRand()*6.28f; it.taken=false; g_items.push_back(it);
    }
}
static Vec3 SpawnPos(){
    for(int r=0;r<40;r++){
        int gi=0, gj=0;
        if(r==0){ gi=0;gj=0; }
        else { gi=(int)(WRand()*24-12); gj=(int)(WRand()*24-12); }
        if(!CellSolid(gi,gj)) return Vec3(gi*CELLSZ+CELLSZ/2, FloorAt(gi*CELLSZ+CELLSZ/2,gj*CELLSZ+CELLSZ/2), gj*CELLSZ+CELLSZ/2);
    }
    return Vec3(0,0,0);
}
static void GiveAKM(){
    if(AddItem(5,1)){ AddMsg(L"获得 AKM 步枪"); Sfx(L"sfx_pick.wav"); }
    else AddMsg(L"背包已满，无法获得 AKM");
}
static void UseItem(int idx){
    if(idx<0||idx>=(int)g_inv.size()) return;
    int id=g_inv[idx].id;
    if(id==1){ // 银杏水(杏仁水)
        g_thr=ClampF(g_thr+50,0,100); g_san=ClampF(g_san+20,0,100);
        g_hp=ClampF(g_hp+2,0,100); g_hun=ClampF(g_hun+1,0,100);
        g_drinkAnim=1.0f; Sfx(L"sfx_drink.wav"); AddMsg(L"饮下银杏水：口渴+50% 理智+20 生命+2");
        g_inv[idx].cnt--; if(g_inv[idx].cnt<=0){ g_inv[idx].id=0; g_inv[idx].cnt=0; }
    } else if(id==2){ // 肉块
        g_hun=ClampF(g_hun+10,0,100); g_hp=ClampF(g_hp+4,0,100);
        g_san=ClampF(g_san-1,0,100); g_xp=ClampF(g_xp+1,0,100);
        g_eatAnim=1.0f; Sfx(L"sfx_eat.wav"); AddMsg(L"吃掉腐肉：饥饿+10% 生命+4 理智-1 经验+1");
        g_inv[idx].cnt--; if(g_inv[idx].cnt<=0){ g_inv[idx].id=0; g_inv[idx].cnt=0; }
        if(g_xp>=100){ g_xp-=100; g_level++; g_capacity=39+g_level; Sfx(L"sfx_level.wav");
            AddView(L"升级！背包扩容至 40+"+to_wstring(g_level-1)+L" 格"); }
    } else if(id==5){ AddMsg(L"AKM：左键开火  R 换弹"); }
    else if(id==4){ AddMsg(L"《生存手册》：后室求生要点——寻找杏仁水恢复理智，警惕腐肉，别回头。"); }
}
static void TryPickup(){
    Vec3 eye=g_pos+Vec3(0,1.5f,0);
    for(size_t i=0;i<g_items.size();i++){
        WorldItem&it=g_items[i];
        if(it.taken) continue;
        float dx=it.pos.x-eye.x, dy=it.pos.y-eye.y, dz=it.pos.z-eye.z;
        if(dx*dx+dy*dy+dz*dz<3.2f){
            if(AddItem(it.type,1)){
                it.taken=true;
                Sfx(L"sfx_pick.wav");
                AddMsg(it.type==1?L"拾取 银杏水(杏仁水)":L"拾取 腐肉块");
                return;
            } else { AddMsg(L"背包已满！"); return; }
        }
    }
    AddMsg(L"附近没有可拾取的物品");
}
static void DropToWorld(int id,int cnt){
    Vec3 f(-sinf(g_yaw)*cosf(g_pitch),sinf(g_pitch),-cosf(g_yaw)*cosf(g_pitch));
    WorldItem it; it.type=id; it.pos=g_pos+Vec3(0,1.0f,0)+f*1.2f; it.rot=g_time; it.taken=false;
    g_items.push_back(it);
    Sfx(L"sfx_drop.wav");
    (void)cnt;
}

// ==================== input ====================
static bool KeyDn(int v){ if(simKey[v]) return true; return (GetAsyncKeyState(v)&0x8000)!=0; }
static int g_prevF=0,g_prevF5=0,g_prevE=0,g_prevQ=0,g_prevT=0,g_prevEsc=0,g_prevF9=0,g_prevR=0,g_prevO=0,g_prevUp=0,g_prevDown=0,g_prevEnter=0;
static bool g_prevL=false,g_prevRbtn=false;

// ==================== chat command ====================
static void RunCommand(const wstring&cmd){
    wstring c=cmd;
    if(c.size()>1&&c[0]==L'/'){
        if(c.find(L"/giop")==0&&c.find(L"akm")!=wstring::npos){ GiveAKM(); }
        else if(c.find(L"/help")==0){ AddMsg(L"指令: /giop @s akm 获得AKM  /join IP:端口 加入联机  /host 公开广播"); }
        else if(c.find(L"/join")==0){
            size_t p=c.find(L' ',4);
            if(p!=wstring::npos){
                wstring addr=c.substr(p+1);
                size_t colon=addr.rfind(L':');
                if(colon!=wstring::npos){
                    string ip(addr.substr(0,colon).begin(),addr.substr(0,colon).end());
                    int port=_wtoi(addr.substr(colon+1).c_str());
                    if(port>0){ NetJoin(ip.c_str(),port); AddMsg(L"正在尝试穿透连接 "+addr+L" ..."); }
                    else AddMsg(L"格式: /join IP:端口");
                } else AddMsg(L"格式: /join IP:端口");
            }
        }
        else if(c.find(L"/host")==0){ g_broadcastOn=true; AddMsg(L"已开启局域网广播，等待玩家加入"); }
        else AddMsg(L"未知指令，输入 /help 查看");
    }
}

// ==================== shooting ====================
static bool g_firing=false; static float g_recoil=0; static int g_muzzleT=0;
static Vec3 g_tracerFrom,g_tracerTo; static float g_tracerT=0;
static void Shoot(){
    if(g_ammo<=0){ Sfx(L"sfx_click.wav"); AddMsg(L"弹药耗尽，按 R 换弹"); return; }
    if(g_reloadT>0) return;
    g_ammo--; g_recoil=1.0f; g_muzzleT=2; Sfx(L"sfx_gun.wav");
    Vec3 eye=g_pos+Vec3(0,1.5f,0);
    Vec3 dir(-sinf(g_yaw)*cosf(g_pitch),sinf(g_pitch),-cosf(g_yaw)*cosf(g_pitch));
    g_tracerFrom=eye+g_pos*0; g_tracerFrom=eye;
    g_tracerTo=eye+dir*60.0f; g_tracerT=0.08f;
    // ray vs walls
    vector<AABB> ws; CollectNearWalls(g_pos,ws,3.0f);
    float bestT=60.0f;
    for(size_t i=0;i<ws.size();i++){
        float t; if(RayAABB(eye,dir,ws[i],t)&&t<bestT){ bestT=t; }
    }
    // also check far chunks in direction (up to 3 chunks away)
    for(int dz=-3;dz<=3;dz++)for(int dx=-3;dx<=3;dx++){
        int key=ChunkKey((int)floorf(g_pos.x/CHUNKSZ)+dx,(int)floorf(g_pos.z/CHUNKSZ)+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()) continue;
        for(size_t i=0;i<it->second.walls.size();i++){
            float t; if(RayAABB(eye,dir,it->second.walls[i],t)&&t<bestT) bestT=t;
        }
    }
    g_tracerTo=eye+dir*bestT;
    if(bestT<60.0f) Sfx(L"sfx_wall.wav");
}
static void StartReload(){ if(g_reloadT==0&&g_ammo<30){ g_reloadT=120; Sfx(L"sfx_reload.wav"); } }

// ==================== save / load ====================
static void SaveGame(){
    FILE* f=_wfopen((g_exeDir+L"\\save.dat").c_str(),L"wb");
    if(!f) return;
    fwrite("HS002",1,5,f);
    fwrite(&g_seed,4,1,f);
    char nm[128]={0};
    WideCharToMultiByte(CP_UTF8,0,g_playerName.c_str(),-1,nm,127,NULL,NULL);
    fwrite(nm,1,128,f);
    fwrite(&g_pos,12,1,f); fwrite(&g_yaw,4,1,f); fwrite(&g_pitch,4,1,f);
    float st[5]={g_hp,g_san,g_hun,g_thr,g_sta}; fwrite(st,4,5,f);
    fwrite(&g_xp,4,1,f); fwrite(&g_level,4,1,f); fwrite(&g_ammo,4,1,f);
    int fo=g_flashOn?1:0; fwrite(&fo,4,1,f);
    int n=(int)g_inv.size(); fwrite(&n,4,1,f);
    for(int i=0;i<n;i++){ fwrite(&g_inv[i].id,4,1,f); fwrite(&g_inv[i].cnt,4,1,f); }
    int m=(int)g_items.size(); fwrite(&m,4,1,f);
    for(int i=0;i<m;i++){ fwrite(&g_items[i].type,4,1,f); fwrite(&g_items[i].pos,12,1,f); fwrite(&g_items[i].rot,4,1,f); }
    fclose(f);
}
static bool LoadGame(){
    FILE* f=_wfopen((g_exeDir+L"\\save.dat").c_str(),L"rb");
    if(!f) return false;
    char mg[6]={0}; fread(mg,1,5,f);
    if(memcmp(mg,"HS002",5)!=0){ fclose(f); return false; }
    fread(&g_seed,4,1,f);
    char nm[128]={0}; fread(nm,1,128,f);
    wchar_t nmw[64]; int nw=MultiByteToWideChar(CP_UTF8,0,nm,-1,nmw,63);
    g_playerName=(nw>0)?wstring(nmw):L"幸存者";
    fread(&g_pos,12,1,f); fread(&g_yaw,4,1,f); fread(&g_pitch,4,1,f);
    float st[5]; fread(st,4,5,f); g_hp=st[0];g_san=st[1];g_hun=st[2];g_thr=st[3];g_sta=st[4];
    fread(&g_xp,4,1,f); fread(&g_level,4,1,f); fread(&g_ammo,4,1,f);
    int fo; fread(&fo,4,1,f); g_flashOn=fo?true:false;
    g_capacity=39+g_level;
    g_inv.assign(g_capacity,InvItem{0,0});
    int n; fread(&n,4,1,f);
    for(int i=0;i<n&&i<g_capacity;i++){ fread(&g_inv[i].id,4,1,f); fread(&g_inv[i].cnt,4,1,f); }
    int m; fread(&m,4,1,f);
    g_items.clear();
    for(int i=0;i<m;i++){ WorldItem it; fread(&it.type,4,1,f); fread(&it.pos,12,1,f); fread(&it.rot,4,1,f); it.taken=false; g_items.push_back(it); }
    fclose(f); return true;
}

// ==================== update ====================
static void UpdateGame(float dt){
    g_time+=dt; g_dt=dt;
    for(size_t i=0;i<g_views.size();i++) g_views[i].t-=dt;
    for(size_t i=0;i<g_views.size();) { if(g_views[i].t<=0) g_views.erase(g_views.begin()+i); else i++; }
    if(g_textCache.size()>500) g_textCache.clear();
    if(g_eatAnim>0) g_eatAnim=MaxF(0,g_eatAnim-dt*2.5f);
    if(g_drinkAnim>0) g_drinkAnim=MaxF(0,g_drinkAnim-dt*2.5f);
    if(g_recoil>0) g_recoil=MaxF(0,g_recoil-dt*4.0f);
    if(g_muzzleT>0) g_muzzleT--;
    if(g_tracerT>0) g_tracerT-=dt;
    if(g_reloadT>0){ g_reloadT--; if(g_reloadT==1){ g_ammo=30; AddMsg(L"换弹完成"); } }

    // chat input
    int nT=KeyDn('T')?1:0;
    if(nT&&!g_prevT&&g_gamestate==GAME_PLAY&&!g_invOpen){ g_chatOpen=!g_chatOpen; g_chatBuf.clear(); }
    g_prevT=nT;
    if(g_chatOpen){
        if(KeyDn(VK_ESCAPE)&&!g_prevEsc){ g_chatOpen=false; }
        g_prevEsc=KeyDn(VK_ESCAPE)?1:0;
        if(KeyDn(VK_RETURN)&&!g_prevEnter){
            wstring txt=g_chatBuf;
            if(!txt.empty()){
                if(txt[0]==L'/') RunCommand(txt);
                else NetChat(txt);
            }
            g_chatBuf.clear(); g_chatOpen=false;
        }
        g_prevEnter=KeyDn(VK_RETURN)?1:0;
        return;
    }
    // pause
    int nEsc=KeyDn(VK_ESCAPE)?1:0;
    if(nEsc&&!g_prevEsc){
        if(g_gamestate==GAME_PLAY&&!g_invOpen) g_gamestate=GAME_PAUSE;
    }
    g_prevEsc=nEsc;
    if(g_gamestate==GAME_PAUSE){
        // menu navigation via arrows + enter handled in UpdateMenu
        return;
    }
    if(g_gamestate==GAME_PLAY){
        // key edges
        int nF=KeyDn('F')?1:0, nF5=KeyDn(VK_F5)?1:0, nE=KeyDn('E')?1:0,
            nQ=KeyDn('Q')?1:0, nF9=KeyDn(VK_F9)?1:0, nR=KeyDn('R')?1:0;
        if(nF&&!g_prevF&&!g_invOpen){ g_flashOn=!g_flashOn; Sfx(L"sfx_click.wav"); AddMsg(g_flashOn?L"手电筒已开启":L"手电筒已关闭"); }
        g_prevF=nF;
        if(nF5&&!g_prevF5){ g_cam=(g_cam+1)%3;
            AddMsg(g_cam==CM_FP?L"第一人称视角":(g_cam==CM_TP?L"第三人称视角":L"上帝视角(仅相机)")); }
        g_prevF5=nF5;
        if(nE&&!g_prevE){
            if(g_invOpen){ g_invOpen=false; g_selSlot=-1; }
            else { g_invOpen=true; }
        }
        g_prevE=nE;
        // hotbar number keys
        for(int k=0;k<9;k++){
            if(KeyDn('1'+k)){
                g_hotbarSel=k;
                if(g_hotbarSel<(int)g_inv.size()) AddView(ItemName(g_inv[g_hotbarSel].id));
                break;
            }
        }
        if(nQ&&!g_prevQ){
            if(!g_invOpen){
                // Q in gameplay: drop selected hotbar item
                if(g_hotbarSel<(int)g_inv.size()&&g_inv[g_hotbarSel].id>0){
                    DropToWorld(g_inv[g_hotbarSel].id,1);
                    g_inv[g_hotbarSel].id=0; g_inv[g_hotbarSel].cnt=0;
                }
            } else {
                if(g_dragId>0){ DropToWorld(g_dragId,g_dragCnt); g_dragId=0; }
                else if(g_hoverSlot>=0&&g_inv[g_hoverSlot].id>0){
                    if(g_selSlot==g_hoverSlot){ DropToWorld(g_inv[g_hoverSlot].id,g_inv[g_hoverSlot].cnt);
                        g_inv[g_hoverSlot].id=0; g_inv[g_hoverSlot].cnt=0; g_selSlot=-1; }
                    else g_selSlot=g_hoverSlot;
                }
            }
        }
        g_prevQ=nQ;
        if(nF9&&!g_prevF9){ SaveGame(); AddMsg(L"存档成功 (F9)"); Sfx(L"sfx_pick.wav"); }
        g_prevF9=nF9;
        if(nR&&!g_prevR){ StartReload(); }
        g_prevR=nR;
        // inventory mouse
        if(g_invOpen){
            POINT mp; GetCursorPos(&mp); ScreenToClient(g_hWnd,&mp);
            bool nL=KeyDn(VK_LBUTTON)?true:false, nRb=KeyDn(VK_RBUTTON)?true:false;
            if(nL&&!g_prevL){
                if(g_hoverSlot>=0){
                    if(g_dragId==0&&g_inv[g_hoverSlot].id>0){
                        g_dragId=g_inv[g_hoverSlot].id; g_dragCnt=g_inv[g_hoverSlot].cnt;
                        g_inv[g_hoverSlot].id=0; g_inv[g_hoverSlot].cnt=0; g_selSlot=-1;
                    } else if(g_dragId>0){
                        if(g_inv[g_hoverSlot].id==0){ g_inv[g_hoverSlot].id=g_dragId; g_inv[g_hoverSlot].cnt=g_dragCnt; g_dragId=0; }
                        else { int ti=g_inv[g_hoverSlot].id, tc=g_inv[g_hoverSlot].cnt;
                            g_inv[g_hoverSlot].id=g_dragId; g_inv[g_hoverSlot].cnt=g_dragCnt;
                            g_dragId=ti; g_dragCnt=tc; }
                    }
                } else if(g_dragId>0){ g_dragId=0; }
            }
            if(nRb&&!g_prevRbtn&&g_hoverSlot>=0&&g_dragId==0) UseItem(g_hoverSlot);
            g_prevL=nL; g_prevRbtn=nRb;
        } else {
            bool nL=KeyDn(VK_LBUTTON)?true:false;
            if(nL&&!g_prevL){
                // shoot or pickup
                if(g_hotbarSel<(int)g_inv.size()&&g_inv[g_hotbarSel].id==5) Shoot();
                else TryPickup();
            }
            g_prevL=nL;
        }
        // movement & physics (disabled while inventory open)
        if(!g_invOpen){
            Vec3 f(-sinf(g_yaw),0,-cosf(g_yaw)), r(cosf(g_yaw),0,-sinf(g_yaw));
            Vec3 mv(0,0,0);
            if(KeyDn('W')) mv=mv+f; if(KeyDn('S')) mv=mv-f;
            if(KeyDn('A')) mv=mv-r; if(KeyDn('D')) mv=mv+r;
            bool sprint=KeyDn(VK_SHIFT)&&g_sta>2.0f&&mv.len()>0.1f;
            float spd=sprint?7.0f:4.2f;
            if(mv.len()>0.01f){
                mv=mv.norm()*spd;
                MovePlayer(mv.x*dt,mv.z*dt);
                g_walkT+=spd*dt;
                g_stepT-=dt;
                if(g_stepT<=0){ g_stepT=sprint?0.34f:0.46f; Sfx(L"sfx_step.wav"); }
                if(sprint) g_sta=ClampF(g_sta-13.0f*dt,0,100);
            } else {
                g_sta=ClampF(g_sta+9.0f*dt,0,100);
            }
            if(g_onGround&&KeyDn(VK_SPACE)){ g_velY=6.0f; g_onGround=false; g_sta=ClampF(g_sta-5.0f,0,100); }
            StepPhysics(dt);
        }
        // stats decay (always)
        g_thr=ClampF(g_thr-1.0f/9.0f*dt,0,100);
        g_hun=ClampF(g_hun-1.0f/22.0f*dt,0,100);
        if(g_flashOn) g_san=ClampF(g_san+0.4f*dt,0,100);
        else g_san=ClampF(g_san-1.0f/9.0f*dt,0,100);
        if(g_hun<=0||g_thr<=0) g_hp=ClampF(g_hp-2.2f*dt,0,100);
        if(g_hp<=0){ g_hp=0; g_dead=true; g_gamestate=GAME_DEAD; Sfx(L"sfx_death.wav"); }
        UpdateChunks();
    }
}
// menu update (menu/pause/dead): arrow select + enter
static void UpdateMenu(float dt){
    g_dt=dt;
    if(g_textCache.size()>500) g_textCache.clear();
    if(KeyDn(VK_UP)&&!g_prevUp){ if(g_gamestate==GAME_MENU){ g_menuSel=(g_menuSel+2)%3; } else g_pauseSel=(g_pauseSel+2)%3; Sfx(L"sfx_click.wav"); }
    if(KeyDn(VK_DOWN)&&!g_prevDown){ if(g_gamestate==GAME_MENU){ g_menuSel=(g_menuSel+1)%3; } else g_pauseSel=(g_pauseSel+1)%3; Sfx(L"sfx_click.wav"); }
    g_prevUp=KeyDn(VK_UP)?1:0; g_prevDown=KeyDn(VK_DOWN)?1:0;
    if(KeyDn(VK_RETURN)&&!g_prevEnter){
        g_prevEnter=1;
        if(g_gamestate==GAME_MENU){
            if(g_menuSel==0){ // new game
                if(g_nameBuf.empty()) g_nameBuf=L"幸存者";
                g_playerName=g_nameBuf;
                g_seed=(int)(GetTickCount()^1234567);
                g_wr=g_seed*7919u+77u;
                g_hp=100;g_san=100;g_hun=100;g_thr=100;g_sta=100;g_xp=0;
                g_level=1; g_capacity=40; g_ammo=30; g_flashOn=true;
                g_pos=SpawnPos(); g_yaw=0; g_pitch=0; g_velY=0; g_onGround=true; g_dead=false;
                g_inv.assign(g_capacity,InvItem{0,0});
                g_inv[0].id=3; g_inv[0].cnt=1;
                g_inv[1].id=4; g_inv[1].cnt=1;
                g_inv[2].id=1; g_inv[2].cnt=2;
                g_inv[3].id=2; g_inv[3].cnt=2;
                SpawnWorld();
                g_chunks.clear();
                g_gamestate=GAME_PLAY;
                g_cam=CM_FP;
                AddMsg(L"欢迎来到《后世》。你被困在了无限迷宫中。");
                AddMsg(L"WASD移动 空格跳 Shift疾跑 E背包 数字键切换物品 F手电 F5视角 T聊天");
                AddMsg(L"指令: /giop @s akm 获得步枪  /join IP:端口 联机");
            } else if(g_menuSel==1){ // continue
                if(LoadGame()){
                    g_chunks.clear();
                    g_gamestate=GAME_PLAY; g_dead=false; g_cam=CM_FP;
                    AddMsg(L"已读取存档，欢迎回到《后世》");
                } else AddMsg(L"没有找到存档！请先创建新游戏");
            } else { // quit
                SaveGame(); PostQuitMessage(0);
            }
        } else if(g_gamestate==GAME_PAUSE){
            if(g_pauseSel==0){ g_gamestate=GAME_PLAY; }
            else if(g_pauseSel==1){ SaveGame(); g_gamestate=GAME_MENU; g_menuSel=1; }
            else { SaveGame(); PostQuitMessage(0); }
        } else if(g_gamestate==GAME_DEAD){
            g_hp=100; g_san=100; g_hun=100; g_thr=100; g_sta=100;
            g_pos=SpawnPos(); g_yaw=0; g_pitch=0; g_velY=0; g_onGround=true;
            g_dead=false; g_gamestate=GAME_PLAY;
            AddMsg(L"你从昏迷中醒来……");
        }
    } else g_prevEnter=0;
    // escape toggles pause (from update when needed)
    if(g_gamestate==GAME_PAUSE){
        int nEsc=KeyDn(VK_ESCAPE)?1:0;
        if(nEsc&&!g_prevEsc){ g_gamestate=GAME_PLAY; }
        g_prevEsc=nEsc;
    }
    if(g_gamestate==GAME_MENU){
        // name input via WM_CHAR buffer is appended in WndProc
        if(!g_nameBuf.empty()&&g_nameBuf[g_nameBuf.size()-1]==L'\r') g_nameBuf.pop_back();
    }
}

// ==================== shaders ====================
static const char* VS_W =
"#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"layout(location=1) in vec3 aNrm;\n"
"layout(location=2) in vec2 aUv;\n"
"uniform mat4 uMVP; uniform mat4 uModel; uniform mat4 uNorm;\n"
"out vec3 wPos; out vec3 wNrm; out vec2 vUv;\n"
"void main(){ wPos=(uModel*vec4(aPos,1.0)).xyz; wNrm=normalize(mat3(uNorm)*aNrm); vUv=aUv; gl_Position=uMVP*vec4(aPos,1.0); }\n";
static const char* FS_W =
"#version 330 core\n"
"in vec3 wPos; in vec3 wNrm; in vec2 vUv;\n"
"uniform vec3 uCamPos; uniform vec3 uAmb; uniform vec3 uFogColor; uniform float uFogD;\n"
"uniform sampler2D uTex; uniform vec3 uTint;\n"
"uniform int uLNum; uniform vec3 uLPos[8]; uniform vec3 uLCol[8]; uniform float uLR[8];\n"
"uniform vec3 uFlashDir; uniform vec3 uFlashPos; uniform float uFlashOn;\n"
"uniform mat4 uShadowMVP; uniform sampler2DShadow uShadow; uniform float uShadowOn;\n"
"out vec4 outCol;\n"
"float shadowAt(vec3 p){\n"
"  vec4 sc=uShadowMVP*vec4(p,1.0); sc.xyz/=sc.w;\n"
"  float sh=1.0;\n"
"  if(sc.z>0.0&&sc.z<1.0&&sc.x>-1.0&&sc.x<1.0&&sc.y>-1.0&&sc.y<1.0){\n"
"    vec2 uv=sc.xy*0.5+0.5; float bias=0.004; vec2 ts=1.0/vec2(1024.0);\n"
"    for(int x=-1;x<=1;x++)for(int y=-1;y<=1;y++)\n"
"      sh+=texture(uShadow, vec3(uv+vec2(float(x),float(y))*ts, sc.z-bias));\n"
"    sh/=9.0;\n"
"  }\n"
"  return sh;\n"
"}\n"
"void main(){\n"
"  vec3 n=normalize(wNrm);\n"
"  vec3 alb=texture(uTex,vUv).rgb*uTint;\n"
"  vec3 col=uAmb*alb;\n"
"  vec3 v=normalize(uCamPos-wPos);\n"
"  for(int i=0;i<8;i++){\n"
"    if(i>=uLNum) break;\n"
"    vec3 d=uLPos[i]-wPos; float dist=length(d);\n"
"    float att=clamp(1.0-dist/uLR[i],0.0,1.0); att=att*att;\n"
"    vec3 l=d/dist;\n"
"    float ndl=max(dot(n,l),0.0);\n"
"    col+=uLCol[i]*alb*ndl*att*1.6;\n"
"    float sp=pow(max(dot(reflect(-l,n),v),0.0),24.0);\n"
"    col+=uLCol[i]*sp*att*0.5;\n"
"  }\n"
"  if(uFlashOn>0.5){\n"
"    vec3 l=normalize(uFlashPos-wPos);\n"
"    float ndl=max(dot(n,l),0.0);\n"
"    float cone=max(dot(l,normalize(uFlashDir)),0.0);\n"
"    float f=pow(cone,14.0);\n"
"    float dist=length(uFlashPos-wPos);\n"
"    float att=clamp(1.0-dist/30.0,0.0,1.0); att=att*att;\n"
"    float sh=uShadowOn>0.5?shadowAt(wPos):1.0;\n"
"    vec3 fl=vec3(1.0,0.96,0.85);\n"
"    col+=fl*alb*ndl*f*att*sh*2.2;\n"
"    col+=fl*pow(max(dot(reflect(-l,n),v),0.0),24.0)*f*att*sh*0.9;\n"
"  }\n"
"  float dist=length(uCamPos-wPos);\n"
"  float fog=1.0-exp(-dist*uFogD);\n"
"  col=mix(col,uFogColor,fog*0.85);\n"
"  outCol=vec4(col,1.0);\n"
"}\n";
static const char* VS_SHADOW =
"#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"uniform mat4 uMVP;\n"
"void main(){ gl_Position=uMVP*vec4(aPos,1.0); }\n";
static const char* FS_SHADOW =
"#version 330 core\n"
"out float d;\n"
"void main(){ d=gl_FragCoord.z; }\n";
static const char* VS_UI =
"#version 330 core\n"
"layout(location=0) in vec2 aPos;\n"
"layout(location=1) in vec2 aUv;\n"
"uniform mat4 uMVP;\n"
"out vec2 vUv;\n"
"void main(){ vUv=aUv; gl_Position=uMVP*vec4(aPos,0.0,1.0); }\n";
static const char* FS_UI =
"#version 330 core\n"
"in vec2 vUv; uniform sampler2D uTex; uniform vec4 uColor;\n"
"out vec4 outCol;\n"
"void main(){ outCol=uColor*texture(uTex,vUv); }\n";
static GLuint progW=0,progShadow=0,progUI=0;
static GLuint fboShadow=0,texShadow=0;
static GLint U(GLuint p,const char* n){ return glGetUniformLocation(p,n); }
static GLuint CompileShader(GLenum type,const char* src){
    GLuint s=glCreateShader(type);
    glShaderSource(s,1,&src,NULL); glCompileShader(s);
    GLint ok=0; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){ char log[2048]={0}; glGetShaderInfoLog(s,2047,NULL,log);
        FILE* lf=fopen("shader.log","a");
        if(lf){ fprintf(lf,"[shader type=%d]\n%s\n---\n",(int)type,log); fclose(lf); }
        glDeleteShader(s); return 0; }
    return s;
}
static GLuint LinkProg(const char* vs,const char* fs,const char* attrs[],int nattr){
    GLuint p=glCreateProgram();
    GLuint v=CompileShader(GL_VERTEX_SHADER,vs);
    GLuint f=CompileShader(GL_FRAGMENT_SHADER,fs);
    if(!v||!f) return 0;
    for(int i=0;i<nattr;i++) glBindAttribLocation(p,i,attrs[i]);
    glAttachShader(p,v); glAttachShader(p,f); glLinkProgram(p);
    GLint ok=0; glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok){ char log[2048]={0}; glGetProgramInfoLog(p,2047,NULL,log);
        FILE* lf=fopen("shader.log","a");
        if(lf){ fprintf(lf,"[link]\n%s\n---\n",log); fclose(lf); }
        return 0; }
    glDeleteShader(v); glDeleteShader(f); return p;
}
static void CompilePrograms(){
    const char* attrs3[]={"aPos","aNrm","aUv"};
    const char* attrs2[]={"aPos","aUv"};
    progW=LinkProg(VS_W,FS_W,attrs3,3);
    progShadow=LinkProg(VS_SHADOW,FS_SHADOW,attrs3,1);
    progUI=LinkProg(VS_UI,FS_UI,attrs2,2);
}
static void MakeShadowFBO(){
    glGenFramebuffers(1,&fboShadow); glBindFramebuffer(GL_FRAMEBUFFER,fboShadow);
    glGenTextures(1,&texShadow); glBindTexture(GL_TEXTURE_2D,texShadow);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,1024,1024,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,texShadow,0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

// ==================== render state ====================
static Mat4 g_uiMvp;
static Vec3 g_lightsPos[8]; static Vec3 g_lightsCol[8]; static float g_lightsR[8]; static int g_lightsN=0;
static void GatherLights(){
    g_lightsN=0;
    vector<Vec3> cand;
    int pcx=(int)floorf(g_camPos.x/CHUNKSZ), pcz=(int)floorf(g_camPos.z/CHUNKSZ);
    for(int dz=-3;dz<=3;dz++)for(int dx=-3;dx<=3;dx++){
        int key=ChunkKey(pcx+dx,pcz+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()) continue;
        const Chunk&ch=it->second;
        for(size_t i=0;i<ch.panels.size();i++){
            Vec3 p=ch.panels[i];
            float d=(p-g_camPos).len();
            if(d<45.0f) cand.push_back(p);
        }
    }
    // simple sort by distance (insertion for first 8)
    for(size_t i=0;i<cand.size()&&g_lightsN<8;i++){
        Vec3 p=cand[i];
        float d=(p-g_camPos).len();
        int gi=(int)floorf(p.x/CELLSZ), gj=(int)floorf(p.z/CELLSZ);
        bool fl=CellFlicker(gi,gj);
        float flick=fl?(0.55f+0.45f*(0.5f+0.5f*sinf(g_time*9.0f+Hash01(gi,gj)*40.0f))):1.0f;
        g_lightsPos[g_lightsN]=p; g_lightsCol[g_lightsN]=Vec3(0.9f,0.93f,1.0f)*flick;
        g_lightsR[g_lightsN]=16.0f; g_lightsN++;
    }
    // muzzle flash light
    if(g_muzzleT>0){
        Vec3 eye=g_camPos;
        Vec3 dir(-sinf(g_yaw)*cosf(g_pitch),sinf(g_pitch),-cosf(g_yaw)*cosf(g_pitch));
        g_lightsPos[g_lightsN]=eye+dir*1.4f; g_lightsCol[g_lightsN]=Vec3(1.0f,0.75f,0.3f);
        g_lightsR[g_lightsN]=8.0f; if(g_lightsN<7) g_lightsN++;
    }
}
static Mat4 g_shadowMVP;
static void SetWorldUniforms(GLuint p,const Mat4&vp,const Mat4&model,const Mat4&normM,GLuint tex,const Vec3&tint){
    Mat4 mvp=M4Mul(vp,model);
    glUniformMatrix4fv(U(p,"uMVP"),1,GL_FALSE,mvp.m);
    glUniformMatrix4fv(U(p,"uModel"),1,GL_FALSE,model.m);
    glUniformMatrix4fv(U(p,"uNorm"),1,GL_FALSE,normM.m);
    glUniform3f(U(p,"uCamPos"),g_camPos.x,g_camPos.y,g_camPos.z);
    glUniform3f(U(p,"uAmb"),0.14f,0.14f,0.16f);
    glUniform3f(U(p,"uFogColor"),0.05f,0.045f,0.04f);
    glUniform1f(U(p,"uFogD"),1.0f/55.0f);
    glUniform3f(U(p,"uTint"),tint.x,tint.y,tint.z);
    glUniform1i(U(p,"uLNum"),g_lightsN);
    for(int i=0;i<8;i++){
        char nm[32]; sprintf(nm,"uLPos[%d]",i); glUniform3f(U(p,nm),g_lightsPos[i].x,g_lightsPos[i].y,g_lightsPos[i].z);
        sprintf(nm,"uLCol[%d]",i); glUniform3f(U(p,nm),g_lightsCol[i].x,g_lightsCol[i].y,g_lightsCol[i].z);
        sprintf(nm,"uLR[%d]",i); glUniform1f(U(p,nm),g_lightsR[i]);
    }
    Vec3 fd(-sinf(g_yaw)*cosf(g_pitch),sinf(g_pitch),-cosf(g_yaw)*cosf(g_pitch));
    Vec3 fp=g_cam==CM_FP?g_camPos:g_pos+Vec3(0,1.6f,0);
    glUniform3f(U(p,"uFlashDir"),fd.x,fd.y,fd.z);
    glUniform3f(U(p,"uFlashPos"),fp.x,fp.y,fp.z);
    glUniform1f(U(p,"uFlashOn"),g_flashOn?1.0f:0.0f);
    glUniform1f(U(p,"uShadowOn"),(g_flashOn&&g_cam!=CM_GOD)?1.0f:0.0f);
    glUniformMatrix4fv(U(p,"uShadowMVP"),1,GL_FALSE,g_shadowMVP.m);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D,texShadow);
    glUniform1i(U(p,"uShadow"),1);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,tex);
    glUniform1i(U(p,"uTex"),0);
}
static void DrawObj(const Mat4&vp,const Mat4&model,const Mesh&m,GLuint tex,const Vec3&tint){
    glUseProgram(progW);
    SetWorldUniforms(progW,vp,model,M4Id(),tex,tint);
    DrawMesh(m);
}
// draw with rotation normals (model matrix may include rotation): normM = model without translation
static void DrawObjRot(const Mat4&vp,const Mat4&model,const Mat4&rotM,const Mesh&m,GLuint tex,const Vec3&tint){
    glUseProgram(progW);
    SetWorldUniforms(progW,vp,model,rotM,tex,tint);
    DrawMesh(m);
}
static void RenderShadowMap(const Mat4&vp){
    if(!g_flashOn||g_cam==CM_GOD) return;
    Vec3 eye=g_cam==CM_FP?g_camPos:g_pos+Vec3(0,1.6f,0);
    Vec3 dir(-sinf(g_yaw)*cosf(g_pitch),sinf(g_pitch),-cosf(g_yaw)*cosf(g_pitch));
    Mat4 proj=M4Persp(60.0f*PI/180.0f,1.0f,0.2f,30.0f);
    Mat4 view=M4LookAt(eye,eye+dir,Vec3(0,1,0));
    g_shadowMVP=M4Mul(proj,view);
    glBindFramebuffer(GL_FRAMEBUFFER,fboShadow);
    glViewport(0,0,1024,1024);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(progShadow);
    glDisable(GL_CULL_FACE);
    int pcx=(int)floorf(g_pos.x/CHUNKSZ), pcz=(int)floorf(g_pos.z/CHUNKSZ);
    for(int dz=-2;dz<=2;dz++)for(int dx=-2;dx<=2;dx++){
        int key=ChunkKey(pcx+dx,pcz+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()) continue;
        Mat4 mvp=M4Mul(g_shadowMVP,M4Id());
        glUniformMatrix4fv(U(progShadow,"uMVP"),1,GL_FALSE,mvp.m);
        if(it->second.meshWall.vao) DrawMesh(it->second.meshWall);
        if(it->second.meshFC.vao) DrawMesh(it->second.meshFC);
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,g_W,g_H);
}
static void DrawChunks(const Mat4&vp){
    glUseProgram(progW);
    glDisable(GL_CULL_FACE);
    int pcx=(int)floorf(g_pos.x/CHUNKSZ), pcz=(int)floorf(g_pos.z/CHUNKSZ);
    for(int dz=-3;dz<=3;dz++)for(int dx=-3;dx<=3;dx++){
        int key=ChunkKey(pcx+dx,pcz+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()) continue;
        Chunk&ch=it->second;
        if(ch.meshFC.vao){ SetWorldUniforms(progW,vp,M4Id(),M4Id(),g_texFloor,Vec3(1,1,1)); DrawMesh(ch.meshFC); }
        if(ch.meshWall.vao){ SetWorldUniforms(progW,vp,M4Id(),M4Id(),g_texWall,Vec3(1,1,1)); DrawMesh(ch.meshWall); }
        // panels: bright emissive
        if(ch.meshPanel.vao){
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE);
            SetWorldUniforms(progW,vp,M4Id(),M4Id(),g_texPanel,Vec3(2.2f,2.3f,2.5f));
            DrawMesh(ch.meshPanel);
            glDisable(GL_BLEND);
        }
    }
    glEnable(GL_CULL_FACE);
}
static void DrawAKM(const Mat4&vp,const Mat4&model);
static void DrawItems(const Mat4&vp){
    for(size_t i=0;i<g_items.size();i++){
        WorldItem&it=g_items[i];
        if(it.taken) continue;
        it.rot+=g_dt*1.2f;
        float bob=sinf(g_time*2.0f+it.pos.x)*0.05f;
        Vec3 p=it.pos+Vec3(0,bob,0);
        Mat4 model=M4Mul(M4T(p),M4RY(it.rot));
        if(it.type==1){
            Mat4 m1=M4Mul(model,M4S(Vec3(1,1,1)));
            DrawObjRot(vp,m1,model,meshBottle,g_texBottle,Vec3(0.55f,0.75f,0.95f));
            Mat4 cap=M4Mul(M4T(Vec3(0,0.15f,0)),M4S(Vec3(1,1,1)));
            DrawObjRot(vp,M4Mul(model,cap),model,meshCap,g_texBottle,Vec3(0.7f,0.85f,1.0f));
        } else if(it.type==2){
            Mat4 m2=M4Mul(model,M4S(Vec3(1,1,1)));
            DrawObjRot(vp,m2,model,meshMeat,g_texMeatTex,Vec3(1,1,1));
        } else if(it.type==3){
            Mat4 m3=M4Mul(model,M4S(Vec3(1,1,1)));
            DrawObjRot(vp,m3,model,meshFlashB,g_texMetal,Vec3(0.8f,0.8f,0.85f));
            Mat4 h3=M4Mul(M4T(Vec3(0,0.1f,0)),M4S(Vec3(1,1,1)));
            DrawObjRot(vp,M4Mul(model,h3),model,meshFlashH,g_texMetal,Vec3(0.9f,0.9f,0.95f));
        } else if(it.type==4){
            Mat4 m4=M4Mul(model,M4S(Vec3(1,1,1)));
            DrawObjRot(vp,m4,model,meshBook,g_texBook,Vec3(1,1,1));
        } else if(it.type==5){
            DrawAKM(vp,model);
        }
    }
}
static void DrawAKM(const Mat4&vp,const Mat4&model){
    // receiver
    Mat4 rec=M4Mul(model,M4S(Vec3(0.10f,0.13f,0.55f)));
    DrawObjRot(vp,rec,model,meshBox,g_texMetal,Vec3(0.75f,0.75f,0.78f));
    // barrel forward +z
    Mat4 bar=M4Mul(M4T(Vec3(0,0.05f,0.45f)),M4Mul(M4S(Vec3(0.06f,0.06f,0.7f)),M4RZ(PI/2)));
    DrawObjRot(vp,M4Mul(model,bar),model,meshCyl,g_texMetal,Vec3(0.3f,0.3f,0.32f));
    // wood stock rear
    Mat4 stock=M4Mul(M4T(Vec3(0,0.02f,-0.34f)),M4S(Vec3(0.09f,0.14f,0.28f)));
    DrawObjRot(vp,M4Mul(model,stock),model,meshBox,g_texWood,Vec3(1,1,1));
    // grip
    Mat4 grip=M4Mul(M4T(Vec3(0,-0.08f,0.08f)),M4Mul(M4S(Vec3(0.08f,0.14f,0.1f)),M4RX(-0.4f)));
    DrawObjRot(vp,M4Mul(model,grip),model,meshBox,g_texWood,Vec3(1,1,1));
    // mag (banana)
    Mat4 mag=M4Mul(M4T(Vec3(0,-0.12f,0.1f)),M4Mul(M4S(Vec3(0.07f,0.2f,0.09f)),M4RX(0.3f)));
    DrawObjRot(vp,M4Mul(model,mag),model,meshBox,g_texMetal,Vec3(0.5f,0.42f,0.3f));
    // handguard
    Mat4 hg=M4Mul(M4T(Vec3(0,0.0f,0.3f)),M4S(Vec3(0.08f,0.09f,0.22f)));
    DrawObjRot(vp,M4Mul(model,hg),model,meshBox,g_texWood,Vec3(1,1,1));
    // front sight
    Mat4 fs=M4Mul(M4T(Vec3(0,0.09f,0.62f)),M4S(Vec3(0.04f,0.08f,0.04f)));
    DrawObjRot(vp,M4Mul(model,fs),model,meshBox,g_texMetal,Vec3(0.2f,0.2f,0.22f));
}

// ==================== UI helpers ====================
static GLuint ItemIcon(int id){
    switch(id){case 1:return g_iconWater;case 2:return g_iconMeat;case 3:return g_iconFlash;
    case 4:return g_iconBook;case 5:return g_iconAKM;default:return g_texWhite;}
}
static void DrawQuadTex(float x,float y,float w,float h,Vec3 col,float a,GLuint tex){
    glUseProgram(progUI);
    Mat4 m=M4Mul(g_uiMvp,M4Mul(M4T(Vec3(x,y,0)),M4S(Vec3(w,h,1))));
    glUniformMatrix4fv(U(progUI,"uMVP"),1,GL_FALSE,m.m);
    glUniform4f(U(progUI,"uColor"),col.x,col.y,col.z,a);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,tex);
    glUniform1i(U(progUI,"uTex"),0);
    glDisable(GL_CULL_FACE);
    DrawMesh(meshQuad);
}
static void DrawText(float x,float y,const wstring&s,int px,Vec3 col,float a){
    if(s.empty()) return;
    TextTex t=GetText(s,px);
    DrawQuadTex(x,y,(float)t.w,(float)t.h,col,a,t.tex);
}
static void Bar(float x,float y,float w,float h,float frac,Vec3 col){
    DrawQuadTex(x-2,y-2,w+4,h+4,Vec3(0,0,0),0.5f,g_texWhite);
    DrawQuadTex(x,y,w,h,Vec3(0.1f,0.1f,0.12f),0.85f,g_texWhite);
    if(frac>0.01f) DrawQuadTex(x,y,w*ClampF(frac,0,1),h,col,0.92f,g_texWhite);
}
static bool MouseIn(float x,float y,float w,float h){
    POINT mp; GetCursorPos(&mp); ScreenToClient(g_hWnd,&mp);
    return mp.x>=x&&mp.x<=x+w&&mp.y>=y&&mp.y<=y+h;
}

// ==================== humanoid model ====================
// skin, shirt, pants textures + black for hair/shoes
static Vec3 CLR_SKIN=Vec3(1,1,1), CLR_SHIRT=Vec3(1,1,1), CLR_PANTS=Vec3(1,1,1), CLR_DARK=Vec3(1,1,1);
static Mat4 PartM(const Mat4&parent,const Vec3&t,const Mat4&rot,const Vec3&s){
    return M4Mul(parent,M4Mul(M4T(t),M4Mul(rot,M4S(s))));
}
static void Part(const Mat4&vp,const Mat4&m,const Mesh&mesh,GLuint tex,const Vec3&tint){
    Mat4 rot=M4Id(); rot.m[0]=m.m[0]; rot.m[1]=m.m[1]; rot.m[2]=m.m[2];
    rot.m[4]=m.m[4]; rot.m[5]=m.m[5]; rot.m[6]=m.m[6];
    rot.m[8]=m.m[8]; rot.m[9]=m.m[9]; rot.m[10]=m.m[10];
    DrawObjRot(vp,m,rot,mesh,tex,tint);
}
static void DrawHumanoid(const Mat4&vp,const Vec3&root,float yaw,float pitch,bool isLocal,float ph){
    // root at feet; body faces -z by default; rotate by yaw
    Mat4 base=M4Mul(M4T(root),M4RY(yaw));
    float sw=sinf(ph)*0.7f, swA=sinf(ph)*0.55f;
    float bend=pitch*0.5f;
    // torso
    Mat4 pel=PartM(base,Vec3(0,0.95f,0),M4Id(),Vec3(0.34f,0.16f,0.22f));
    Part(vp,pel,meshBox,g_texPants,CLR_PANTS);
    Mat4 chest=PartM(base,Vec3(0,1.28f,0),M4RX(bend*0.3f),Vec3(0.42f,0.5f,0.26f));
    Part(vp,chest,meshBox,g_texShirt,CLR_SHIRT);
    // head
    Mat4 headM=PartM(base,Vec3(0,1.66f,0),M4RX(bend),Vec3(1,1,1));
    DrawObjRot(vp,M4Mul(headM,M4S(Vec3(0.13f,0.16f,0.14f))),headM,meshSphere,g_texSkin,CLR_SKIN);
    Mat4 hair=PartM(base,Vec3(0,1.79f,0),M4RX(bend),Vec3(0.26f,0.10f,0.28f));
    Part(vp,hair,meshBox,g_texPants,CLR_DARK);
    Mat4 eyeL=PartM(base,Vec3(-0.055f,1.68f,0.12f),M4Id(),Vec3(0.03f,0.02f,0.01f));
    Part(vp,eyeL,meshBox,g_texPants,CLR_DARK);
    Mat4 eyeR=PartM(base,Vec3(0.055f,1.68f,0.12f),M4Id(),Vec3(0.03f,0.02f,0.01f));
    Part(vp,eyeR,meshBox,g_texPants,CLR_DARK);
    // legs
    Mat4 hipL=PartM(base,Vec3(-0.11f,1.02f,0),M4RX(sw),Vec3(1,1,1));
    Mat4 thighL=M4Mul(hipL,M4Mul(M4T(Vec3(0,-0.25f,0)),M4S(Vec3(0.17f,0.52f,0.19f))));
    Part(vp,thighL,meshBox,g_texPants,CLR_PANTS);
    Mat4 shinL=M4Mul(hipL,M4Mul(M4T(Vec3(0,-0.69f,0.02f)),M4S(Vec3(0.13f,0.44f,0.14f))));
    Part(vp,shinL,meshBox,g_texPants,CLR_PANTS);
    Mat4 footL=M4Mul(hipL,M4Mul(M4T(Vec3(0,-0.92f,0.09f)),M4S(Vec3(0.11f,0.09f,0.27f))));
    Part(vp,footL,meshBox,g_texPants,CLR_DARK);
    Mat4 hipR=PartM(base,Vec3(0.11f,1.02f,0),M4RX(-sw),Vec3(1,1,1));
    Mat4 thighR=M4Mul(hipR,M4Mul(M4T(Vec3(0,-0.25f,0)),M4S(Vec3(0.17f,0.52f,0.19f))));
    Part(vp,thighR,meshBox,g_texPants,CLR_PANTS);
    Mat4 shinR=M4Mul(hipR,M4Mul(M4T(Vec3(0,-0.69f,0.02f)),M4S(Vec3(0.13f,0.44f,0.14f))));
    Part(vp,shinR,meshBox,g_texPants,CLR_PANTS);
    Mat4 footR=M4Mul(hipR,M4Mul(M4T(Vec3(0,-0.92f,0.09f)),M4S(Vec3(0.11f,0.09f,0.27f))));
    Part(vp,footR,meshBox,g_texPants,CLR_DARK);
    // arms
    Mat4 shL=PartM(base,Vec3(-0.25f,1.45f,0),M4RX(swA*0.6f+bend*0.4f),Vec3(1,1,1));
    Mat4 upL=M4Mul(shL,M4Mul(M4T(Vec3(0,-0.16f,0)),M4S(Vec3(0.09f,0.34f,0.1f))));
    Part(vp,upL,meshBox,g_texShirt,CLR_SHIRT);
    Mat4 foL=M4Mul(shL,M4Mul(M4T(Vec3(0,-0.42f,0)),M4S(Vec3(0.075f,0.28f,0.085f))));
    Part(vp,foL,meshBox,g_texSkin,CLR_SKIN);
    Mat4 handL=M4Mul(shL,M4Mul(M4T(Vec3(0,-0.57f,0)),M4S(Vec3(0.07f,0.09f,0.075f))));
    Part(vp,handL,meshBox,g_texSkin,CLR_SKIN);
    Mat4 shR=PartM(base,Vec3(0.25f,1.45f,0),M4RX(-swA*0.6f+bend*0.4f),Vec3(1,1,1));
    Mat4 upR=M4Mul(shR,M4Mul(M4T(Vec3(0,-0.16f,0)),M4S(Vec3(0.09f,0.34f,0.1f))));
    Part(vp,upR,meshBox,g_texShirt,CLR_SHIRT);
    Mat4 foR=M4Mul(shR,M4Mul(M4T(Vec3(0,-0.42f,0)),M4S(Vec3(0.075f,0.28f,0.085f))));
    Part(vp,foR,meshBox,g_texSkin,CLR_SKIN);
    Mat4 handR=M4Mul(shR,M4Mul(M4T(Vec3(0,-0.57f,0)),M4S(Vec3(0.07f,0.09f,0.075f))));
    Part(vp,handR,meshBox,g_texSkin,CLR_SKIN);
}
struct Vec4{ float x,y,z,w; Vec4(){} Vec4(float a,float b,float c,float d):x(a),y(b),z(c),w(d){} };
static Vec4 Vec4Mul(const Mat4&m,const Vec4&v){
    Vec4 r;
    r.x=m.m[0]*v.x+m.m[4]*v.y+m.m[8]*v.z+m.m[12]*v.w;
    r.y=m.m[1]*v.x+m.m[5]*v.y+m.m[9]*v.z+m.m[13]*v.w;
    r.z=m.m[2]*v.x+m.m[6]*v.y+m.m[10]*v.z+m.m[14]*v.w;
    r.w=m.m[3]*v.x+m.m[7]*v.y+m.m[11]*v.z+m.m[15]*v.w;
    return r;
}
static void DrawText3D(const Mat4&vp,const Vec3&p,const wstring&s){
    if(s.empty()) return;
    TextTex t=GetText(s,16);
    Mat4 m=M4Mul(vp,M4Mul(M4T(p),M4S(Vec3(0.003f,0.003f,0.003f))));
    // billboard-ish: ignore, just draw at anchor with UI ortho using projected pos
    // project to screen
    Vec4 clip = Vec4Mul(m,Vec4(0,0,0,1));
    if(clip.w<=0.01f) return;
    float sx=(clip.x/clip.w*0.5f+0.5f)*g_W, sy=(1.0f-(clip.y/clip.w*0.5f+0.5f))*g_H;
    DrawQuadTex(sx-t.w/2,sy-18,(float)t.w,(float)t.h,Vec3(1,1,1),0.95f,t.tex);
}
static void DrawRemotePlayers(const Mat4&vp){
    for(auto&kv:g_peers){
        RemotePlayer&rp=kv.second;
        if(g_time-rp.last>5.0f) continue;
        float ph=g_time*3.0f;
        DrawHumanoid(vp,rp.pos,rp.yaw,rp.pitch,false,sinf(ph)*0.0f);
        // name label
        wstring nm(rp.name,rp.name+strlen(rp.name));
        DrawText3D(vp,rp.pos+Vec3(0,2.05f,0),nm);
    }
}


// ==================== first person arms & legs ====================
static void DrawHandMesh(const Mat4&vp,const Mat4&m){
    Mat4 rot=M4Id(); rot.m[0]=m.m[0]; rot.m[1]=m.m[1]; rot.m[2]=m.m[2];
    rot.m[4]=m.m[4]; rot.m[5]=m.m[5]; rot.m[6]=m.m[6];
    rot.m[8]=m.m[8]; rot.m[9]=m.m[9]; rot.m[10]=m.m[10];
    DrawObjRot(vp,m,rot,meshBox,g_texSkin,CLR_SKIN);
}
static void DrawFingers(const Mat4&vp,const Mat4&handM,int dir){
    for(int i=0;i<5;i++){
        float fx=dir*0.045f*(i-2);
        Mat4 f=PartM(handM,Vec3(fx,-0.03f,0.055f),M4RX(0.5f),Vec3(0.02f,0.07f,0.02f));
        DrawHandMesh(vp,f);
    }
}
static void DrawHandsFP(const Mat4&vp){
    Mat4 camBase=M4Mul(M4T(g_camPos),M4Mul(M4RY(g_yaw),M4RX(-g_pitch)));
    Vec3 bob(0,sinf(g_walkT*2.2f)*0.015f,0);
    camBase=M4Mul(M4T(bob),camBase);
    bool holdingAKM=false; (void)holdingAKM;
    float recoil=g_recoil*0.08f;
    float eatR=(g_eatAnim>0.5f||g_drinkAnim>0.5f)?1.0f:0.0f;
    // right arm
    Mat4 shR=PartM(camBase,Vec3(0.30f,-0.32f,-0.5f),M4RX(-0.4f+eatR*1.3f+recoil),Vec3(1,1,1));
    Mat4 upR=M4Mul(shR,M4Mul(M4T(Vec3(0,-0.10f,0.12f)),M4S(Vec3(0.07f,0.26f,0.08f))));
    Part(vp,upR,meshBox,g_texShirt,CLR_SHIRT);
    Mat4 foR=M4Mul(shR,M4Mul(M4T(Vec3(0,-0.26f,0.24f)),M4S(Vec3(0.06f,0.22f,0.07f))));
    Part(vp,foR,meshBox,g_texSkin,CLR_SKIN);
    Mat4 handR=PartM(shR,Vec3(0,-0.36f,0.34f),M4Id(),Vec3(0.055f,0.07f,0.06f));
    DrawHandMesh(vp,handR);
    DrawFingers(vp,handR,1);
    // held item in right hand (by hotbar slot)
    Mat4 hold=M4Mul(handR,M4T(Vec3(0,0,0.09f)));
    int heldId=(g_hotbarSel<(int)g_inv.size())?g_inv[g_hotbarSel].id:0;
    if(heldId==5){
        DrawAKM(vp,hold);
    } else if(heldId==3){
        Mat4 fl=M4Mul(hold,M4S(Vec3(1,1,1)));
        DrawObjRot(vp,M4Mul(fl,M4S(Vec3(1.2f,1.2f,1.2f))),hold,meshFlashB,g_texMetal,Vec3(0.8f,0.8f,0.85f));
        DrawObjRot(vp,M4Mul(M4Mul(fl,M4T(Vec3(0,0.05f,0))),M4S(Vec3(1.4f,1.4f,1.4f))),hold,meshFlashH,g_texMetal,Vec3(0.9f,0.9f,0.95f));
    } else if(heldId==1){
        DrawObjRot(vp,M4Mul(hold,M4S(Vec3(1,1,1))),hold,meshBottle,g_texBottle,Vec3(0.6f,0.75f,0.95f));
        DrawObjRot(vp,M4Mul(M4Mul(hold,M4T(Vec3(0,0.15f,0))),M4S(Vec3(1,1,1))),hold,meshCap,g_texBottle,Vec3(0.7f,0.85f,1.0f));
    } else if(heldId==2){
        DrawObjRot(vp,M4Mul(hold,M4S(Vec3(1.4f,1.4f,1.4f))),hold,meshMeat,g_texMeatTex,Vec3(1,1,1));
    } else if(heldId==4){
        DrawObjRot(vp,M4Mul(hold,M4S(Vec3(1.2f,1.2f,1.2f))),hold,meshBook,g_texBook,Vec3(1,1,1));
    }
    // left arm
    Mat4 shL=PartM(camBase,Vec3(-0.30f,-0.32f,-0.5f),M4RX(-0.4f+eatR*1.0f),Vec3(1,1,1));
    Mat4 upL=M4Mul(shL,M4Mul(M4T(Vec3(0,-0.10f,0.10f)),M4S(Vec3(0.07f,0.26f,0.08f))));
    Part(vp,upL,meshBox,g_texShirt,CLR_SHIRT);
    Mat4 foL=M4Mul(shL,M4Mul(M4T(Vec3(0,-0.26f,0.20f)),M4S(Vec3(0.06f,0.22f,0.07f))));
    Part(vp,foL,meshBox,g_texSkin,CLR_SKIN);
    Mat4 handL=PartM(shL,Vec3(0,-0.36f,0.30f),M4Id(),Vec3(0.055f,0.07f,0.06f));
    DrawHandMesh(vp,handL);
    DrawFingers(vp,handL,-1);
    // legs & feet when looking down
    if(g_pitch<-0.35f){
        Mat4 legBase=M4Mul(M4T(g_pos),M4RY(g_yaw));
        float sw=sinf(g_walkT*1.6f)*0.5f;
        Mat4 hipL=PartM(legBase,Vec3(-0.11f,1.02f,0),M4RX(sw),Vec3(1,1,1));
        Mat4 thighL=M4Mul(hipL,M4Mul(M4T(Vec3(0,-0.25f,0)),M4S(Vec3(0.17f,0.52f,0.19f))));
        Part(vp,thighL,meshBox,g_texPants,CLR_PANTS);
        Mat4 shinL=M4Mul(hipL,M4Mul(M4T(Vec3(0,-0.69f,0.02f)),M4S(Vec3(0.13f,0.44f,0.14f))));
        Part(vp,shinL,meshBox,g_texPants,CLR_PANTS);
        Mat4 footL=M4Mul(hipL,M4Mul(M4T(Vec3(0,-0.92f,0.09f)),M4S(Vec3(0.11f,0.09f,0.27f))));
        Part(vp,footL,meshBox,g_texPants,CLR_DARK);
        Mat4 hipR=PartM(legBase,Vec3(0.11f,1.02f,0),M4RX(-sw),Vec3(1,1,1));
        Mat4 thighR=M4Mul(hipR,M4Mul(M4T(Vec3(0,-0.25f,0)),M4S(Vec3(0.17f,0.52f,0.19f))));
        Part(vp,thighR,meshBox,g_texPants,CLR_PANTS);
        Mat4 shinR=M4Mul(hipR,M4Mul(M4T(Vec3(0,-0.69f,0.02f)),M4S(Vec3(0.13f,0.44f,0.14f))));
        Part(vp,shinR,meshBox,g_texPants,CLR_PANTS);
        Mat4 footR=M4Mul(hipR,M4Mul(M4T(Vec3(0,-0.92f,0.09f)),M4S(Vec3(0.11f,0.09f,0.27f))));
        Part(vp,footR,meshBox,g_texPants,CLR_DARK);
    }
}

// ==================== HUD / menu ====================
static void DrawHUD(){
    glDisable(GL_DEPTH_TEST);
    g_uiMvp=M4Ortho(0,(float)g_W,(float)g_H,0,-1,1);
    if(g_gamestate==GAME_MENU){
        DrawQuadTex(0,0,(float)g_W,(float)g_H,Vec3(0.02f,0.015f,0.02f),0.96f,g_texWhite);
        DrawText((float)g_W/2-230,120,L"后 世  Afterlife",52,Vec3(0.9f,0.83f,0.55f),1.0f);
        DrawText((float)g_W/2-180,185,L"—— Backrooms 无限迷宫 · 生存 ——",22,Vec3(0.7f,0.7f,0.72f),1.0f);
        float bw=320,bh=52,bx=(float)g_W/2-bw/2,by=300;
        const wchar_t* items[3]={L"开始新游戏",L"继续游戏",L"退出游戏"};
        for(int i=0;i<3;i++){
            float y=by+i*76;
            bool hv=MouseIn(bx,y,bw,bh);
            DrawQuadTex(bx,y,bw,bh,hv?Vec3(0.30f,0.26f,0.16f):Vec3(0.14f,0.12f,0.10f),0.95f,g_texWhite);
            if(g_menuSel==i) DrawQuadTex(bx,y,bw,bh,Vec3(0.9f,0.7f,0.2f),0.18f,g_texWhite);
            DrawText(bx+bw/2-60,y+10,items[i],30,hv?Vec3(1,0.95f,0.7f):Vec3(0.9f,0.9f,0.9f),1.0f);
            if(hv&&KeyDn(VK_LBUTTON)) g_menuSel=i;
        }
        DrawText((float)g_W/2-180,560,L"角色名: "+g_nameBuf+((g_time*2.0f-(int)(g_time*2.0f))<0.5f?L"|":L" "),24,Vec3(0.8f,0.8f,0.85f),1.0f);
        DrawText((float)g_W/2-180,605,L"↑↓ 选择  回车 确认  直接输入角色名",18,Vec3(0.5f,0.5f,0.55f),1.0f);
        DrawText((float)g_W/2-180,80,L"联机: 我的码 "+wstring(g_pubIP.begin(),g_pubIP.end())+L":"+to_wstring(g_pubPort)+L"  (O键广播开关)",18,Vec3(0.6f,0.7f,0.6f),1.0f);
        return;
    }
    if(g_gamestate==GAME_PAUSE){
        DrawQuadTex(0,0,(float)g_W,(float)g_H,Vec3(0,0,0),0.5f,g_texWhite);
        DrawText((float)g_W/2-140,160,L"暂停",40,Vec3(0.95f,0.9f,0.6f),1.0f);
        float bw=300,bh=48,bx=(float)g_W/2-bw/2,by=260;
        const wchar_t* items[3]={L"继续游戏",L"保存并返回主菜单",L"退出游戏"};
        for(int i=0;i<3;i++){
            float y=by+i*64;
            bool hv=MouseIn(bx,y,bw,bh);
            DrawQuadTex(bx,y,bw,bh,hv?Vec3(0.30f,0.26f,0.16f):Vec3(0.14f,0.12f,0.10f),0.95f,g_texWhite);
            if(g_pauseSel==i) DrawQuadTex(bx,y,bw,bh,Vec3(0.9f,0.7f,0.2f),0.18f,g_texWhite);
            DrawText(bx+bw/2-80,y+8,items[i],24,hv?Vec3(1,0.95f,0.7f):Vec3(0.9f,0.9f,0.9f),1.0f);
            if(hv&&KeyDn(VK_LBUTTON)) g_pauseSel=i;
        }
        DrawText((float)g_W/2-140,500,L"↑↓ 选择  回车 确认  ESC 继续",18,Vec3(0.6f,0.6f,0.65f),1.0f);
        return;
    }
    if(g_gamestate==GAME_DEAD){
        DrawQuadTex(0,0,(float)g_W,(float)g_H,Vec3(0.1f,0.02f,0.02f),0.7f,g_texWhite);
        DrawText((float)g_W/2-150,250,L"你死了……",44,Vec3(0.95f,0.3f,0.25f),1.0f);
        DrawText((float)g_W/2-180,330,L"按 回车 在迷宫中重生",24,Vec3(0.8f,0.8f,0.8f),1.0f);
        return;
    }
    // ---- in game HUD ----
    struct SR{ wstring n; float v; Vec3 c; };
    SR rows[5]={{L"生命",g_hp,Vec3(0.88f,0.22f,0.22f)},{L"理智",g_san,Vec3(0.68f,0.4f,0.95f)},
                {L"饥饿",g_hun,Vec3(0.95f,0.7f,0.25f)},{L"口渴",g_thr,Vec3(0.3f,0.6f,0.95f)},
                {L"体力",g_sta,Vec3(0.35f,0.85f,0.4f)}};
    for(int i=0;i<5;i++){
        float y=20+i*28;
        DrawText(18,y,rows[i].n,24,Vec3(0.95f,0.95f,0.95f),1.0f);
        Bar(96,y+5,180,15,rows[i].v/100.0f,rows[i].c);
        DrawText(286,y+2,to_wstring((int)rows[i].v),20,Vec3(1,1,1),0.95f);
    }
    Bar(18,168,266,10,g_xp/100.0f,Vec3(0.95f,0.85f,0.4f));
    DrawText(18,182,L"等级 "+to_wstring(g_level)+L"  经验 "+to_wstring((int)g_xp)+L"/100",18,Vec3(0.95f,0.9f,0.7f),1.0f);
    DrawText(18,206,L"背包容量 "+to_wstring(g_capacity)+L" 格",16,Vec3(0.75f,0.75f,0.8f),1.0f);
    DrawText((float)g_W-110,12,L"FPS "+to_wstring((int)g_fps),20,Vec3(0.5f,0.95f,0.5f),1.0f);
    // ammo
    if(g_hotbarSel<(int)g_inv.size()&&g_inv[g_hotbarSel].id==5){
        DrawText((float)g_W-210,(float)g_H-70,g_reloadT>0?L"换弹中...":L"弹药 "+to_wstring(g_ammo)+L"/30",24,Vec3(1,0.9f,0.5f),1.0f);
    }
    DrawText(18,(float)g_H-36,g_flashOn?L"手电筒 已开启 (F)":L"手电筒 已关闭 (F)",18,Vec3(1,0.95f,0.7f),1.0f);
    DrawText(18,(float)g_H-58,L"WASD移动 空格跳 Shift疾跑 E背包 数字键换物 F手电 F5视角 T聊天 ESC菜单",16,Vec3(0.6f,0.6f,0.68f),0.9f);
    // crosshair
    if(!g_invOpen&&!g_chatOpen){
        float cx=(float)g_W/2, cy=(float)g_H/2;
        DrawQuadTex(cx-8,cy-1,16,2,Vec3(1,1,1),0.9f,g_texWhite);
        DrawQuadTex(cx-1,cy-8,2,16,Vec3(1,1,1),0.9f,g_texWhite);
    }
    // hotbar
    int hs=9; float hw=56, hx=(float)g_W/2-hs*hw/2, hy=(float)g_H-hw-12;
    for(int i=0;i<hs;i++){
        DrawQuadTex(hx+i*hw,hy,hw,hw,Vec3(0.1f,0.1f,0.12f),0.82f,g_texWhite);
        int id=(i<(int)g_inv.size())?g_inv[i].id:0;
        if(id>0) DrawQuadTex(hx+i*hw+6,hy+6,hw-12,hw-12,Vec3(1,1,1),1.0f,ItemIcon(id));
        if(i==g_hotbarSel) DrawQuadTex(hx+i*hw,hy,hw,hw,Vec3(0.9f,0.7f,0.2f),0.28f,g_texWhite);
        if(id>0&&g_inv[i].cnt>1) DrawText(hx+i*hw+hw-30,hy+hw-30,to_wstring(g_inv[i].cnt),18,Vec3(1,1,1),1.0f);
    }
    // messages
    float my=240;
    for(size_t i=0;i<g_msgs.size();i++){
        DrawText(18,my,g_msgs[i],18,Vec3(0.95f,0.95f,0.9f),1.0f);
        my+=24;
    }
    for(size_t i=0;i<g_views.size();i++){
        DrawText(18,my,g_views[i].s,20,Vec3(1.0f,0.9f,0.5f),1.0f);
        my+=26;
    }
    // chat log
    int clh=(int)g_chatLog.size(); int start=clh>7?clh-7:0;
    float chy=(float)g_H-90;
    for(int i=start;i<clh;i++){
        DrawText(18,chy,g_chatLog[i],17,Vec3(0.85f,0.9f,0.85f),0.95f);
        chy+=21;
    }
    if(g_chatOpen){
        DrawQuadTex(10,(float)g_H-30,(float)g_W-20,26,Vec3(0.1f,0.12f,0.1f),0.9f,g_texWhite);
        DrawText(16,(float)g_H-27,L"> "+g_chatBuf+(((g_time*2.0f-(int)(g_time*2.0f))<0.5f)?L"|":L" "),19,Vec3(1,1,1),1.0f);
    }
    // vignette low sanity
    if(g_san<35.0f){ float a=(35.0f-g_san)/35.0f*0.45f;
        DrawQuadTex(0,0,(float)g_W,(float)g_H,Vec3(0.02f,0.02f,0.12f),a,g_texWhite); }
    if(g_hp<25.0f){ float a=(25.0f-g_hp)/25.0f*0.4f;
        DrawQuadTex(0,0,(float)g_W,(float)g_H,Vec3(0.5f,0.02f,0.02f),a,g_texWhite); }
    // inventory
    if(g_invOpen){
        int cols=8; float cell=56, gap=6, m=18;
        int rowsN=(g_capacity+cols-1)/cols;
        float pW=cols*cell+(cols-1)*gap+2*m, pH=rowsN*cell+(rowsN-1)*gap+2*m;
        float x0=(g_W-pW)/2, y0=g_H*0.10f;
        DrawQuadTex(x0,y0,pW,pH,Vec3(0.05f,0.05f,0.08f),0.94f,g_texWhite);
        DrawText(x0+16,y0-28,L"背包 (E 关闭)   左键拿起/放下 · 右键使用 · Q 选中再按Q丢弃",20,Vec3(1,1,1),1.0f);
        POINT mp; GetCursorPos(&mp); ScreenToClient(g_hWnd,&mp);
        g_hoverSlot=-1;
        for(int i=0;i<(int)g_inv.size();i++){
            int c=i%cols, r=i/cols;
            float sx=x0+m+c*(cell+gap), sy=y0+m+r*(cell+gap);
            DrawQuadTex(sx,sy,cell,cell,Vec3(0.15f,0.15f,0.19f),1.0f,g_texWhite);
            if(mp.x>=sx&&mp.x<=sx+cell&&mp.y>=sy&&mp.y<=sy+cell) g_hoverSlot=i;
            if(i==g_selSlot) DrawQuadTex(sx+2,sy+2,cell-4,cell-4,Vec3(0.9f,0.8f,0.2f),0.25f,g_texWhite);
            if(g_inv[i].id>0){
                DrawQuadTex(sx+6,sy+6,cell-12,cell-12,Vec3(1,1,1),1.0f,ItemIcon(g_inv[i].id));
                if(g_inv[i].cnt>1) DrawText(sx+cell-28,sy+cell-26,to_wstring(g_inv[i].cnt),18,Vec3(1,1,1),1.0f);
            }
        }
        if(g_hoverSlot>=0&&g_inv[g_hoverSlot].id>0&&g_dragId==0)
            DrawText(x0+16,y0+pH+6,ItemName(g_inv[g_hoverSlot].id),20,Vec3(1,0.95f,0.7f),1.0f);
        if(g_dragId>0){
            POINT mp2; GetCursorPos(&mp2); ScreenToClient(g_hWnd,&mp2);
            DrawQuadTex((float)mp2.x-24,(float)mp2.y-24,48,48,Vec3(1,1,1),0.95f,ItemIcon(g_dragId));
        }
    }
    // tracer
    if(g_tracerT>0){
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glUseProgram(progW);
        SetWorldUniforms(progW,g_view,M4Id(),M4Id(),g_texWhite,Vec3(1,1,1));
        glUniform3f(U(progW,"uTint"),1.2f,0.8f,0.3f);
        glUniform1i(U(progW,"uLNum"),0);
        glUniform1f(U(progW,"uFlashOn"),0.0f);
        glUniform1f(U(progW,"uShadowOn"),0.0f);
        // simple line as thin quad along direction
        Vec3 d=g_tracerTo-g_tracerFrom; float len=d.len();
        if(len>0.1f){
            Vec3 n=d.norm();
            Vec3 up(0,1,0); Vec3 right=n.cross(up).norm(); Vec3 up2=right.cross(n).norm();
            float th=0.012f;
            GeoBuild g;
            GeoQuad(g,g_tracerFrom-right*th,g_tracerTo-right*th,g_tracerTo+right*th,g_tracerFrom+right*th,up2,0,0,1,1);
            // build temp mesh
            Mesh mm=M0(); BuildMesh(mm,g.v,g.idx,8);
            DrawMesh(mm);
            glDeleteVertexArrays(1,&mm.vao); glDeleteBuffers(2,&mm.vbo);
        }
        glDisable(GL_BLEND);
    }
}

// ==================== frame ====================
static void RenderFrame(){
    Mat4 proj=M4Persp(70.0f*PI/180.0f,(float)g_W/(float)g_H,0.1f,400.0f);
    Vec3 camPos,fwd;
    if(g_cam==CM_FP){
        camPos=g_pos+Vec3(0,1.55f+sinf(g_walkT*2.2f)*0.012f,0);
        fwd=Vec3(-sinf(g_yaw)*cosf(g_pitch),sinf(g_pitch),-cosf(g_yaw)*cosf(g_pitch));
    } else if(g_cam==CM_TP){
        Vec3 f(-sinf(g_yaw),0,-cosf(g_yaw));
        camPos=g_pos+Vec3(0,1.8f,0)-f*4.5f+Vec3(0,0.5f,0);
        fwd=(g_pos+Vec3(0,1.4f,0)-camPos).norm();
    } else {
        camPos=g_pos+Vec3(0,20.0f,0);
        fwd=(g_pos-camPos).norm();
    }
    g_camPos=camPos; g_camFwd=fwd;
    g_view=M4LookAt(camPos,camPos+fwd,Vec3(0,1,0)); g_proj=proj;
    Mat4 vp=M4Mul(proj,g_view);
    GatherLights();
    RenderShadowMap(vp);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,g_W,g_H);
    glClearColor(0.03f,0.03f,0.04f,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    DrawChunks(vp);
    DrawItems(vp);
    DrawRemotePlayers(vp);
    if(g_cam==CM_FP) DrawHandsFP(vp); else DrawHumanoid(vp,g_pos,g_yaw,g_pitch,true,sinf(g_walkT*1.6f)*0.0f);
    DrawHUD();
    SwapBuffers(g_hDC);
}
static void DrawLoadingFrame(){
    glViewport(0,0,g_W,g_H);
    glClearColor(0.02f,0.02f,0.03f,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    g_uiMvp=M4Ortho(0,(float)g_W,(float)g_H,0,-1,1);
    DrawQuadTex(0,0,(float)g_W,(float)g_H,Vec3(0.02f,0.02f,0.03f),1.0f,g_texWhite);
    DrawText((float)g_W/2-230,g_H/2-110,L"后 世  Afterlife",52,Vec3(0.9f,0.85f,0.6f),1.0f);
    DrawText((float)g_W/2-150,g_H/2-10,g_loadText,24,Vec3(0.8f,0.8f,0.8f),1.0f);
    Bar((float)g_W/2-150,g_H/2+26,300,18,0.5f,Vec3(0.4f,0.8f,0.4f));
    SwapBuffers(g_hDC);
}

// ==================== autotest (dev verification harness) ====================
struct AutoCmd{ int type; char txt[128]; float t; };
static vector<AutoCmd> g_auto; static size_t g_autoIdx=0; static float g_autoT=0;
static void LoadAutoTest(){
    FILE* f=fopen("autotest.cfg","r");
    if(!f) return;
    char line[256];
    while(fgets(line,sizeof(line),f)){
        AutoCmd c; memset(&c,0,sizeof(c)); c.t=0;
        char cmd[64]; char arg[128]={0};
        if(sscanf(line,"%63s %127[^\n]",cmd,arg)>=1){
            if(strcmp(cmd,"hold")==0||strcmp(cmd,"press")==0){
                c.type=(cmd[0]=='h')?1:2;
                char key[32]={0}; float dur=0.12f;
                sscanf(arg,"%31s %f",key,&dur);
                strncpy(c.txt,key,127); c.t=dur<0.001f?0.12f:dur;
            }
            else if(strcmp(cmd,"wait")==0){ c.type=3; c.t=(float)atof(arg); }
            else if(strcmp(cmd,"type")==0){ c.type=4; strncpy(c.txt,arg,127); }
            else if(strcmp(cmd,"log")==0){ c.type=5; strncpy(c.txt,arg,127); }
            else if(strcmp(cmd,"check")==0){ c.type=6; }
            else if(strcmp(cmd,"shoot")==0){ c.type=7; }
            else if(strcmp(cmd,"give")==0){ c.type=8; strncpy(c.txt,arg,127); }
            g_auto.push_back(c);
        }
    }
    fclose(f);
    if(g_auto.empty()) return;
    // activate first hold/press
    g_autoIdx=0; g_autoT=0;
}
static void AutoLog(const char* s){
    FILE* f=fopen("test.log","a");
    if(f){ fprintf(f,"%s\n",s); fclose(f); }
}
static int KeyToVK(const char* s){
    if(!s) return 0;
    if(s[0]>='a'&&s[0]<='z'&&s[1]==0) return s[0]-32;
    if(strcmp(s,"space")==0) return VK_SPACE;
    if(strcmp(s,"shift")==0) return VK_SHIFT;
    if(strcmp(s,"enter")==0) return VK_RETURN;
    if(strcmp(s,"esc")==0) return VK_ESCAPE;
    if(strcmp(s,"f5")==0) return VK_F5;
    if(strcmp(s,"f")==0) return 'F';
    if(strcmp(s,"e")==0) return 'E';
    if(strcmp(s,"t")==0) return 'T';
    if(strcmp(s,"r")==0) return 'R';
    if(strcmp(s,"q")==0) return 'Q';
    if(s[0]>='1'&&s[0]<='9') return s[0];
    return 0;
}
static void UpdateAutoTest(float dt){
    if(g_autoIdx>=g_auto.size()) return;
    AutoCmd&c=g_auto[g_autoIdx];
    if(c.type==1||c.type==2){ // hold / press
        int vk=KeyToVK(c.txt);
        if(vk) simKey[vk]=1;
        c.t-=dt;
        if(c.t<=0){ if(vk) simKey[vk]=0; c.type=-1; }
    } else if(c.type==3){ // wait
        c.t-=dt;
        if(c.t<=0) c.type=-1;
    } else if(c.type==4){ // type chars
        for(char* p=c.txt;*p;p++){
            if(g_chatOpen&&g_chatBuf.size()<128) g_chatBuf.push_back((wchar_t)*p);
            else if(g_gamestate==GAME_MENU&&g_nameBuf.size()<24) g_nameBuf.push_back((wchar_t)*p);
        }
        c.type=-1;
    } else if(c.type==5){ AutoLog(c.txt); c.type=-1; }
    else if(c.type==6){ // check
        char buf[256];
        sprintf(buf,"CHECK pos=%.1f,%.1f,%.1f hp=%.0f san=%.0f hun=%.0f thr=%.0f invOpen=%d cam=%d ammo=%d flash=%d",
            g_pos.x,g_pos.y,g_pos.z,g_hp,g_san,g_hun,g_thr,g_invOpen?1:0,g_cam,g_ammo,g_flashOn?1:0);
        AutoLog(buf);
        c.type=-1;
    } else if(c.type==7){ if(g_gamestate==GAME_PLAY) Shoot(); c.type=-1; }
    else if(c.type==8){ if(strstr(c.txt,"akm")) GiveAKM(); c.type=-1; }
    if(c.type==-1){ g_autoIdx++; g_autoT=0; }
}

// ==================== window ====================
static LRESULT CALLBACK WndProc(HWND h,UINT m,WPARAM w,LPARAM l){
    switch(m){
        case WM_CHAR:{
            if(g_chatOpen&&g_chatBuf.size()<128){
                if(w==8){ if(!g_chatBuf.empty()) g_chatBuf.pop_back(); }
                else if(w>=32) g_chatBuf.push_back((wchar_t)w);
            } else if(g_gamestate==GAME_MENU&&g_nameBuf.size()<24){
                if(w==8){ if(!g_nameBuf.empty()) g_nameBuf.pop_back(); }
                else if(w>=32) g_nameBuf.push_back((wchar_t)w);
            }
            return 0; }
        case WM_CLOSE: SaveGame(); DestroyWindow(h); return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
        case WM_ERASEBKGND: return 1;
    }
    return DefWindowProcW(h,m,w,l);
}
static bool CreateGLWindow(){
    WNDCLASSW wc; memset(&wc,0,sizeof(wc));
    wc.style=CS_OWNDC; wc.lpfnWndProc=WndProc; wc.hInstance=g_hInst;
    wc.hIcon=LoadIconW(g_hInst,MAKEINTRESOURCEW(101));
    if(!wc.hIcon) wc.hIcon=LoadIconW(NULL,(LPCWSTR)IDI_APPLICATION);
    wc.hCursor=LoadCursorW(NULL,(LPCWSTR)IDC_ARROW);
    wc.lpszClassName=L"HoushiWin";
    if(!RegisterClassW(&wc)) return false;
    RECT cr={0,0,1280,720}; AdjustWindowRect(&cr,WS_OVERLAPPEDWINDOW,FALSE);
    g_hWnd=CreateWindowW(L"HoushiWin",L"后世 Afterlife",WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT,cr.right-cr.left,cr.bottom-cr.top,NULL,NULL,g_hInst,NULL);
    if(!g_hWnd) return false;
    ShowWindow(g_hWnd,SW_SHOW);
    g_hDC=GetDC(g_hWnd);
    PIXELFORMATDESCRIPTOR pfd={sizeof(pfd),1,PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,32,0,0,0,0,0,0,0,0,0,0,0,0,0,24,8,0,PFD_MAIN_PLANE,0,0,0,0};
    int pf=ChoosePixelFormat(g_hDC,&pfd); SetPixelFormat(g_hDC,pf,&pfd);
    HGLRC tmp=wglCreateContext(g_hDC); wglMakeCurrent(g_hDC,tmp);
    PFN_wglCreateContextAttribsARB createAttribs=
        (PFN_wglCreateContextAttribsARB)wglGetProcAddress("wglCreateContextAttribsARB");
    HGLRC rc=0;
    if(createAttribs){
        const int attrs[]={0x2091,3,0x2092,3,0x9126,0x00000001,0};
        rc=createAttribs(g_hDC,0,attrs);
    }
    if(rc){ wglMakeCurrent(g_hDC,rc); wglDeleteContext(tmp); } else rc=tmp;
    g_hRC=rc;
    if(!LoadGL()){
        FILE* lf=fopen("startup.log","a");
        if(lf){ const char* ver=(const char*)glGetString(GL_VERSION);
            fprintf(lf,"LoadGL failed. GL_VERSION=%s\n",ver?ver:"?"); fclose(lf); }
        return false;
    }
    if(wglSwapIntervalEXT) wglSwapIntervalEXT(0);
    { FILE* lf=fopen("startup.log","w");
        if(lf){ const char* ver=(const char*)glGetString(GL_VERSION);
            const char* ren=(const char*)glGetString(GL_RENDERER);
            fprintf(lf,"GL_VERSION=%s\nGL_RENDERER=%s\nCORE33=%s\n",ver?ver:"?",ren?ren:"?",rc!=tmp?"yes":"no(legacy)");
            fclose(lf); } }
    return true;
}

// ==================== main ====================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int){
    g_hInst=hInst;
    GetExeDir();
    AddFontResourceW((g_exeDir+L"\\font.ttc").c_str());
    NetInit();
    srand(GetTickCount());
    if(!CreateGLWindow()){ MessageBoxA(NULL,"无法创建 OpenGL 窗口(需 OpenGL 3.3)","后世",MB_OK); return 1; }
    RECT rc; GetClientRect(g_hWnd,&rc); g_W=rc.right; g_H=rc.bottom;
    glEnable(GL_DEPTH_TEST); glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glClearColor(0.03f,0.03f,0.04f,1);
    CompilePrograms();
    { Canvas c(8,8); CanvasFillRect(c,0,0,8,8,255,255,255); g_texWhite=c.Upload(); }
    MakeBaseMeshes();
    MakeAllTextures();
    MakeShadowFBO();
    GenSounds();
    g_inv.assign(40,InvItem{0,0});
    g_inv[0].id=3; g_inv[0].cnt=1;
    g_inv[1].id=4; g_inv[1].cnt=1;
    g_inv[2].id=1; g_inv[2].cnt=2;
    g_inv[3].id=2; g_inv[3].cnt=2;
    g_pos=SpawnPos();
    UpdateChunks();
    g_gamestate=GAME_MENU;
    LoadAutoTest();
    MSG msg; memset(&msg,0,sizeof(msg));
    LARGE_INTEGER freq,last; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&last);
    for(;;){
        while(PeekMessageW(&msg,NULL,0,0,PM_REMOVE)){
            if(msg.message==WM_QUIT) goto done;
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
        LARGE_INTEGER now; QueryPerformanceCounter(&now);
        double dtf=(double)(now.QuadPart-last.QuadPart)/(double)freq.QuadPart;
        last=now;
        if(dtf>0.1) dtf=0.1;
        float dt=(float)dtf;
        g_fps=g_fps*0.95f+((float)(1.0/(dtf>0.0001?dtf:0.0001)))*0.05f;
        UpdateAutoTest(dt);
        if(g_gamestate==GAME_PLAY) UpdateGame(dt);
        else UpdateMenu(dt);
        if(g_gamestate==GAME_PLAY) UpdateChunks();
        NetUpdate(dt);
        RenderFrame();
    }
done:
    SaveGame();
    return 0;
}
