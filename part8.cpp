// ==================== autotest (dev verification harness) ====================
static int simKey[256]={0};
static struct AutoCmd{ int type; char txt[128]; float t; };
static vector<AutoCmd> g_auto; static size_t g_autoIdx=0; static float g_autoT=0;
static void AutoLoad(){
    FILE* f=fopen("autotest.cfg","r");
    if(!f) return;
    char line[256];
    while(fgets(line,sizeof(line),f)){
        AutoCmd c; memset(&c,0,sizeof(c)); c.t=0;
        char cmd[64]; char arg[128]={0};
        if(sscanf(line,"%63s %127[^\n]",cmd,arg)>=1){
            if(strcmp(cmd,"hold")==0){ c.type=1; strncpy(c.txt,arg,127); c.t=0; }
            else if(strcmp(cmd,"press")==0){ c.type=2; strncpy(c.txt,arg,127); }
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
static void AutoUpdate(float dt){
    if(g_autoIdx>=g_auto.size()) return;
    AutoCmd&c=g_auto[g_autoIdx];
    if(c.type==1){ // hold
        int vk=KeyToVK(c.txt);
        if(vk){ simKey[vk]=1; c.t+=dt; }
        else c.type=-1;
        if(c.t>=atof(c.txt)||c.type==-1){ if(vk) simKey[vk]=0; c.type=-1; }
        // fallthrough when done
    } else if(c.type==2){ // press 120ms
        int vk=KeyToVK(c.txt);
        if(vk) simKey[vk]=1;
        c.t+=dt;
        if(c.t>=0.12f){ if(vk) simKey[vk]=0; c.type=-1; }
    } else if(c.type==3){ // wait
        c.t+=dt;
        if(c.t>=atof(c.txt)) c.type=-1;
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
static void RenderLoadingFrames(int n){
    for(int i=0;i<n;i++){
        MSG m2; while(PeekMessageW(&m2,NULL,0,0,PM_REMOVE)){ TranslateMessage(&m2); DispatchMessageW(&m2); }
        DrawLoadingFrame();
        Sleep(200);
    }
}
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
    // loading frames need these two ready
    { Canvas c(8,8); CanvasFillRect(c,0,0,8,8,255,255,255); g_texWhite=c.Upload(); }
    MakeBaseMeshes();
    g_loadText=L"正在初始化 OpenGL 渲染器 ..."; RenderLoadingFrames(1);
    CompilePrograms();
    g_loadText=L"正在编译着色器 (OpenGL 3.3) ..."; RenderLoadingFrames(2);
    MakeAllTextures();
    MakeShadowFBO();
    g_loadText=L"正在生成世界与音效 ..."; RenderLoadingFrames(2);
    GenSounds();
    g_inv.assign(40,InvItem{0,0});
    g_inv[0].id=3; g_inv[0].cnt=1;
    g_inv[1].id=4; g_inv[1].cnt=1;
    g_inv[2].id=1; g_inv[2].cnt=2;
    g_inv[3].id=2; g_inv[3].cnt=2;
    g_pos=SpawnPos();
    UpdateChunks();
    g_gamestate=GAME_MENU;
    AutoLoad();
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
        AutoUpdate(dt);
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
