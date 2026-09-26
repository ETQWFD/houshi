# -*- coding: utf-8 -*-
# Build a self-contained bitmap font atlas (1024x1024, 16px cells) from font_sub.ttf.
# Outputs: font_atlas.raw (1 byte/pixel alpha), plus a C wchar table of the chars in atlas order.
from PIL import Image, ImageDraw, ImageFont
import sys, os

CELL=16; COLS=64; SIZE=COLS*CELL  # 1024
MAX_CELLS=COLS*COLS               # 4096

# char set: ASCII + GB2312 level-1 hanzi + common CJK punctuation
chars=set(chr(c) for c in range(0x20,0x7F))
punct="，。、；：？！“”‘’（）《》【】·—…％×＋－＝／＼～　↑↓▌_|"
chars.update(punct)
for b1 in range(0xB0,0xD8):       # GB2312 level-1 hanzi
    for b2 in range(0xA1,0xFF):
        try:
            ch=bytes([b1,b2]).decode('gb2312')
            chars.add(ch)
        except Exception:
            pass
# extra common level-2 that appear in our UI (safety)
extra="盒 瓶 肉 枪 弹 药 索 引 层 房 间 洞 掩 蔽 所 仓 库 堡 垒"
chars.update(extra.split())
chars=sorted(chars)
if len(chars)>MAX_CELLS:
    print("TOO MANY CHARS", len(chars)); sys.exit(1)

font=ImageFont.truetype("font_sub.ttf", CELL)
img=Image.new("L",(SIZE,SIZE),0)
dr=ImageDraw.Draw(img)
for i,ch in enumerate(chars):
    c=i%COLS; r=i//COLS
    x=c*CELL; y=r*CELL
    dr.text((x+1,y+1),ch,font=font,fill=255)
img.save("font_atlas.png")
raw=img.tobytes()
open("font_atlas.raw","wb").write(raw)

# C wchar table (escape special ascii)
def esc(ch):
    o=ord(ch)
    if o==0x5C: return "\\\\"
    if o==0x22: return "\\\""
    if o==0x0A: return "\\n"
    if o==0x0D: return "\\r"
    if 0x20<=o<0x7F: return ch
    return ch
table="".join(esc(ch) for ch in chars)
code='static const wchar_t g_fontChars[]=L"'+table+'";'
open("font_chars.h","w",encoding="utf-8").write(code+"\n")
print("chars:",len(chars),"atlas:",SIZE,"x",SIZE,"raw bytes:",len(raw))
