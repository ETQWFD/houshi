// ==================== audio (procedural WAV) ====================
static void GenSoundWav(const wchar_t* name,int type){
    wstring path=g_exeDir+L"\\"+name;
    FILE* f=_wfopen(path.c_str(),L"rb");
    if(f){ fclose(f); return; } // already exists
    f=_wfopen(path.c_str(),L"wb");
    if(!f) return;
    const int SR=22050, N= (type==0)?SR/6:(type==1)?SR/2:(type==2)?SR/3:(type==3)?SR/5:(type==4)?SR/4:(type==5)?SR/10:(type==6)?SR/20:(type==7)?SR/3:(type==8)?SR/12:SR/4;
    vector<short> pcm(N,0);
    unsigned rnd=12345u;
    for(int i=0;i<N;i++){
        float t=(float)i/SR; float s=0;
        if(type==0){ // carpet step: muffled thud
            float env=expf(-t*22.0f);
            s=(sinf(t*95.0f)*0.6f+sinf(t*180.0f)*0.3f)*env*(t>0.09f?-1.0f:1.0f);
        } else if(type==1){ // drink: water-ish bubbles
            float env=expf(-t*3.0f);
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            s=(sinf(t*420.0f)*0.3f+n*0.7f)*env*0.7f;
        } else if(type==2){ // eat: crunch
            float env=expf(-t*9.0f);
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            s=(n*0.9f+sinf(t*700.0f)*0.15f)*env;
        } else if(type==3){ // pickup: blip
            s=sinf(t*880.0f*(1.0f-t*2.0f))*expf(-t*8.0f)*0.7f;
        } else if(type==4){ // drop: thud
            s=sinf(t*130.0f)*expf(-t*15.0f)*0.8f;
        } else if(type==5){ // levelup: arpeggio
            float fr[4]={523.0f,659.0f,784.0f,1046.0f};
            int seg=(int)(t*8.0f); if(seg<4) s=sinf(2*PI*fr[seg]*t)*0.5f*expf(-t*2.0f);
        } else if(type==6){ // click
            s=sinf(t*1200.0f)*expf(-t*60.0f)*0.6f;
        } else if(type==7){ // gunshot: noise burst + crack
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            float env=t<0.01f?1.0f:expf(-(t-0.01f)*30.0f);
            s=(n*0.9f+sinf(t*1800.0f)*0.2f)*env;
        } else if(type==8){ // drywall hit
            float env=expf(-t*18.0f);
            rnd=rnd*1664525u+1013904223u; float n=(float)(rnd&0xffff)/65535.0f-0.5f;
            s=(n*0.6f+sinf(t*240.0f)*0.4f)*env;
        } else if(type==9){ // reload click-clack
            if(t<0.05f) s=sinf(t*1000.0f)*expf(-t*80.0f)*0.6f;
            else if(t>0.12f&&t<0.16f) s=sinf((t-0.12f)*1400.0f)*expf(-(t-0.12f)*90.0f)*0.7f;
        }
        float v=ClampF(s*32767.0f,-32767,32767); pcm[i]=(short)v;
    }
    unsigned char hdr[44]={0};
    memcpy(hdr,"RIFF",4); unsigned sz=(unsigned)(36+N*2);
    memcpy(hdr+4,&sz,4); memcpy(hdr+8,"WAVE",4); memcpy(hdr+12,"fmt ",4);
    unsigned fsz=16; memcpy(hdr+16,&fsz,4);
    unsigned short audiofmt=1,ch=1; unsigned rate=SR, byterate=SR*2, align=2, bits=16;
    memcpy(hdr+20,&audiofmt,2); memcpy(hdr+22,&ch,2); memcpy(hdr+24,&rate,4);
    memcpy(hdr+28,&byterate,4); memcpy(hdr+32,&align,2); memcpy(hdr+34,&bits,2);
    memcpy(hdr+36,"data",4); unsigned dsz=(unsigned)(N*2); memcpy(hdr+40,&dsz,4);
    fwrite(hdr,1,44,f); fwrite(&pcm[0],2,N,f); fclose(f);
}
static void GenSounds(){
    GenSoundWav(L"sfx_step.wav",0); GenSoundWav(L"sfx_drink.wav",1); GenSoundWav(L"sfx_eat.wav",2);
    GenSoundWav(L"sfx_pick.wav",3); GenSoundWav(L"sfx_drop.wav",4); GenSoundWav(L"sfx_level.wav",5);
    GenSoundWav(L"sfx_click.wav",6); GenSoundWav(L"sfx_gun.wav",7); GenSoundWav(L"sfx_wall.wav",8);
    GenSoundWav(L"sfx_reload.wav",9);
}
static void Sfx(const wchar_t* n){ PlaySoundW((g_exeDir+L"\\"+n).c_str(),NULL,SND_ASYNC|SND_FILENAME|SND_NODEFAULT); }

// ==================== networking (UDP LAN + STUN hole punch) ====================
static SOCKET g_sock=INVALID_SOCKET;
static sockaddr_in g_lanAddr;
static bool g_broadcastOn=true;
static float g_netTimer=0; static float g_stateTimer=0;
struct RemotePlayer{ char name[32]; Vec3 pos; float yaw,pitch; int cam,hp,sel; float last; };
static map<string,RemotePlayer> g_peers;
struct LanPeer{ sockaddr_in addr; char name[32]; float last; };
static vector<LanPeer> g_lanPeers;
static bool g_stunDone=false; static string g_pubIP="--"; static int g_pubPort=0;
static float g_stunTimer=0; static SOCKET g_stunSock=INVALID_SOCKET;
static bool g_connected=false;

static void NetInit(){
    WSADATA wd; WSAStartup(MAKEWORD(2,2),&wd);
    g_sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    u_long nb=1; ioctlsocket(g_sock,FIONBIO,&nb);
    BOOL br=TRUE; setsockopt(g_sock,SOL_SOCKET,SO_BROADCAST,(const char*)&br,sizeof(br));
    sockaddr_in b; memset(&b,0,sizeof(b)); b.sin_family=AF_INET; b.sin_addr.s_addr=htonl(INADDR_ANY); b.sin_port=0;
    bind(g_sock,(sockaddr*)&b,sizeof(b));
    memset(&g_lanAddr,0,sizeof(g_lanAddr)); g_lanAddr.sin_family=AF_INET;
    g_lanAddr.sin_addr.s_addr=htonl(INADDR_BROADCAST); g_lanAddr.sin_port=htons(47778);
    // STUN socket
    g_stunSock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    ioctlsocket(g_stunSock,FIONBIO,&nb);
}
static void NetSendTo(const sockaddr_in&addr,const char*data,int len){
    if(g_sock!=INVALID_SOCKET) sendto(g_sock,data,len,0,(sockaddr*)&addr,sizeof(addr));
}
static void StunProbe(){
    // async resolve once
    static bool resolving=false;
    if(!resolving&&!g_stunDone){ resolving=true;
        addrinfo hints; memset(&hints,0,sizeof(hints)); hints.ai_family=AF_INET; hints.ai_socktype=SOCK_DGRAM;
        addrinfo* res=0;
        if(getaddrinfo("stun.l.google.com","19302",&hints,&res)==0&&res){
            sockaddr_in sa; memcpy(&sa,res->ai_addr,res->ai_addrlen);
            char req[20]; memset(req,0,20);
            req[0]=0x00;req[1]=0x01; // binding request
            unsigned id=(unsigned)(GetTickCount()^0x5C87A);
            memcpy(req+4,&id,4); memcpy(req+8,&id,4); memcpy(req+12,&id,4);
            sendto(g_stunSock,req,20,0,(sockaddr*)&sa,sizeof(sa));
            g_stunTimer=2.0f;
            freeaddrinfo(res);
        }
        resolving=false;
    }
    // read response
    char buf[512]; sockaddr_in from; int fl=sizeof(from);
    int n=recvfrom(g_stunSock,buf,sizeof(buf),0,(sockaddr*)&from,&fl);
    if(n>=20){
        unsigned char* p=(unsigned char*)buf;
        unsigned msgtype=(p[0]<<8)|p[1];
        if(msgtype==0x0101){ // success response
            for(int off=20;off+4<=n;){
                unsigned atype=(p[off]<<8)|p[off+1]; unsigned alen=(p[off+2]<<8)|p[off+3];
                if(atype==0x0020&&alen>=8){
                    unsigned char fam=p[off+4+1];
                    if(fam==0x01){
                        unsigned xport=(p[off+4+2]<<8)|p[off+4+3];
                        unsigned xaddr=(p[off+4+4]<<24)|(p[off+4+5]<<16)|(p[off+4+6]<<8)|p[off+4+7];
                        unsigned port=xport^0x2112; unsigned addr=xaddr^0x2112A442;
                        char ipbuf[32]; sprintf(ipbuf,"%u.%u.%u.%u",(addr>>24)&255,(addr>>16)&255,(addr>>8)&255,addr&255);
                        g_pubIP=ipbuf; g_pubPort=(int)port; g_stunDone=true;
                    }
                }
                off+=4+alen;
            }
        }
    }
    if(g_stunTimer>0){ g_stunTimer-=g_dt; if(g_stunTimer<=0){ g_stunDone=true; } }
}
static void NetUpdate(float dt){
    StunProbe();
    if(g_sock==INVALID_SOCKET) return;
    g_netTimer-=dt;
    g_stateTimer-=dt;
    if(g_broadcastOn&&g_netTimer<=0){ g_netTimer=2.0f;
        char pkt[256]; sprintf(pkt,"HSB|%S",g_playerName.c_str());
        NetSendTo(g_lanAddr,pkt,(int)strlen(pkt));
    }
    // read packets
    char buf[2048]; sockaddr_in from; int fl=sizeof(from);
    int n=recvfrom(g_sock,buf,sizeof(buf),0,(sockaddr*)&from,&fl);
    while(n>0){
        buf[n]=0;
        if(strncmp(buf,"HSB|",4)==0){
            bool found=false;
            for(size_t i=0;i<g_lanPeers.size();i++){
                if(g_lanPeers[i].addr.sin_addr.s_addr==from.sin_addr.s_addr&&g_lanPeers[i].addr.sin_port==from.sin_port){
                    g_lanPeers[i].last=g_time; found=true; break; }
            }
            if(!found){ LanPeer lp; lp.addr=from; lp.last=g_time;
                strncpy(lp.name,buf+4,31); lp.name[31]=0;
                g_lanPeers.push_back(lp); }
        } else if(strncmp(buf,"P|",2)==0){ // state packet
            char name[32]; float px,py,pz,yaw,pitch; int cam,hp,sel;
            if(sscanf(buf+2,"%31[^|]|%f|%f|%f|%f|%f|%d|%d|%d",name,&px,&py,&pz,&yaw,&pitch,&cam,&hp,&sel)>=9){
                char keybuf[64]; sprintf(keybuf,"%s:%d",inet_ntoa(from.sin_addr),ntohs(from.sin_port));
                RemotePlayer&rp=g_peers[string(keybuf)];
                strncpy(rp.name,name,31); rp.pos=Vec3(px,py,pz); rp.yaw=yaw; rp.pitch=pitch;
                rp.cam=cam; rp.hp=hp; rp.sel=sel; rp.last=g_time;
                g_connected=true;
            }
        } else if(strncmp(buf,"C|",2)==0){ // chat
            char name[32],txt[512];
            if(sscanf(buf+2,"%31[^|]|%511[^\n]",name,txt)==2){
                g_chatLog.push_back(wstring(L"[")+wstring(name,name+strlen(name))+L"] "+wstring(txt,txt+strlen(txt)));
                if((int)g_chatLog.size()>80) g_chatLog.erase(g_chatLog.begin());
            }
        }
        n=recvfrom(g_sock,buf,sizeof(buf),0,(sockaddr*)&from,&fl);
    }
    // send state to connected peers (10 Hz)
    if(g_connected&&g_stateTimer<=0){
        g_stateTimer=0.1f;
        char pkt[256];
        sprintf(pkt,"P|%S|%.2f|%.2f|%.2f|%.2f|%.2f|%d|%d|%d",g_playerName.c_str(),
            g_pos.x,g_pos.y,g_pos.z,g_yaw,g_pitch,g_cam,(int)g_hp,g_hotbarSel);
        for(auto&kv:g_peers){
            const char* k=kv.first.c_str();
            const char* colon=strchr(k,':'); if(!colon) continue;
            char ip[64]; int len=(int)(colon-k); if(len>63) len=63;
            memcpy(ip,k,len); ip[len]=0;
            int port=atoi(colon+1); if(port<=0) continue;
            sockaddr_in sa; memset(&sa,0,sizeof(sa)); sa.sin_family=AF_INET;
            sa.sin_addr.s_addr=inet_addr(ip); sa.sin_port=htons((unsigned short)port);
            NetSendTo(sa,pkt,(int)strlen(pkt));
        }
    }
}
static void NetChat(const wstring&txt){
    if(txt.empty()) return;
    g_chatLog.push_back(L"你: "+txt);
    if((int)g_chatLog.size()>80) g_chatLog.erase(g_chatLog.begin());
    if(g_connected){
        char pkt[1024];
        sprintf(pkt,"C|%S|%S",g_playerName.c_str(),txt.c_str());
        for(auto&kv:g_peers){
            const char* k=kv.first.c_str();
            const char* colon=strchr(k,':'); if(!colon) continue;
            char ip[64]; int len=(int)(colon-k); if(len>63) len=63;
            memcpy(ip,k,len); ip[len]=0;
            int port=atoi(colon+1); if(port<=0) continue;
            sockaddr_in sa; memset(&sa,0,sizeof(sa)); sa.sin_family=AF_INET;
            sa.sin_addr.s_addr=inet_addr(ip); sa.sin_port=htons((unsigned short)port);
            NetSendTo(sa,pkt,(int)strlen(pkt));
        }
    }
}
static void NetJoin(const char* ip,int port){
    sockaddr_in sa; memset(&sa,0,sizeof(sa)); sa.sin_family=AF_INET;
    sa.sin_addr.s_addr=inet_addr(ip); sa.sin_port=htons((unsigned short)port);
    char pkt[64]; sprintf(pkt,"HSB|%S",g_playerName.c_str());
    for(int i=0;i<20;i++) NetSendTo(sa,pkt,(int)strlen(pkt)); // hole punch probes
}

// ==================== messages ====================
static void AddMsg(const wchar_t* s){ g_msgs.push_back(s); if((int)g_msgs.size()>6) g_msgs.erase(g_msgs.begin()); }
static void AddView(const wchar_t* s){ MsgT m; m.s=s; m.t=4.0f; g_views.push_back(m); if((int)g_views.size()>8) g_views.erase(g_views.begin()); }

// ==================== inventory & items ====================
static int HasItem(int id){ for(size_t i=0;i<g_inv.size();i++) if(g_inv[i].id==id&&g_inv[i].cnt>0) return (int)i; return -1; }
static bool AddItem(int id,int cnt){
    for(size_t i=0;i<g_inv.size();i++) if(g_inv[i].id==id&&g_inv[i].cnt<99){ g_inv[i].cnt+=cnt; return true; }
    for(size_t i=0;i<g_inv.size();i++) if(g_inv[i].id==0){ g_inv[i].id=id; g_inv[i].cnt=cnt; return true; }
    return false;
}
static void SpawnWorld(){
    g_items.clear();
    int nw=14, nm=10;
    for(int i=0;i<nw;i++){
        int gi=(int)(WRand()*40-20), gj=(int)(WRand()*40-20);
        if(CellSolid(gi,gj)) continue;
        WorldItem it; it.type=1;
        it.pos=Vec3(gi*CELLSZ+1.5f+WRand()*5.0f, FloorAt(gi*CELLSZ+4, gj*CELLSZ+4), gj*CELLSZ+1.5f+WRand()*5.0f);
        it.rot=WRand()*6.28f; it.taken=false; g_items.push_back(it);
    }
    for(int i=0;i<nm;i++){
        int gi=(int)(WRand()*40-20), gj=(int)(WRand()*40-20);
        if(CellSolid(gi,gj)) continue;
        WorldItem it; it.type=2;
        it.pos=Vec3(gi*CELLSZ+1.5f+WRand()*5.0f, FloorAt(gi*CELLSZ+4, gj*CELLSZ+4), gj*CELLSZ+1.5f+WRand()*5.0f);
        it.rot=WRand()*6.28f; it.taken=false; g_items.push_back(it);
    }
}
static Vec3 SpawnPos(){
    for(int r=0;r<40;r++){
        int gi=0, gj=0;
        if(r==0){ gi=0;gj=0; }
        else { gi=(int)(WRand()*24-12); gj=(int)(WRand()*24-12); }
        if(!CellSolid(gi,gj)) return Vec3(gi*CELLSZ+CELLSZ/2, FloorAt(gi*CELLSZ+CELLSZ/2,gj*CELLSZ+CELLSZ/2), gj*CELLSZ+CELLSZ/2);
    }
    return Vec3(0,0,0);
}
static void GiveAKM(){
    if(AddItem(5,1)){ AddMsg(L"获得 AKM 步枪"); Sfx(L"sfx_pick.wav"); }
    else AddMsg(L"背包已满，无法获得 AKM");
}
static void UseItem(int idx){
    if(idx<0||idx>=(int)g_inv.size()) return;
    int id=g_inv[idx].id;
    if(id==1){ // 银杏水(杏仁水)
        g_thr=ClampF(g_thr+50,0,100); g_san=ClampF(g_san+20,0,100);
        g_hp=ClampF(g_hp+2,0,100); g_hun=ClampF(g_hun+1,0,100);
        g_drinkAnim=1.0f; Sfx(L"sfx_drink.wav"); AddMsg(L"饮下银杏水：口渴+50% 理智+20 生命+2");
        g_inv[idx].cnt--; if(g_inv[idx].cnt<=0){ g_inv[idx].id=0; g_inv[idx].cnt=0; }
    } else if(id==2){ // 肉块
        g_hun=ClampF(g_hun+10,0,100); g_hp=ClampF(g_hp+4,0,100);
        g_san=ClampF(g_san-1,0,100); g_xp=ClampF(g_xp+1,0,100);
        g_eatAnim=1.0f; Sfx(L"sfx_eat.wav"); AddMsg(L"吃掉腐肉：饥饿+10% 生命+4 理智-1 经验+1");
        g_inv[idx].cnt--; if(g_inv[idx].cnt<=0){ g_inv[idx].id=0; g_inv[idx].cnt=0; }
        if(g_xp>=100){ g_xp-=100; g_level++; g_capacity=39+g_level; Sfx(L"sfx_level.wav");
            AddView(L"升级！背包扩容至 40+"+to_wstring(g_level-1)+L" 格"); }
    } else if(id==5){ AddMsg(L"AKM：左键开火  R 换弹"); }
    else if(id==4){ AddMsg(L"《生存手册》：后室求生要点——寻找杏仁水恢复理智，警惕腐肉，别回头。"); }
}
static void TryPickup(){
    Vec3 eye=g_pos+Vec3(0,1.5f,0);
    for(size_t i=0;i<g_items.size();i++){
        WorldItem&it=g_items[i];
        if(it.taken) continue;
        float dx=it.pos.x-eye.x, dy=it.pos.y-eye.y, dz=it.pos.z-eye.z;
        if(dx*dx+dy*dy+dz*dz<3.2f){
            if(AddItem(it.type,1)){
                it.taken=true;
                Sfx(L"sfx_pick.wav");
                AddMsg(it.type==1?L"拾取 银杏水(杏仁水)":L"拾取 腐肉块");
                return;
            } else { AddMsg(L"背包已满！"); return; }
        }
    }
    AddMsg(L"附近没有可拾取的物品");
}
static void DropToWorld(int id,int cnt){
    Vec3 f(-sinf(g_yaw)*cosf(g_pitch),sinf(g_pitch),-cosf(g_yaw)*cosf(g_pitch));
    WorldItem it; it.type=id; it.pos=g_pos+Vec3(0,1.0f,0)+f*1.2f; it.rot=g_time; it.taken=false;
    g_items.push_back(it);
    Sfx(L"sfx_drop.wav");
    (void)cnt;
}
