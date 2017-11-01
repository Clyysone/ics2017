#include <am.h>
#include <x86.h>


#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0x64

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
	unsigned long now_time = inl(RTC_PORT);
	return now_time-boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
 	int a=0,b=0,i=0;

	for(b=y;b<(y+h);b++)
    for(a=x;a<(x+w);a++,i++)
		{
			fb[a+b*400]= pixels[i];
		}

}

void _draw_sync() {
}

int _read_key() {
  int ret = _KEY_NONE;
  if(inb(I8042_STATUS_PORT)){
  	ret = inl(I8042_DATA_PORT);
  }
  return ret;
}
