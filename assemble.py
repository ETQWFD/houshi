#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import io,sys,os
sys.stdout=io.TextIOWrapper(sys.stdout.buffer,encoding='utf-8',errors='replace')
D="/home/user/Doubao/chats/38443834867513602/houshi/"
def rd(n):
    with open(D+n,"r",encoding="utf-8",errors="replace") as f: return f.read()
p0=rd("part0.cpp"); p1=rd("part1.cpp"); p2=rd("part2.cpp"); p3=rd("part3.cpp")
p4=rd("part4.cpp"); p5=rd("part5.cpp"); p6=rd("part6.cpp"); p7=rd("part7.cpp"); p8=rd("part8.cpp")

# ---------- split part1 ----------
# p1a: from top through Noise1 (math/hash/noise) + GL block (which is before math)
# p1 order: includes..LoadGL (GL section), then math, hash/noise, then mesh, then textures
marker_mesh="#define glGenVertexArrays glGenVertexArrays_pp"  # GL defines come first
i_gl=p1.find("// ---------------- GL function pointers")
i_math=p1.find("// ==================== math ====================")
i_hash=p1.find("// ==================== hash / noise ====================")
i_mesh=p1.find("// ==================== mesh ====================")
i_tex=p1.find("// ==================== textures (procedural) ====================")
assert i_gl>=0 and i_math>i_gl and i_hash>i_math and i_mesh>i_hash and i_tex>i_mesh, (i_gl,i_math,i_hash,i_mesh,i_tex)
p1a=p1[:i_mesh]          # includes, GL, math, hash/noise
p1a=p1a.replace("#include <winsock2.h>\n","#ifndef _WIN32_WINNT\n#define _WIN32_WINNT 0x0600\n#endif\n#include <winsock2.h>\n#include <ws2tcpip.h>\n",1)
p1b=p1[i_mesh:i_tex]     # mesh section
p1c=p1[i_tex:]           # textures

# move Canvas struct from p1c into p2 (so free helpers can use it)
i_cs=p1c.find("struct Canvas {")
i_cs_end=p1c.find("static GLuint g_texWall")
assert i_cs>=0 and i_cs_end>i_cs
canvas_struct=p1c[i_cs:i_cs_end]
p1c=p1c[:i_cs]+p1c[i_cs_end:]
p2="// ==================== canvas ====================\n"+canvas_struct+"\n"+p2

# chunk constants into part2 before chunk section
p2=p2.replace("// ==================== chunks: infinite backrooms ====================",
"// ==================== chunks: infinite backrooms ====================\n"
"static const float CELLSZ=8.0f, ROOMH=3.0f, CHUNKSZ=32.0f;\n"
"static const int CELLS=4;\n")

# P1: remove broken MeshQuadXY
import re
p1b=re.sub(r"static void MeshQuadXY\(Mesh&m,float w,float h\)\{.*?\n\}\n", "", p1b, flags=re.S)
# P1 textures: free-function patch
p1c=p1c.replace("c.FillRect(","CanvasFillRect(c,").replace("c.FillCircle(","CanvasFillCircle(c,").replace("c.Line(","CanvasLine(c,")

# ---------- P2: chunk 3-mesh ----------
old_chunk="""struct Chunk{
    int cx,cz; bool gen;
    vector<AABB> walls;
    vector<Vec3> panels;   // light sources (ceiling centers)
    Mesh mesh;
    Chunk():cx(0),cz(0),gen(false){}
};"""
new_chunk="""struct Chunk{
    int cx,cz; bool gen;
    vector<AABB> walls;
    vector<Vec3> panels;
    Mesh meshWall, meshFC, meshPanel;
    Chunk():cx(0),cz(0),gen(false){}
};"""
assert old_chunk in p2
p2=p2.replace(old_chunk,new_chunk)

old_build="""static void BuildChunk(int cx,int cz){
    int key=ChunkKey(cx,cz);
    Chunk&ch=g_chunks[key]; ch.cx=cx; ch.cz=cz;
    ch.walls.clear(); ch.panels.clear();
    GeoBuild g;
    float ox=cx*CHUNKSZ, oz=cz*CHUNKSZ;
    for(int j=0;j<CELLS;j++)for(int i=0;i<CELLS;i++){
        int gi=cx*CELLS+i, gj=cz*CELLS+j; // global cell index
        float x0=ox+i*CELLSZ, z0=oz+j*CELLSZ;
        float floorY=CellFloor(gi,gj);
        bool solid=CellSolid(gi,gj);
        // floor & ceiling for open cells (draw always to avoid holes)
        Vec3 n(0,1,0);
        GeoQuad(g,Vec3(x0,floorY,z0),Vec3(x0+CELLSZ,floorY,z0),Vec3(x0+CELLSZ,floorY,z0+CELLSZ),Vec3(x0,floorY,z0+CELLSZ),n,gi*2.0f,gj*2.0f,(gi+1)*2.0f,(gj+1)*2.0f);
        Vec3 n2(0,-1,0);
        GeoQuad(g,Vec3(x0,ROOMH,z0+CELLSZ),Vec3(x0+CELLSZ,ROOMH,z0+CELLSZ),Vec3(x0+CELLSZ,ROOMH,z0),Vec3(x0,ROOMH,z0),n2,gi*2.0f,gj*2.0f,(gi+1)*2.0f,(gj+1)*2.0f);
        // fluorescent panel on ceiling of open cells
        ch.panels.push_back(Vec3(x0+CELLSZ/2,ROOMH-0.06f,z0+CELLSZ/2));
        float pw=3.2f;
        Vec3 pc(x0+CELLSZ/2,ROOMH-0.03f,z0+CELLSZ/2);
        GeoQuad(g,Vec3(pc.x-pw,pc.y,pc.z-pw*0.5f),Vec3(pc.x+pw,pc.y,pc.z-pw*0.5f),Vec3(pc.x+pw,pc.y,pc.z+pw*0.5f),Vec3(pc.x-pw,pc.y,pc.z+pw*0.5f),Vec3(0,-1,0),0,0,1,1);
        if(solid){
            bool broken=Hash01(gi*3+1,gj*7+2)>0.90f;
            float top=broken?1.0f:ROOMH; // broken wall stub
            AABB w; w.mn=Vec3(x0,0,z0); w.mx=Vec3(x0+CELLSZ,top,z0+CELLSZ);
            ch.walls.push_back(w);
            // wall faces toward neighbors
            Vec3 c(x0+CELLSZ/2,top/2,z0+CELLSZ/2);
            Vec3 h(CELLSZ/2,top/2,CELLSZ/2);
            if(!CellSolid(gi-1,gj)) GeoQuad(g,Vec3(x0,0,z0),Vec3(x0,top,z0),Vec3(x0,top,z0+CELLSZ),Vec3(x0,0,z0+CELLSZ),Vec3(-1,0,0),0,0,CELLSZ*0.25f,ROOMH*0.25f);
            if(!CellSolid(gi+1,gj)) GeoQuad(g,Vec3(x0+CELLSZ,0,z0+CELLSZ),Vec3(x0+CELLSZ,top,z0+CELLSZ),Vec3(x0+CELLSZ,top,z0),Vec3(x0+CELLSZ,0,z0),Vec3(1,0,0),0,0,CELLSZ*0.25f,ROOMH*0.25f);
            if(!CellSolid(gi,gj-1)) GeoQuad(g,Vec3(x0+CELLSZ,0,z0),Vec3(x0+CELLSZ,top,z0),Vec3(x0,top,z0),Vec3(x0,0,z0),Vec3(0,0,-1),0,0,CELLSZ*0.25f,ROOMH*0.25f);
            if(!CellSolid(gi,gj+1)) GeoQuad(g,Vec3(x0,0,z0+CELLSZ),Vec3(x0,top,z0+CELLSZ),Vec3(x0+CELLSZ,top,z0+CELLSZ),Vec3(x0+CELLSZ,0,z0+CELLSZ),Vec3(0,0,1),0,0,CELLSZ*0.25f,ROOMH*0.25f);
        }
    }
    // build mesh
    Mesh m=M0();
    BuildMesh(m,g.v,g.idx,8);
    ch.mesh=m; ch.gen=true;
}"""
new_build="""static void BuildChunk(int cx,int cz){
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
        Vec3 pc(x0+CELLSZ/2,ROOMH-0.12f,z0+CELLSZ/2);
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
}"""
assert old_build in p2, "build chunk not found"
p2=p2.replace(old_build,new_build)

old_del="""        if(abs(it->second.cx-pcx)>R+2||abs(it->second.cz-pcz)>R+2){
            if(it->second.mesh.vao){ glDeleteVertexArrays(1,&it->second.mesh.vao); glDeleteBuffers(2,&it->second.mesh.vbo); }
            it=g_chunks.erase(it);
        } else ++it;"""
new_del="""        if(abs(it->second.cx-pcx)>R+2||abs(it->second.cz-pcz)>R+2){
            if(it->second.meshWall.vao){ glDeleteVertexArrays(1,&it->second.meshWall.vao); glDeleteBuffers(2,&it->second.meshWall.vbo); }
            if(it->second.meshFC.vao){ glDeleteVertexArrays(1,&it->second.meshFC.vao); glDeleteBuffers(2,&it->second.meshFC.vbo); }
            if(it->second.meshPanel.vao){ glDeleteVertexArrays(1,&it->second.meshPanel.vao); glDeleteBuffers(2,&it->second.meshPanel.vbo); }
            it=g_chunks.erase(it);
        } else ++it;"""
assert old_del in p2
p2=p2.replace(old_del,new_del)

# append Fbm2/Noise2 at end of part2 (before textures)
fbm="""
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
"""
p2=p2+fbm

# ---------- P4 ----------
old_in="static int g_prevF=0,g_prevF5=0,g_prevE=0,g_prevQ=0,g_prevT=0,g_prevEsc=0,g_prevF9=0,g_prevR=0,g_prevO=0;"
new_in="static int g_prevF=0,g_prevF5=0,g_prevE=0,g_prevQ=0,g_prevT=0,g_prevEsc=0,g_prevF9=0,g_prevR=0,g_prevO=0,g_prevUp=0,g_prevDown=0,g_prevEnter=0;"
assert old_in in p4
p4=p4.replace(old_in,new_in)
p4=p4.replace("static int g_prevUp=0,g_prevDown=0,g_prevEnter=0,g_prevEsc2=0;\n","")
p4=p4.replace("        if(nEsc&&0){} // handled above (toggle)\n","")
old_kd="static bool KeyDn(int v){ return (GetAsyncKeyState(v)&0x8000)!=0; }"
new_kd="static bool KeyDn(int v){ if(simKey[v]) return true; return (GetAsyncKeyState(v)&0x8000)!=0; }"
assert old_kd in p4
p4=p4.replace(old_kd,new_kd)

# ---------- P3: AddMsg/AddView take wstring ----------
p3=p3.replace("static void AddMsg(const wchar_t* s){ g_msgs.push_back(s); if((int)g_msgs.size()>6) g_msgs.erase(g_msgs.begin()); }",
              "static void AddMsg(const wstring& s){ g_msgs.push_back(s); if((int)g_msgs.size()>6) g_msgs.erase(g_msgs.begin()); }")
p3=p3.replace("static void AddView(const wchar_t* s){ MsgT m; m.s=s; m.t=4.0f; g_views.push_back(m); if((int)g_views.size()>8) g_views.erase(g_views.begin()); }",
              "static void AddView(const wstring& s){ MsgT m; m.s=s; m.t=4.0f; g_views.push_back(m); if((int)g_views.size()>8) g_views.erase(g_views.begin()); }")

# ---------- P5 ----------
p5=p5.replace("static Mat4 mvp0=M4Id();\n","")
old_mvp="""        Mat4 mvp=mvp0; mvp=M4Mul(g_shadowMVP,M4Id());
        glUniformMatrix4fv(U(progShadow,"uMVP"),1,GL_FALSE,mvp.m);"""
new_mvp="""        Mat4 mvp=M4Mul(g_shadowMVP,M4Id());
        glUniformMatrix4fv(U(progShadow,"uMVP"),1,GL_FALSE,mvp.m);"""
assert old_mvp in p5
p5=p5.replace(old_mvp,new_mvp)
anchor="static void SetWorldUniforms(GLuint p,const Mat4&vp,const Mat4&model,const Mat4&normM,GLuint tex,const Vec3&tint){"
assert anchor in p5
p5=p5.replace(anchor,"static Mat4 g_shadowMVP;\n"+anchor)
p5=p5.replace("static Mat4 g_shadowMVP;\nstatic void RenderShadowMap","static void RenderShadowMap")
# remove the LAST occurrence of the g_shadowMVP declaration (keep the one before SetWorldUniforms)
occ=p5.count("static Mat4 g_shadowMVP;\n")
if occ>1:
    i=p5.rfind("static Mat4 g_shadowMVP;\n")
    p5=p5[:i]+p5[i+len("static Mat4 g_shadowMVP;\n"):]
anchor2="static void DrawItems(const Mat4&vp){"
assert anchor2 in p5
p5=p5.replace(anchor2,"static void DrawAKM(const Mat4&vp,const Mat4&model);\n"+anchor2)

# ---------- P6 ----------
# reorder: Vec4+Vec4Mul must come BEFORE DrawText3D, and the pair before DrawRemotePlayers
m=re.search(r"static void DrawText3D\(.*?\n\}\n(struct Vec4\{.*?\};\nstatic Vec4 Vec4Mul\(.*?\n\}\n)", p6, flags=re.S)
assert m, "drawtext3d block not found"
block4=m.group(1)
block4=block4.replace("struct Vec4{ float x,y,z,w; };","struct Vec4{ float x,y,z,w; Vec4(){} Vec4(float a,float b,float c,float d):x(a),y(b),z(c),w(d){} };")
block3d=m.group(0)[:m.group(0).find("struct Vec4")]
p6=p6.replace(m.group(0),"\n")
anchor6="static void DrawRemotePlayers(const Mat4&vp){"
assert anchor6 in p6
p6=p6.replace(anchor6,block4+block3d+anchor6)

# ---------- P7: views display ----------
old_msg="""    // messages
    float my=240;
    for(size_t i=0;i<g_msgs.size();i++){
        DrawText(18,my,g_msgs[i],18,Vec3(0.95f,0.95f,0.9f),1.0f);
        my+=24;
    }"""
new_msg=old_msg+"""
    for(size_t i=0;i<g_views.size();i++){
        DrawText(18,my,g_views[i].s,20,Vec3(1.0f,0.9f,0.5f),1.0f);
        my+=26;
    }"""
assert old_msg in p7
p7=p7.replace(old_msg,new_msg)

# ---------- P8 ----------
p8=p8.replace("static int simKey[256]={0};\n","")
p8=p8.replace("static struct AutoCmd{ int type; char txt[128]; float t; };","struct AutoCmd{ int type; char txt[128]; float t; };")
p8=p8.replace("AutoLoad","LoadAutoTest").replace("AutoUpdate","UpdateAutoTest")
old_al="""            if(strcmp(cmd,"hold")==0){ c.type=1; strncpy(c.txt,arg,127); c.t=0; }
            else if(strcmp(cmd,"press")==0){ c.type=2; strncpy(c.txt,arg,127); }"""
new_al="""            if(strcmp(cmd,"hold")==0||strcmp(cmd,"press")==0){
                c.type=(cmd[0]=='h')?1:2;
                char key[32]={0}; float dur=0.12f;
                sscanf(arg,"%31s %f",key,&dur);
                strncpy(c.txt,key,127); c.t=dur<0.001f?0.12f:dur;
            }"""
assert old_al in p8
p8=p8.replace(old_al,new_al)

old_au="""    if(c.type==1){ // hold
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
    }"""
new_au="""    if(c.type==1||c.type==2){ // hold / press
        int vk=KeyToVK(c.txt);
        if(vk) simKey[vk]=1;
        c.t-=dt;
        if(c.t<=0){ if(vk) simKey[vk]=0; c.type=-1; }
    } else if(c.type==3){ // wait
        c.t-=dt;
        if(c.t<=0) c.type=-1;
    }"""
assert old_au in p8
p8=p8.replace(old_au,new_au)

final=(p1a+"\n"+p0+"\n"+p1b+"\n"+p2+"\n"+p1c+"\n"+p3+"\n"+p4+"\n"+p5+"\n"+p6+"\n"+p7+"\n"+p8)
with open(D+"houshi.cpp","w",encoding="utf-8") as f:
    f.write(final)
print("assembled houshi.cpp, bytes=",os.path.getsize(D+"houshi.cpp"))
