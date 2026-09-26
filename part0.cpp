// ==================== globals & types (part0) ====================
#ifdef __EMSCRIPTEN__
enum { VK_ESCAPE=0x1B, VK_RETURN=0x0D, VK_SPACE=0x20, VK_BACK=0x08, VK_TAB=0x09, VK_SHIFT=0x10, VK_CONTROL=0x11,
       VK_UP=0x26, VK_DOWN=0x28, VK_LEFT=0x25, VK_RIGHT=0x27, VK_F1=0x70, VK_F2=0x71, VK_F3=0x72, VK_F4=0x73,
       VK_F5=0x74, VK_F6=0x75, VK_F7=0x76, VK_F8=0x77, VK_F9=0x78, VK_F10=0x79, VK_F11=0x7A, VK_F12=0x7B,
       VK_LBUTTON=0x01, VK_RBUTTON=0x02 };
// web mouse state (fed by the SDL event pump in web.cpp)
int g_mouseX=0,g_mouseY=0,g_mouseDX=0,g_mouseDY=0;
int g_mouseDown=0,g_mousePressed=0; bool g_pointerLock=false;
#define PostQuitMessage(x) do{ g_running=false; }while(0)
#define ShowCursor(x) ((void)0)
#define GetExeDir() GetExeDirWeb()
static void GetExeDirWeb(){ g_exeDir=L""; }
#else
#define GetExeDir() GetExeDirWin()
#endif
enum GameState{ GAME_MENU, GAME_GENERATING, GAME_PLAY, GAME_PAUSE, GAME_DEAD };
enum CamMode{ CM_FP=0, CM_TP, CM_GOD };
struct InvItem{ int id; int cnt; };
struct AABB{ Vec3 mn,mx; };
struct WorldItem{ int type; Vec3 pos; float rot; bool taken; };
struct MsgT{ wstring s; float t; };
static wstring g_exeDir;
#ifndef __EMSCRIPTEN__
static HINSTANCE g_hInst=0;
static HWND g_hWnd=0; static HDC g_hDC=0; static HGLRC g_hRC=0;
static void GetExeDirWin(){ wchar_t buf[1024]; GetModuleFileNameW(NULL,buf,1023);
    wchar_t* sl=wcsrchr(buf,L'\\'); if(sl)*sl=0; g_exeDir=buf; }
#else
bool g_running=true;
#endif
static int g_W=1280,g_H=720;
static int g_gamestate=GAME_MENU;
static int g_cam=CM_FP;
static Vec3 g_pos(0,1.7f,0); static float g_yaw=0,g_pitch=0,g_velY=0; static bool g_onGround=true;
static float g_hp=100,g_san=100,g_hun=100,g_thr=100,g_sta=100;
static float g_xp=0; static int g_level=1,g_capacity=40;
static vector<InvItem> g_inv;
static vector<WorldItem> g_items;
static bool g_flashOn=true;
static bool g_invOpen=false,g_chatOpen=false,g_dead=false,g_helpOpen=false;
static int g_hotbarSel=0,g_selSlot=-1,g_hoverSlot=-1;
static int g_dragId=0,g_dragCnt=0;
static int g_ammo=30; static float g_reloadT=0;
static float g_time=0,g_dt=0,g_fps=0,g_walkT=0,g_stepT=0;
static float g_eatAnim=0,g_drinkAnim=0;
static int g_menuSel=0,g_pauseSel=0;
static int g_menuPage=0;              // 0=main,1=single,2=multi
static bool g_multiplayer=false;      // entered via 多人游戏
static wstring g_pendingJoin;         // IP:port to join after world generated
static wstring g_playerName=L"幸存者",g_nameBuf,g_chatBuf;
static vector<wstring> g_msgs,g_chatLog;
static vector<MsgT> g_views;
static Vec3 g_camPos; static Vec3 g_camFwd; static Mat4 g_view,g_proj;
static wstring g_loadText=L"正在加载...";
static int simKey[256]={0};
static wstring ItemName(int id){
    switch(id){case 1:return L"银杏水(杏仁水)";case 2:return L"腐肉块";case 3:return L"手电筒";
    case 4:return L"生存手册";case 5:return L"AKM 步枪";default:return L"空";}
}
static float MaxF(float a,float b){return a>b?a:b;}
