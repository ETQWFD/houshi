// ==================== input ====================
static bool KeyDn(int v){ return (GetAsyncKeyState(v)&0x8000)!=0; }
static int g_prevF=0,g_prevF5=0,g_prevE=0,g_prevQ=0,g_prevT=0,g_prevEsc=0,g_prevF9=0,g_prevR=0,g_prevO=0,g_prevH=0;
static bool g_prevL=false,g_prevRbtn=false;
static bool g_mouseLock=false;

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
    // ---- mouse look (captured cursor, first/third person) ----
    bool wantLock=(g_gamestate==GAME_PLAY&&!g_invOpen&&!g_chatOpen&&!g_dead&&!g_helpOpen&&g_cam!=CM_GOD);
    if(wantLock&&!g_mouseLock){
        g_mouseLock=true; ShowCursor(FALSE);
        RECT rc; GetClientRect(g_hWnd,&rc); POINT c={rc.right/2,rc.bottom/2};
        ClientToScreen(g_hWnd,&c); SetCursorPos(c.x,c.y);
    } else if(!wantLock&&g_mouseLock){
        g_mouseLock=false; ShowCursor(TRUE);
    }
    if(g_mouseLock){
        POINT p; GetCursorPos(&p);
        RECT rc; GetClientRect(g_hWnd,&rc); POINT c={rc.right/2,rc.bottom/2};
        ClientToScreen(g_hWnd,&c);
        int dx=p.x-c.x, dy=p.y-c.y;
        if(dx||dy){
            const float sens=0.0022f;
            g_yaw+=dx*sens;
            g_pitch+=dy*sens;
            if(g_pitch>1.5f) g_pitch=1.5f;
            if(g_pitch<-1.5f) g_pitch=-1.5f;
            SetCursorPos(c.x,c.y);
        }
    }
    for(size_t i=0;i<g_views.size();i++) g_views[i].t-=dt;
    for(size_t i=0;i<g_views.size();) { if(g_views[i].t<=0) g_views.erase(g_views.begin()+i); else i++; }
    if(g_eatAnim>0) g_eatAnim=MaxF(0,g_eatAnim-dt*2.5f);
    if(g_drinkAnim>0) g_drinkAnim=MaxF(0,g_drinkAnim-dt*2.5f);
    if(g_recoil>0) g_recoil=MaxF(0,g_recoil-dt*4.0f);
    if(g_muzzleT>0) g_muzzleT--;
    if(g_tracerT>0) g_tracerT-=dt;
    if(g_reloadT>0){ g_reloadT--; if(g_reloadT==1){ g_ammo=30; AddMsg(L"换弹完成"); } }

    // help panel (H)
    int nH=KeyDn('H')?1:0;
    if(nH&&!g_prevH&&!g_chatOpen&&!g_invOpen){ g_helpOpen=!g_helpOpen; }
    g_prevH=nH;
    // chat input
    int nT=KeyDn('T')?1:0;
    if(nT&&!g_prevT&&g_gamestate==GAME_PLAY&&!g_invOpen&&!g_helpOpen){ g_chatOpen=!g_chatOpen; g_chatBuf.clear(); }
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
        if(nEsc&&0){} // handled above (toggle)
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
static void StartNewGame(bool multi){
    if(g_nameBuf.empty()) g_nameBuf=L"幸存者";
    g_playerName=g_nameBuf; g_nameBuf.clear();
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
    g_multiplayer=multi;
    g_pendingJoin.clear();
    g_gamestate=GAME_GENERATING;
    g_loadText=multi?L"正在生成多人世界 ...":L"正在生成无限迷宫 ...";
    g_cam=CM_FP;
    if(multi){ g_broadcastOn=true; AddMsg(L"多人模式已开启广播，好友可用 /join 你的联机码加入"); }
    AddMsg(L"欢迎来到《后世》。你被困在了无限迷宫中。");
    AddMsg(L"WASD移动 空格跳 Shift疾跑 E背包 数字键切换物品 F手电 F5视角 T聊天");
    AddMsg(L"指令: /giop @s akm 获得步枪  /join IP:端口 联机");
}
static void UpdateMenu(float dt){
    g_dt=dt;
    int nHm=KeyDn('H')?1:0;
    if(nHm&&!g_prevH){ g_helpOpen=!g_helpOpen; Sfx(L"sfx_click.wav"); }
    g_prevH=nHm;
    if(KeyDn(VK_UP)&&!g_prevUp){ if(g_gamestate==GAME_MENU){ g_menuSel=(g_menuSel+2)%3; } else g_pauseSel=(g_pauseSel+2)%3; Sfx(L"sfx_click.wav"); }
    if(KeyDn(VK_DOWN)&&!g_prevDown){ if(g_gamestate==GAME_MENU){ g_menuSel=(g_menuSel+1)%3; } else g_pauseSel=(g_pauseSel+1)%3; Sfx(L"sfx_click.wav"); }
    g_prevUp=KeyDn(VK_UP)?1:0; g_prevDown=KeyDn(VK_DOWN)?1:0;
    if(KeyDn(VK_RETURN)&&!g_prevEnter){
        g_prevEnter=1;
        if(g_gamestate==GAME_MENU){
            if(g_menuPage==0){
                if(g_menuSel==0){ g_menuPage=1; g_menuSel=0; Sfx(L"sfx_click.wav"); }        // 单人游戏
                else if(g_menuSel==1){ g_menuPage=2; g_menuSel=0; Sfx(L"sfx_click.wav"); }    // 多人游戏
                else { SaveGame(); PostQuitMessage(0); }
            } else if(g_menuPage==1){
                if(g_menuSel==0){ StartNewGame(false); }          // 开始新游戏（单人）
                else if(g_menuSel==1){                            // 继续游戏
                    if(LoadGame()){
                        g_chunks.clear();
                        g_gamestate=GAME_GENERATING;
                        g_loadText=L"正在载入存档世界 ...";
                        g_dead=false; g_cam=CM_FP; g_multiplayer=false;
                        AddMsg(L"已读取存档，欢迎回到《后世》");
                    } else AddMsg(L"没有找到存档！请先创建新游戏");
                } else { g_menuPage=0; g_menuSel=0; g_nameBuf.clear(); }
            } else {                                              // 多人子菜单
                if(g_menuSel==0){ StartNewGame(true); }           // 开始新游戏（多人）
                else if(g_menuSel==1){                            // 加入联机（输入 IP:端口）
                    wstring ip=g_nameBuf; g_nameBuf.clear();
                    if(!ip.empty()){ StartNewGame(true); g_pendingJoin=ip; }
                    else AddMsg(L"请输入好友的 公网IP:端口 再按回车");
                } else { g_menuPage=0; g_menuSel=0; g_nameBuf.clear(); }
            }
        } else if(g_gamestate==GAME_PAUSE){
            if(g_pauseSel==0){ g_gamestate=GAME_PLAY; }
            else if(g_pauseSel==1){ SaveGame(); g_gamestate=GAME_MENU; g_menuPage=0; g_menuSel=1; }
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
static int g_prevUp=0,g_prevDown=0,g_prevEnter=0,g_prevEsc2=0;
