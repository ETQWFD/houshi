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
struct Vec4{ float x,y,z,w; };
static Vec4 Vec4Mul(const Mat4&m,const Vec4&v){
    Vec4 r;
    r.x=m.m[0]*v.x+m.m[4]*v.y+m.m[8]*v.z+m.m[12]*v.w;
    r.y=m.m[1]*v.x+m.m[5]*v.y+m.m[9]*v.z+m.m[13]*v.w;
    r.z=m.m[2]*v.x+m.m[6]*v.y+m.m[10]*v.z+m.m[14]*v.w;
    r.w=m.m[3]*v.x+m.m[7]*v.y+m.m[11]*v.z+m.m[15]*v.w;
    return r;
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
