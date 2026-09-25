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
    Mat4 proj=M4Persp(70.0f*PI/180.0f,(float)g_W/(float)g_H,0.1f,120.0f);
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
