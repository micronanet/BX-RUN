#pragma once
// A small, reusable band replaces a second 128 KB frame buffer.
// Each VGA pixel is submitted once, already containing the background and objects.
const int BAND_H=8,MAX_RASTER_OPS=256,TEXT_POOL_SIZE=2048;
struct RasterOp {int16_t x,y,w,h;uint8_t kind,colour;const Bitmap*bitmap;};
RasterOp rasterOps[MAX_RASTER_OPS];char textPool[TEXT_POOL_SIZE];int rasterCount=0,textUsed=0;bool rasterActive=false,rasterRoad=false,rasterOverflow=false,sceneDrawing=false;bool liteMode=false;int measuredFPS=0;
uint8_t bandPixels[640*BAND_H];Bitmap bandBitmap(640,BAND_H,bandPixels,PixelFormat::RGBA2222);
void rasterRect(int x,int y,int w,int h,uint8_t c){if(rasterCount>=MAX_RASTER_OPS){rasterOverflow=true;return;}rasterOps[rasterCount++]={(int16_t)x,(int16_t)y,(int16_t)w,(int16_t)h,0,c,nullptr};}
void rasterBitmap(int x,int y,const Bitmap*b){if(rasterCount>=MAX_RASTER_OPS){rasterOverflow=true;return;}rasterOps[rasterCount++]={(int16_t)x,(int16_t)y,(int16_t)b->width,(int16_t)b->height,uint8_t(sceneDrawing?3:1),0,b};}
void rasterText(int x,int y,const char*s,uint8_t c){int n=int(strlen(s));if(rasterCount>=MAX_RASTER_OPS||textUsed+n>TEXT_POOL_SIZE){rasterOverflow=true;return;}memcpy(textPool+textUsed,s,n);rasterOps[rasterCount++]={(int16_t)x,(int16_t)y,(int16_t)n,(int16_t)textUsed,uint8_t(sceneDrawing?4:2),c,nullptr};textUsed+=n;}
void beginRaster(){rasterCount=textUsed=0;rasterActive=true;rasterRoad=rasterOverflow=sceneDrawing=false;}
void span(uint8_t*row,int x1,int x2,uint8_t colour){x1=std::max(0,x1);x2=std::min(639,x2);if(x1<=x2)memset(row+x1,colour,x2-x1+1);}
void sceneSpan(uint8_t*row,int x1,int x2,uint8_t colour){span(row,std::max(viewLeft(),x1),std::min(viewRight(),x2),colour);}
void skyRow(int stage,int y,uint8_t*row,int left,int right){int x=0;for(int i=skyRows[stage][y];i<skyRows[stage][y+1];++i){uint16_t word=skyStreams[stage][i];int len=(word>>6)+1;int a=std::max(left,x),b=std::min(right,x+len-1);if(a<=b)memset(row+a,uint8_t(192|(word&63)),b-a+1);x+=len;if(x>right)break;}}
void backgroundRow(int y,uint8_t*row){memset(row,192,640);if(!rasterRoad||y<28||y>191)return;int stage=currentStage(),leftEdge=viewLeft(),width=viewRight()-leftEdge+1;
 if(y<horizon){skyRow(stage,y,row,leftEdge,viewRight());return;}
 RoadRow&r=roadRows[y];int left=int(r.x-r.w),right=int(r.x+r.w),band=int((race.z+r.dist)/40)&1;
 const uint8_t shades[STAGES][2]={{200,201},{196,216},{219,203},{196,212},{213,208},{198,217}};memset(row+leftEdge,shades[stage][band],width);
 int shore=left-int(r.w*.65f);if((stage==0||stage==5)&&shore>leftEdge){sceneSpan(row,leftEdge,shore,stage==5?((int(race.z+r.dist)/85)&1?225:230):((int(race.z+r.dist)/85)&1?244:248));sceneSpan(row,shore,shore+std::max(2,int(r.w*.06f)),239);if((y+int(race.z/8))%9==0)sceneSpan(row,leftEdge,shore-4,stage==5?251:253);}
 if(y%9==0){sceneSpan(row,right+int(r.w*.3f),right+int(r.w*.3f)+4,stage==2?198:196);}
 int kerb=std::max(2,int(r.w*.065f));sceneSpan(row,left-kerb,left,band?255:195);sceneSpan(row,right,right+kerb,band?255:195);sceneSpan(row,left+1,right-1,192);
 if((int((race.z+r.dist)/26)&1)==0){int width=std::max(1,int(r.w*.013f));int a=int(r.x-r.w/3),b=int(r.x+r.w/3);sceneSpan(row,a-width,a+width,255);sceneSpan(row,b-width,b+width,255);}
}
void paintRasterBand(int top){for(int row=0;row<BAND_H;++row)backgroundRow(top+row,bandPixels+row*640);
 for(int n=0;n<rasterCount;++n){const RasterOp&o=rasterOps[n];int height=(o.kind==2||o.kind==4)?8:o.h;int y0=std::max(top,int(o.y)),y1=std::min(top+BAND_H,int(o.y)+height);if(y0>=y1)continue;
  if(o.kind==0){for(int y=y0;y<y1;++y)span(bandPixels+(y-top)*640,o.x,o.x+o.w-1,o.colour);}
  else if(o.kind==1||o.kind==3){int x0=std::max(o.kind==3?viewLeft():0,int(o.x)),x1=std::min(o.kind==3?viewRight()+1:640,int(o.x)+o.w);if(o.kind==3){y0=std::max(28,y0);y1=std::min(192,y1);}if(x0>=x1)continue;for(int y=y0;y<y1;++y){const uint8_t*src=o.bitmap->data+(y-o.y)*o.w+(x0-o.x);uint8_t*dst=bandPixels+(y-top)*640+x0;
    // All opaque artwork can be copied without a per-pixel alpha test.
    if(o.bitmap==&bmpTitle[0])memcpy(dst,src,x1-x0);else for(int x=x0;x<x1;++x){uint8_t p=*src++;if(p)*dst=p;++dst;}}}
  else{for(int y=y0;y<y1;++y){uint8_t*dst=bandPixels+(y-top)*640;for(int ch=0;ch<o.w;++ch){unsigned char c=textPool[o.h+ch];if(c<32||c>127)c='?';uint8_t bits=hudFont[c-32][y-o.y];int x=o.x+ch*8;for(int b=0;b<8;++b)if((bits&(128>>b))&&x+b>=(o.kind==4?viewLeft():0)&&x+b<=(o.kind==4?viewRight():639))dst[x+b]=o.colour;}}}
 }}
// The native ESP32 can bypass the Canvas command queue completely.
// FabGL documents the scanline order as pixel x -> byte (x XOR 2).
#if (defined(__XTENSA__) && defined(ARDUINO_ARCH_ESP32)) || defined(BXRUN_RAW_TEST)
#define BXRUN_NATIVE_RASTER 1
uint8_t nativePalette[64];bool paletteReady=false;
void copyNativeBand(int top){if(!paletteReady){for(int c=0;c<64;++c)nativePalette[c]=DisplayController.createRawPixel(RGB222(c&3,(c>>2)&3,(c>>4)&3));paletteReady=true;}
 for(int y=0;y<BAND_H;++y){volatile uint32_t*dst=reinterpret_cast<volatile uint32_t*>(DisplayController.getScanline(top+y));const uint8_t*src=bandPixels+y*640;
  int left=(!refreshMargins&&!mapDirty&&rasterRoad&&!wideView&&top+y>=28&&top+y<136)?64:0;int right=left?576:640;dst+=left/4;for(int x=left;x<right;x+=4){uint32_t p0=nativePalette[src[x]&63],p1=nativePalette[src[x+1]&63],p2=nativePalette[src[x+2]&63],p3=nativePalette[src[x+3]&63];*dst++=p2|(p3<<8)|(p0<<16)|(p1<<24);}}}
#endif
void endRaster(){rasterActive=false;canvas.waitCompletion(false);canvas.beginUpdate();for(int y=0;y<200;y+=BAND_H){paintRasterBand(y);
#ifdef BXRUN_NATIVE_RASTER
 copyNativeBand(y);
#else
 canvas.drawBitmap(0,y,&bandBitmap);canvas.waitCompletion(false);
#endif
 }canvas.endUpdate();refreshMargins=mapDirty=false;}
void blit(int x,int y,Bitmap*b){if(rasterActive)rasterBitmap(x,y,b);else canvas.drawBitmap(x,y,b);}
