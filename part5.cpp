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
            Vec3 dv=p-g_camPos;
            if(dv.dot(dv)<45.0f*45.0f) cand.push_back(p);
        }
    }
    // nearest 8 fluorescent lights by distance (stable, no jumpy pick)
    std::sort(cand.begin(),cand.end(),[&](const Vec3&a,const Vec3&b){
        Vec3 da=a-g_camPos, db=b-g_camPos; return da.dot(da)<db.dot(db); });
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
static Mat4 g_shadowMVP;
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
        Mat4 mvp=mvp0; mvp=M4Mul(g_shadowMVP,M4Id());
        glUniformMatrix4fv(U(progShadow,"uMVP"),1,GL_FALSE,mvp.m);
        if(it->second.meshWall.vao) DrawMesh(it->second.meshWall);
        if(it->second.meshFC.vao) DrawMesh(it->second.meshFC);
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,g_W,g_H);
}
static Mat4 mvp0=M4Id();
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
        // panels: bright emissive (polygon offset avoids z-fight flicker vs ceiling)
        if(ch.meshPanel.vao){
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE);
            glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(-1.0f,-1.0f);
            SetWorldUniforms(progW,vp,M4Id(),M4Id(),g_texPanel,Vec3(2.2f,2.3f,2.5f));
            DrawMesh(ch.meshPanel);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_BLEND);
        }
    }
    glEnable(GL_CULL_FACE);
}
static void DrawItems(const Mat4&vp){
    for(size_t i=0;i<g_items.size();i++){
        WorldItem&it=g_items[i];
        if(it.taken) continue;
        it.rot+=g_dt*1.2f;
        float bob=sinf(g_time*2.0f+it.pos.x)*0.05f;
        Vec3 p=it.pos+Vec3(0,bob,0);
        Mat4 model=M4Mul(M4T(p),M4RY(it.rot));
        if(it.type==1){
            Mat4 m1=M4Mul(model,M4S(Vec3(1.5f,1.5f,1.5f)));
            DrawObjRot(vp,m1,model,meshBottle,g_texBottle,Vec3(1.25f,1.5f,1.8f));
            Mat4 cap=M4Mul(M4T(Vec3(0,0.15f,0)),M4S(Vec3(1.5f,1.5f,1.5f)));
            DrawObjRot(vp,M4Mul(model,cap),model,meshCap,g_texBottle,Vec3(1.4f,1.7f,2.0f));
        } else if(it.type==2){
            Mat4 m2=M4Mul(model,M4S(Vec3(1.6f,1.6f,1.6f)));
            DrawObjRot(vp,m2,model,meshMeat,g_texMeatTex,Vec3(1.5f,1.15f,1.15f));
        } else if(it.type==3){
            Mat4 m3=M4Mul(model,M4S(Vec3(1.5f,1.5f,1.5f)));
            DrawObjRot(vp,m3,model,meshFlashB,g_texMetal,Vec3(1.3f,1.3f,1.45f));
            Mat4 h3=M4Mul(M4T(Vec3(0,0.1f,0)),M4S(Vec3(1.6f,1.6f,1.6f)));
            DrawObjRot(vp,M4Mul(model,h3),model,meshFlashH,g_texMetal,Vec3(1.5f,1.5f,1.6f));
        } else if(it.type==4){
            Mat4 m4=M4Mul(model,M4S(Vec3(1.6f,1.6f,1.6f)));
            DrawObjRot(vp,m4,model,meshBook,g_texBook,Vec3(1.35f,1.3f,1.0f));
        } else if(it.type==5){
            Mat4 m5=M4Mul(model,M4S(Vec3(1.2f,1.2f,1.2f)));
            DrawAKM(vp,m5);
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
