#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""生成《后世》游戏图标 icon.ico（PNG 压缩 ICO，含 16/32/48/256 四档）"""
import struct, zlib, math, os

def make_png(w, h, rgba):
    def chunk(t, d):
        c = struct.pack('>I', len(d)) + t + d
        c += struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
        return c
    ihdr = struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0)
    raw = b''.join(b'\x00' + bytes(rgba[y*w*4:(y+1)*w*4]) for y in range(h))
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', ihdr)
            + chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b''))

def draw(size):
    w = h = size
    c = size / 256.0
    px = bytearray(w*h*4)
    def setp(x, y, r, g, b, a=255):
        if 0 <= x < w and 0 <= y < h:
            i = (y*w + x)*4
            px[i], px[i+1], px[i+2], px[i+3] = r, g, b, a
    def blend(x, y, r, g, b, a):
        if 0 <= x < w and 0 <= y < h:
            i = (y*w + x)*4
            na = a/255.0
            px[i]   = int(px[i]*(1-na) + r*na)
            px[i+1] = int(px[i+1]*(1-na) + g*na)
            px[i+2] = int(px[i+2]*(1-na) + b*na)
            if px[i+3] < 255:
                px[i+3] = min(255, px[i+3] + a)
    cx = cy = 128*c
    R = 118*c
    # 外圈光晕
    for y in range(h):
        for x in range(w):
            d = math.hypot(x-cx, y-cy)
            if d <= R*1.06:
                t = max(0.0, 1.0 - d/(R*1.06))
                blend(x, y, 90, 100, 160, int(t*36))
    # 主体暗夜球
    for y in range(h):
        for x in range(w):
            d = math.hypot(x-cx, y-cy)
            if d <= R:
                t = d/R
                r = int(14 + 34*t); g = int(18 + 36*t); b = int(30 + 48*t)
                setp(x, y, r, g, b, 255)
    # 地表剪影（底部起伏）
    ground = 168*c
    for y in range(h):
        for x in range(w):
            if y >= ground:
                d = math.hypot(x-cx, y-cy)
                if d <= R:
                    hgt = math.sin(x*0.03)*6*c + 8*c
                    if y - ground <= hgt:
                        setp(x, y, 22, 22, 30, 255)
    # 月光（右上新月）
    mx, my = 96*c, 84*c
    mr = 30*c
    for y in range(h):
        for x in range(w):
            d1 = math.hypot(x-mx, y-my)
            d2 = math.hypot(x-mx-10*c, y-my-6*c)
            if d1 <= mr and d2 > mr*0.86:
                blend(x, y, 250, 240, 200, 255)
    # 手电筒光束（从左下指向右上）
    bx, by = 52*c, 196*c
    for y in range(h):
        for x in range(w):
            vx, vy = x-bx, y-by
            d = math.hypot(vx, vy)
            if d < 1: continue
            # 光束方向指向 (150c, 80c)
            tx, ty = 150*c-bx, 80*c-by
            tl = math.hypot(tx, ty)
            nd = (vx*tx + vy*ty)/d/tl
            if nd > 0.985 and d < 190*c:
                fade = max(0.0, 1.0 - d/(190*c))
                edge = (nd-0.985)/0.015
                blend(x, y, 255, 240, 180, int(120*fade*edge))
    # 火柴人幸存者剪影
    sx, sy = 150*c, ground-2
    for y in range(h):
        for x in range(w):
            dx, dy = x-sx, y-sy
            if abs(dx) < 7*c and dy < 0 and dy > -46*c: setp(x, y, 10, 10, 14, 255)
            if abs(dx) < 7*c and abs(dy) < 7*c: setp(x, y, 10, 10, 14, 255)
    return bytes(px)

sizes = [16, 32, 48, 256]
pngs = []
for s in sizes:
    pngs.append((s, make_png(s, s, draw(s))))
header = struct.pack('<HHH', 0, 1, len(pngs))
offset = 6 + 16*len(pngs)
entries = b''
data = b''
for s, p in pngs:
    entries += struct.pack('<BBBBHHII', s if s < 256 else 0, s if s < 256 else 0,
                           0, 0, 1, 32, len(p), offset)
    offset += len(p)
    data += p
out = header + entries + data
with open('icon.ico', 'wb') as f:
    f.write(out)
print('icon.ico written:', len(out), 'bytes')
