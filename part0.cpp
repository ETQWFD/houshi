// ==================== globals & types (part0) ====================
enum GameState{ GAME_MENU, GAME_GENERATING, GAME_PLAY, GAME_PAUSE, GAME_DEAD };
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
static int g_menuPage=0;              // 0=main,1=single,2=multi
static bool g_multiplayer=false;      // entered via 多人游戏
static wstring g_pendingJoin;         // IP:port to join after world generated
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
