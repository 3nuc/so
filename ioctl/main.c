#include <linux/ioctl.h>

#define IOCTL_DOT_DURATION _IOW(MORSE_MAJOR, 0, int)
#define currcons fg_console
#define MORSE_MAJOR 60
#define MAX_DEVICES 8

int main(int argc, char* argv[]) {
  int dot_duration;
  char* device_name;
  int fd; 
  printf("\nhello, argc = %d, %s, %s, %s\n", argc, argv[0], argv[1], argv[2]);
  if(argc < 3) {
    printf("\npass at least 2 arg\n (first is eg. /dev/morse and 2nd is dot_duration\n");
    return -1;
  }
  device_name=argv[1];
  fd=open(device_name, 0);
  dot_duration=atoi(argv[2]);
  if(!fd) {
    printf("can't open m1");
    return -1;
  }
  ioctl(fd, IOCTL_DOT_DURATION, dot_duration);
  printf("\n%s set to duration: %d\n", device_name, dot_duration);
  close(fd);
}
