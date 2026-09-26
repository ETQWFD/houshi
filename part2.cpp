// ==================== canvas helpers (fill methods) ====================
inline void CanvasFillRect(Canvas&c,int x0,int y0,int x1,int y1,unsigned char r,unsigned char g,unsigned char b,unsigned char a=255){
    if(x0<0)x0=0;if(y0<0)y0=0;if(x1>c.w)x1=c.w;if(y1>c.h)y1=c.h;
    for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++) c.Set(x,y,r,g,b,a);
}
inline void CanvasFillCircle(Canvas&c,int cx,int cy,int rr,unsigned char r,unsigned char g,unsigned char b){
    for(int dy=-rr;dy<=rr;dy++)for(int dx=-rr;dx<=rr;dx++)
        if(dx*dx+dy*dy<=rr*rr) c.Set(cx+dx,cy+dy,r,g,b);
}
inline void CanvasLine(Canvas&c,int x0,int y0,int x1,int y1,int th,unsigned char r,unsigned char g,unsigned char b){
    int dx=abs(x1-x0),dy=abs(y1-y0),sx=x0<x1?1:-1,sy=y0<y1?1:-1,err=dx-dy;
    for(;;){ for(int t=-th/2;t<=th/2;t++){ c.Set(x0,y0+t,r,g,b); c.Set(x0+t,y0,r,g,b); }
        if(x0==x1&&y0==y1)break; int e2=2*err;
        if(e2>-dy){err-=dy;x0+=sx;} if(e2<dx){err+=dx;y0+=sy;} }
}



// ==================== chunks: infinite backrooms ====================
struct GeoBuild{ vector<float> v; vector<unsigned int> idx; };
static void GeoQuad(GeoBuild&g,Vec3 a,Vec3 b,Vec3 c,Vec3 d,Vec3 n,float u0,float v0,float u1,float v1){
    unsigned int base=(unsigned int)(g.v.size()/8);
    Vec3 p[4]={a,b,c,d};
    for(int i=0;i<4;i++){
        g.v.push_back(p[i].x);g.v.push_back(p[i].y);g.v.push_back(p[i].z);
        g.v.push_back(n.x);g.v.push_back(n.y);g.v.push_back(n.z);
        float u=(i==1||i==2)?u1:u0, v=(i==2||i==3)?v1:v0;
        g.v.push_back(u);g.v.push_back(v);
    }
    g.idx.push_back(base);g.idx.push_back(base+1);g.idx.push_back(base+2);
    g.idx.push_back(base);g.idx.push_back(base+2);g.idx.push_back(base+3);
}
// solid cell test (infinite maze): corridors every 3rd row/col guarantee connectivity
static bool CellSolid(int i,int j){
    if(j%3==0||i%3==0) return false;      // corridors always open
    if(Hash01(i,j)>0.80f) return false;   // occasional rooms
    return true;
}
static float CellFloor(int i,int j){ return Hash01(i,j)*0.12f; } // slight damp variation
static bool CellFlicker(int i,int j){ return Hash01(i+777,j+555)>0.80f; }
struct Chunk{
    int cx,cz; bool gen;
    vector<AABB> walls;
    vector<Vec3> panels;   // light sources (ceiling centers)
    Mesh mesh;
    Chunk():cx(0),cz(0),gen(false){}
};
static map<int,Chunk> g_chunks;
static int ChunkKey(int cx,int cz){ return cx*100003+cz; }
static int g_lhx=0,g_lhz=0; // last player chunk for respawn anchor
static void BuildChunk(int cx,int cz){
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
}
static void UpdateChunks(){
    int pcx=(int)floorf(g_pos.x/CHUNKSZ), pcz=(int)floorf(g_pos.z/CHUNKSZ);
    const int R=3;
    vector<int> need;
    for(int dz=-R;dz<=R;dz++)for(int dx=-R;dx<=R;dx++){
        int key=ChunkKey(pcx+dx,pcz+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()||!it->second.gen) BuildChunk(pcx+dx,pcz+dz);
    }
    for(auto it=g_chunks.begin();it!=g_chunks.end();){
        if(abs(it->second.cx-pcx)>R+2||abs(it->second.cz-pcz)>R+2){
            if(it->second.mesh.vao){ glDeleteVertexArrays(1,&it->second.mesh.vao); glDeleteBuffers(2,&it->second.mesh.vbo); }
            it=g_chunks.erase(it);
        } else ++it;
    }
    g_lhx=pcx; g_lhz=pcz;
}
static float FloorAt(float x,float z){
    int gi=(int)floorf(x/CELLSZ), gj=(int)floorf(z/CELLSZ);
    return CellFloor(gi,gj);
}
static void CollectNearWalls(const Vec3&p,vector<AABB>&out,float margin){
    out.clear();
    int pcx=(int)floorf(p.x/CHUNKSZ), pcz=(int)floorf(p.z/CHUNKSZ);
    for(int dz=-1;dz<=1;dz++)for(int dx=-1;dx<=1;dx++){
        int key=ChunkKey(pcx+dx,pcz+dz);
        auto it=g_chunks.find(key);
        if(it==g_chunks.end()||!it->second.gen) continue;
        const Chunk&ch=it->second;
        for(size_t i=0;i<ch.walls.size();i++){
            const AABB&w=ch.walls[i];
            if(p.x+margin>w.mn.x&&p.x-margin<w.mx.x&&p.z+margin>w.mn.z&&p.z-margin<w.mx.z)
                out.push_back(w);
        }
    }
}
// player physics (AABB capsule, axis separated, gravity, no flying)
static float PLR_HALF=0.35f, PLR_H=1.75f;
static void MovePlayer(float dx,float dz){
    vector<AABB> ws;
    float y0=g_pos.y;
    CollectNearWalls(g_pos,ws,PLR_HALF+0.1f);
    // X axis
    g_pos.x+=dx;
    for(size_t i=0;i<ws.size();i++){
        const AABB&w=ws[i];
        if(g_pos.x+PLR_HALF>w.mn.x&&g_pos.x-PLR_HALF<w.mx.x&&y0<w.mx.y&&y0+PLR_H>w.mn.y){
            if(g_pos.z+PLR_HALF>w.mn.z&&g_pos.z-PLR_HALF<w.mx.z){
                if(dx>0) g_pos.x=w.mn.x-PLR_HALF; else if(dx<0) g_pos.x=w.mx.x+PLR_HALF;
            }
        }
    }
    // Z axis
    g_pos.z+=dz;
    CollectNearWalls(g_pos,ws,PLR_HALF+0.1f);
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
    // ceiling clamp
    if(g_pos.y+PLR_H>ROOMH-0.1f){ g_pos.y=ROOMH-0.1f-PLR_H; if(g_velY>0) g_velY=0; }
    if(!g_onGround){
        g_velY-=19.5f*dt; g_pos.y+=g_velY*dt;
        if(g_pos.y<=fl){ g_pos.y=fl; g_velY=0; g_onGround=true; }
    }
}
// ray vs AABB (for AKM hit)
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
