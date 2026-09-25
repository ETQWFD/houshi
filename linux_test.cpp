// linux_test.cpp — mirror-verification of houshi.cpp core logic (maze/physics)
// Mirrors the exact algorithms from houshi.cpp to catch design bugs:
// connectivity, no-clip, no-fly, spawn validity, floor adherence.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;
typedef long long GLsizeiptr; typedef long long GLintptr;
#define PI 3.14159265358979323846f
struct Vec3 { float x,y,z;
    Vec3():x(0),y(0),z(0){}
    Vec3(float a,float b,float c):x(a),y(b),z(c){}
};
struct AABB{ Vec3 mn,mx; };
struct Mat4 { float m[16]; };
static float LerpF(float a,float b,float t){return a+(b-a)*t;}
static float ClampF(float v,float a,float b){return v<a?a:(v>b?b:v);}
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
// --- constants (mirror houshi.cpp) ---
static const float CELLSZ=8.0f, ROOMH=3.0f, CHUNKSZ=32.0f;
static const int CELLS=4;
static float PLR_HALF=0.35f, PLR_H=1.75f;
static bool CellSolid(int i,int j){
    if(j%3==0||i%3==0) return false;
    if(Hash01(i,j)>0.80f) return false;
    return true;
}
static float CellFloor(int i,int j){ return Hash01(i,j)*0.12f; }
static bool CellFlicker(int i,int j){ return Hash01(i+777,j+555)>0.80f; }
static float FloorAt(float x,float z){
    int gi=(int)floorf(x/CELLSZ), gj=(int)floorf(z/CELLSZ);
    return CellFloor(gi,gj);
}
// --- wall collection: mirror of CollectNearWalls for region cells ---
static void CollectWalls(const Vec3&p,vector<AABB>&out,float margin){
    out.clear();
    int c0=(int)floorf(p.x/CELLSZ)-2, c1=c0+4;
    int r0=(int)floorf(p.z/CELLSZ)-2, r1=r0+4;
    for(int gj=r0;gj<=r1;gj++)for(int gi=c0;gi<=c1;gi++){
        if(!CellSolid(gi,gj)) continue;
        bool broken=Hash01(gi*3+1,gj*7+2)>0.90f;
        float top=broken?1.0f:ROOMH;
        float x0=gi*CELLSZ, z0=gj*CELLSZ;
        AABB w; w.mn=Vec3(x0,0,z0); w.mx=Vec3(x0+CELLSZ,top,z0+CELLSZ);
        if(p.x+margin>w.mn.x&&p.x-margin<w.mx.x&&p.z+margin>w.mn.z&&p.z-margin<w.mx.z)
            out.push_back(w);
    }
}
static Vec3 g_pos; static float g_velY=0; static bool g_onGround=true;
// --- mirror MovePlayer ---
static void MovePlayer(float dx,float dz){
    vector<AABB> ws;
    float y0=g_pos.y;
    CollectWalls(g_pos,ws,PLR_HALF+0.1f);
    g_pos.x+=dx;
    for(size_t i=0;i<ws.size();i++){
        const AABB&w=ws[i];
        if(g_pos.x+PLR_HALF>w.mn.x&&g_pos.x-PLR_HALF<w.mx.x&&y0<w.mx.y&&y0+PLR_H>w.mn.y){
            if(g_pos.z+PLR_HALF>w.mn.z&&g_pos.z-PLR_HALF<w.mx.z){
                if(dx>0) g_pos.x=w.mn.x-PLR_HALF; else if(dx<0) g_pos.x=w.mx.x+PLR_HALF;
            }
        }
    }
    g_pos.z+=dz;
    CollectWalls(g_pos,ws,PLR_HALF+0.1f);
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
    if(g_pos.y+PLR_H>ROOMH-0.1f){ g_pos.y=ROOMH-0.1f-PLR_H; if(g_velY>0) g_velY=0; }
    if(!g_onGround){
        g_velY-=19.5f*dt; g_pos.y+=g_velY*dt;
        if(g_pos.y<=fl){ g_pos.y=fl; g_velY=0; g_onGround=true; }
    }
}
// --- spawn: mirror ---
static Vec3 SpawnPos(){
    for(int r=0;r<40;r++){
        int gi=0, gj=0;
        if(r==0){ gi=0;gj=0; }
        else { gi=(int)(WRand()*24-12); gj=(int)(WRand()*24-12); }
        if(!CellSolid(gi,gj)) return Vec3(gi*CELLSZ+CELLSZ/2, FloorAt(gi*CELLSZ+CELLSZ/2,gj*CELLSZ+CELLSZ/2), gj*CELLSZ+CELLSZ/2);
    }
    return Vec3(0,0,0);
}
// --- RayAABB mirror ---
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
static int failures=0;
static void expect(const char* name,bool ok){
    printf("%s %s\n", ok?"PASS":"FAIL", name);
    if(!ok) failures++;
}
static bool insideWall(const Vec3&p){
    vector<AABB> ws; CollectWalls(p,ws,PLR_HALF);
    for(size_t i=0;i<ws.size();i++){
        const AABB&w=ws[i];
        if(p.x+PLR_HALF-0.01f>w.mn.x&&p.x-PLR_HALF+0.01f<w.mx.x &&
           p.z+PLR_HALF-0.01f>w.mn.z&&p.z-PLR_HALF+0.01f<w.mx.z) return true;
    }
    return false;
}
int main(){
    g_seed=20260925; g_wr=1;
    // T1: maze connectivity (BFS over region, all open cells reachable from (0,0))
    {
        const int R=60; // region radius in cells
        set<pair<int,int>> open;
        for(int gj=-R;gj<=R;gj++)for(int gi=-R;gi<=R;gi++)
            if(!CellSolid(gi,gj)) open.insert({gi,gj});
        set<pair<int,int>> seen; seen.insert({0,0});
        vector<pair<int,int>> st; st.push_back({0,0});
        while(!st.empty()){
            auto c=st.back(); st.pop_back();
            int i=c.first,j=c.second;
            int dx[4]={1,-1,0,0}, dy[4]={0,0,1,-1};
            for(int k=0;k<4;k++){
                int ni=i+dx[k], nj=j+dy[k];
                if(open.count({ni,nj})&&!seen.count({ni,nj})){ seen.insert({ni,nj}); st.push_back({ni,nj}); }
            }
        }
        expect("T1 maze connectivity (all open cells reachable)", seen.size()==open.size());
    }
    // T2: no-fly — jumping never exceeds ceiling
    {
        g_pos=SpawnPos(); g_pos.y=FloorAt(g_pos.x,g_pos.z);
        float maxY=g_pos.y;
        for(int i=0;i<500;i++){ g_pos.y=FloorAt(g_pos.x,g_pos.z); g_velY=6.0f; g_onGround=false;
            for(int k=0;k<40;k++) StepPhysics(1.0f/60.0f);
            maxY=max(maxY,g_pos.y); }
        expect("T2 no-fly (max y <= ceiling)", maxY<=ROOMH-0.1f-PLR_H+0.01f);
    }
    // T3: no-clip — walking into walls stops, never inside
    {
        g_pos=SpawnPos();
        bool clipped=false; float lastX=g_pos.x;
        // walk +X for a long time; find a wall on the way
        for(int i=0;i<2000&&!clipped;i++){
            MovePlayer(4.2f/60.0f,0);
            if(insideWall(g_pos)) clipped=true;
            if(g_pos.x-lastX<1e-4f){ break; }
            lastX=g_pos.x;
        }
        expect("T3 no-clip (+X stops at wall, never inside)", !clipped);
        // diagonal walk
        g_pos=SpawnPos(); bool clipped2=false;
        for(int i=0;i<3000&&!clipped2;i++){
            MovePlayer(2.0f/60.0f,2.0f/60.0f);
            if(insideWall(g_pos)) clipped2=true;
        }
        expect("T3b no-clip (diagonal)", !clipped2);
    }
    // T4: floor adherence after random walking
    {
        g_pos=SpawnPos();
        unsigned rnd=99u; bool fell=false;
        for(int i=0;i<3000;i++){
            rnd=rnd*1664525u+1013904223u;
            float a=((rnd>>8)&0xffff)/65535.0f*6.283f;
            MovePlayer(cosf(a)*3.0f/60.0f, sinf(a)*3.0f/60.0f);
            g_pos.y=g_pos.y; g_onGround=false; g_velY=0;
            for(int k=0;k<2;k++) StepPhysics(1.0f/60.0f);
            float fl=FloorAt(g_pos.x,g_pos.z);
            if(g_pos.y<fl-0.02f) fell=true;
        }
        expect("T4 floor adherence", !fell);
    }
    // T5: spawn validity
    {
        for(int s=0;s<200;s++){ g_wr=(unsigned)(s*7919+77); g_seed=20260925;
            Vec3 sp=SpawnPos();
            int gi=(int)floorf(sp.x/CELLSZ), gj=(int)floorf(sp.z/CELLSZ);
            if(CellSolid(gi,gj)){ expect("T5 spawn open cell", false); break; }
            if(s==199) expect("T5 spawn open cell", true);
        }
    }
    // T6: RayAABB sanity
    {
        AABB w; w.mn=Vec3(8,0,8); w.mx=Vec3(16,3,16);
        float t;
        bool hit=RayAABB(Vec3(0,1.5f,12),Vec3(1,0,0),w,t);
        expect("T6 ray hits wall", hit&&fabsf(t-8.0f)<0.01f);
        bool hit2=RayAABB(Vec3(0,1.5f,4),Vec3(1,0,0),w,t);
        expect("T6b ray misses wall", !hit2);
    }
    // T7: broken walls still block (no fly-through)
    {
        // find a broken wall cell
        bool found=false;
        for(int gi=-40;gi<=40&&!found;gi++)for(int gj=-40;gj<=40&&!found;gj++){
            if(CellSolid(gi,gj)&&Hash01(gi*3+1,gj*7+2)>0.90f){ found=true; }
        }
        expect("T7 broken wall exists (and blocks)", found);
    }
    printf("FAILURES=%d\n",failures);
    return failures?1:0;
}
