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

// ==================== humanoid model (realistic proportions) ====================
// skin, shirt, pants textures + black for hair/shoes
static Vec3 CLR_SKIN=Vec3(1,1,1), CLR_SHIRT=Vec3(1,1,1), CLR_PANTS=Vec3(1,1,1), CLR_DARK=Vec3(1,1,1);
static Mat4 PartM(const Mat4&parent,const Vec3&t,const Mat4&rot,const Vec3&s){
    return M4Mul(parent,M4Mul(M4T(t),M4Mul(rot,M4S(s))));
}
static void PartMesh(const Mat4&vp,const Mat4&m,const Mesh&mesh,GLuint tex,const Vec3&tint){
    Mat4 rot=M4Id(); rot.m[0]=m.m[0]; rot.m[1]=m.m[1]; rot.m[2]=m.m[2];
    rot.m[4]=m.m[4]; rot.m[5]=m.m[5]; rot.m[6]=m.m[6];
    rot.m[8]=m.m[8]; rot.m[9]=m.m[9]; rot.m[10]=m.m[10];
    DrawObjRot(vp,m,rot,mesh,tex,tint);
}
static void Part(const Mat4&vp,const Mat4&m,const Mesh&mesh,GLuint tex,const Vec3&tint){
    PartMesh(vp,m,mesh,tex,tint);
}
// cylinder limb helper: meshCyl is a unit cylinder along Y, scale = (r, len, r)
static Mat4 Limb(const Mat4&parent,const Vec3&t,float r,float len){
    return M4Mul(parent,M4Mul(M4T(t),M4S(Vec3(r,len,r))));
}
static void DrawFingers(const Mat4&vp,const Mat4&handM,int dir){
    for(int i=0;i<5;i++){
        float fx=dir*0.022f*(i-2);
        Mat4 f=PartM(handM,Vec3(fx,-0.02f,0.05f),M4RX(0.4f),Vec3(1,1,1));
        // finger capsule along +z (meshCyl along Y -> rotate Y to Z)
        Mat4 fm=M4Mul(f,M4Mul(M4T(Vec3(0,0,0.045f)),M4Mul(M4RX(PI/2),M4S(Vec3(0.011f,0.085f,0.011f)))));
        PartMesh(vp,fm,meshCyl,g_texSkin,CLR_SKIN);
        // fingertip ball
        Mat4 tip=M4Mul(f,M4Mul(M4T(Vec3(0,0,0.095f)),M4S(Vec3(0.011f,0.011f,0.011f))));
        PartMesh(vp,tip,meshSphere,g_texSkin,CLR_SKIN);
    }
}
static void DrawArm(const Mat4&vp,const Mat4&shoulder,float swing,int dir,GLuint upperTex){
    PartMesh(vp,Limb(shoulder,Vec3(0,-0.14f,0),0.055f,0.36f),meshCyl,upperTex,CLR_SHIRT);
    PartMesh(vp,Limb(shoulder,Vec3(0,-0.36f,0),0.045f,0.30f),meshCyl,g_texSkin,CLR_SKIN);
    PartMesh(vp,M4Mul(shoulder,M4Mul(M4T(Vec3(0,-0.52f,0)),M4S(Vec3(0.06f,0.075f,0.07f)))),meshBox,g_texSkin,CLR_SKIN);
    DrawFingers(vp,M4Mul(shoulder,M4Mul(M4T(Vec3(0,-0.60f,0.02f)),M4S(Vec3(1,1,1)))),dir);
}
static void DrawLeg(const Mat4&vp,const Mat4&hip,float swing){
    PartMesh(vp,Limb(hip,Vec3(0,-0.24f,0),0.080f,0.55f),meshCyl,g_texPants,CLR_PANTS);
    PartMesh(vp,M4Mul(hip,M4Mul(M4T(Vec3(0,-0.50f,0.01f)),M4S(Vec3(0.056f,0.056f,0.056f)))),meshSphere,g_texPants,CLR_PANTS);
    PartMesh(vp,Limb(hip,Vec3(0,-0.64f,0.01f),0.063f,0.48f),meshCyl,g_texPants,CLR_PANTS);
    Part(vp,M4Mul(hip,M4Mul(M4T(Vec3(0,-0.91f,0.11f)),M4S(Vec3(0.105f,0.08f,0.27f)))),meshBox,g_texPants,CLR_DARK);
}
static void DrawHumanoid(const Mat4&vp,const Vec3&root,float yaw,float pitch,bool isLocal,float ph){
    // root at feet; realistic body ~1.9m
    Mat4 base=M4Mul(M4T(root),M4RY(yaw));
    float sw=sinf(ph)*0.7f, swA=sinf(ph)*0.55f;
    float bend=pitch*0.5f;
    // pelvis, waist, chest (shoulders wider than waist)
    Part(vp,PartM(base,Vec3(0,1.00f,0),M4Id(),Vec3(0.30f,0.15f,0.20f)),meshBox,g_texPants,CLR_PANTS);
    Part(vp,PartM(base,Vec3(0,1.16f,0),M4RX(bend*0.2f),Vec3(0.30f,0.11f,0.18f)),meshBox,g_texShirt,CLR_SHIRT);
    Part(vp,PartM(base,Vec3(0,1.40f,0),M4RX(bend*0.3f),Vec3(0.52f,0.46f,0.28f)),meshBox,g_texShirt,CLR_SHIRT);
    // rounded shoulders
    PartMesh(vp,PartM(base,Vec3(-0.28f,1.52f,0),M4Id(),Vec3(0.075f,0.075f,0.075f)),meshSphere,g_texShirt,CLR_SHIRT);
    PartMesh(vp,PartM(base,Vec3(0.28f,1.52f,0),M4Id(),Vec3(0.075f,0.075f,0.075f)),meshSphere,g_texShirt,CLR_SHIRT);
    // head: ellipsoid skull + hair cap + eyes
    Mat4 headM=PartM(base,Vec3(0,1.82f,0),M4RX(bend),Vec3(1,1,1));
    DrawObjRot(vp,M4Mul(headM,M4S(Vec3(0.105f,0.135f,0.115f))),headM,meshSphere,g_texSkin,CLR_SKIN);
    DrawObjRot(vp,M4Mul(PartM(base,Vec3(0,1.91f,0),M4RX(bend),Vec3(1,1,1)),M4S(Vec3(0.115f,0.06f,0.125f))),headM,meshSphere,g_texPants,CLR_DARK);
    PartMesh(vp,PartM(base,Vec3(-0.045f,1.84f,0.095f),M4Id(),Vec3(0.022f,0.016f,0.012f)),meshSphere,g_texPants,CLR_DARK);
    PartMesh(vp,PartM(base,Vec3(0.045f,1.84f,0.095f),M4Id(),Vec3(0.022f,0.016f,0.012f)),meshSphere,g_texPants,CLR_DARK);
    // legs
    DrawLeg(vp,PartM(base,Vec3(-0.11f,1.02f,0),M4RX(sw),Vec3(1,1,1)),sw);
    DrawLeg(vp,PartM(base,Vec3(0.11f,1.02f,0),M4RX(-sw),Vec3(1,1,1)),-sw);
    // arms
    DrawArm(vp,PartM(base,Vec3(-0.28f,1.52f,0),M4RX(swA*0.6f+bend*0.4f),Vec3(1,1,1)),swA,-1,g_texShirt);
    DrawArm(vp,PartM(base,Vec3(0.28f,1.52f,0),M4RX(-swA*0.6f+bend*0.4f),Vec3(1,1,1)),swA,1,g_texShirt);
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
static void DrawHandsFP(const Mat4&vp){
    Mat4 camBase=M4Mul(M4T(g_camPos),M4Mul(M4RY(g_yaw),M4RX(-g_pitch)));
    Vec3 bob(0,sinf(g_walkT*2.2f)*0.015f,0);
    camBase=M4Mul(M4T(bob),camBase);
    float recoil=g_recoil*0.08f;
    float eatR=(g_eatAnim>0.5f||g_drinkAnim>0.5f)?1.0f:0.0f;
    // right arm (cylinder limbs + realistic hand with fingers)
    Mat4 shR=PartM(camBase,Vec3(0.30f,-0.30f,-0.48f),M4RX(-0.4f+eatR*1.3f+recoil),Vec3(1,1,1));
    PartMesh(vp,Limb(shR,Vec3(0,-0.10f,0.10f),0.055f,0.32f),meshCyl,g_texShirt,CLR_SHIRT);
    PartMesh(vp,Limb(shR,Vec3(0,-0.28f,0.20f),0.045f,0.26f),meshCyl,g_texSkin,CLR_SKIN);
    Mat4 handR=PartM(shR,Vec3(0,-0.40f,0.28f),M4Id(),Vec3(0.06f,0.075f,0.07f));
    PartMesh(vp,handR,meshBox,g_texSkin,CLR_SKIN);
    DrawFingers(vp,M4Mul(handR,M4T(Vec3(0,0.01f,0.02f))),1);
    // held item in right hand (by hotbar slot)
    Mat4 hold=M4Mul(handR,M4T(Vec3(0,0,0.10f)));
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
    Mat4 shL=PartM(camBase,Vec3(-0.30f,-0.30f,-0.48f),M4RX(-0.4f+eatR*1.0f),Vec3(1,1,1));
    PartMesh(vp,Limb(shL,Vec3(0,-0.10f,0.10f),0.055f,0.32f),meshCyl,g_texShirt,CLR_SHIRT);
    PartMesh(vp,Limb(shL,Vec3(0,-0.28f,0.18f),0.045f,0.26f),meshCyl,g_texSkin,CLR_SKIN);
    Mat4 handL=PartM(shL,Vec3(0,-0.40f,0.26f),M4Id(),Vec3(0.06f,0.075f,0.07f));
    PartMesh(vp,handL,meshBox,g_texSkin,CLR_SKIN);
    DrawFingers(vp,M4Mul(handL,M4T(Vec3(0,0.01f,0.02f))),-1);
    // legs & feet when looking down (realistic cylinder legs)
    if(g_pitch<-0.35f){
        Mat4 legBase=M4Mul(M4T(g_pos),M4RY(g_yaw));
        float sw=sinf(g_walkT*1.6f)*0.5f;
        DrawLeg(vp,PartM(legBase,Vec3(-0.11f,1.02f,0),M4RX(sw),Vec3(1,1,1)),sw);
        DrawLeg(vp,PartM(legBase,Vec3(0.11f,1.02f,0),M4RX(-sw),Vec3(1,1,1)),-sw);
    }
}
