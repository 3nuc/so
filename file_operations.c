#define MODULE
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
// z instrukcji: krok 3.3
#include <linux/tty.h>
#include <linux/vt.h>
#include <linux/mm.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/ioctl.h>
// z instrukcji: krok 3.1
// jak bedzie zaincludowane przed tty.h i vt.h to gcc wybuchnie
#include <char/console_struct.h>


#define IOCTL_DOT_DURATION _IOW(MORSE_MAJOR, 0, int)
#define currcons fg_console
#define MORSE_MAJOR 60
#define MAX_DEVICES 8

struct morse_device {
  int minor;
  int dot_duration;
};

struct morse_device devices[MAX_DEVICES];

char *characters[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M","N", "O", "P", "Q", "R", "S", "T", "U",
                      "V", "W", "X", "Y", "Z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", " "};


char *morsecode[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---", "-.-",".-..","--","-.","---",".--.","--.-",
                     ".-.","...","-","..-", "...-",".--","-..-","-.--","--..", ".----","..---","...--","....-", ".....", "-....",
                     "--...","---..","----.","-----", " "};

char* char_to_morse(char* character) {
	int i;
  for(i = 0; i<36; i++) {
		if(strcmp(characters[i],character)==0) break;	
	}
	return morsecode[i];
}

void set_morse_signal() {
  //can't just do *origin=0x3C46 for some reason
  unsigned short* _origin =(unsigned short*) origin;
  *_origin=0xC46; //F char with teal background
}

void set_morse_nosignal() {
  unsigned short* _origin =(unsigned short*) origin;
  *_origin=0x0020; //space char with black background
}
void set_morse_dot(int dot_duration) {
	set_morse_signal();
	current->state=TASK_INTERRUPTIBLE;
	current->timeout=jiffies+dot_duration*HZ/1000;
	schedule();
	set_morse_nosignal();
}
void set_morse_dash(int dot_duration) {
	set_morse_signal();
	current->state=TASK_INTERRUPTIBLE;
	current->timeout=jiffies+3*dot_duration*HZ/1000;
	schedule();
	set_morse_nosignal();
}

static void morse_release(struct inode *inode, struct file *file) {
  int minor = MINOR(inode->i_rdev)-1;
  devices[minor].dot_duration = 500;
  devices[minor].minor = minor;
  return;
}

static int morse_write(struct inode *inode, struct file *file, const char *buffer, int count) {
  int minor = MINOR(inode->i_rdev)-1;
  int dot_duration = devices[minor].dot_duration;
	int i;
  printk("\n morse_write: for minor %d dot_duration is %d\n",minor+1, dot_duration);
	for(i = 0; i < count-1; i++) {
		char ubyte = get_user(buffer+i);
		char ubyte_string[2] = {ubyte, 0};
		char ubyte_morse[10] = {'0', 0, 0, 0, 0, 0, 0, 0, 0, 0};
		int beep_count;
		int j;
		if(!((ubyte >= 48 && ubyte <= 57) || (ubyte >= 65 && ubyte <= 90) || (ubyte >= 97 && ubyte <= 122) || (ubyte == 32))) {
		  printk("character is not a digit, letter or space -- ignoring\n");
			continue;
		}
		if(ubyte >=97 && ubyte <=122) {
			//morse doesn't care about letter casing, conver to uppercase
			printk("%c", ubyte);
		  ubyte=ubyte-32;
			printk("%c", ubyte);
		}		
    ubyte_string[0] = ubyte;
		strcpy(ubyte_morse, char_to_morse(ubyte_string));
		beep_count = strlen(ubyte_morse);
		printk("raw_char: %c, morsified: %s, morsified_len: %d\n", ubyte, ubyte_morse, beep_count);
		for(j=0; j<beep_count; j++) {
			char current_dash_or_dot = ubyte_morse[j];
		  printk(" \t sign: %c\n", current_dash_or_dot);
			if(current_dash_or_dot=='-') {
				set_morse_dash(dot_duration); 
		  }
			else if(current_dash_or_dot=='.') {
				set_morse_dot(dot_duration);
			}
      else if(current_dash_or_dot==' ') {
        printk("  (space, waiting 3 dot durations)\n");
        current->state=TASK_INTERRUPTIBLE;
        current->timeout=jiffies+2*dot_duration*HZ/1000;
        schedule();
      }
      //break between signals
      current->state=TASK_INTERRUPTIBLE;
      current->timeout=jiffies+dot_duration*HZ/1000;
      schedule();
		}
	}
  return count;
}

static int morse_open(struct inode* inode, struct file* file) {
  int minor = MINOR(inode->i_rdev)-1;
  if(minor >= MAX_DEVICES) return -1;
  devices[minor].minor = minor;
  return 0;
}

static int morse_ioctl(struct inode* inode, struct file* file, unsigned int cmd, unsigned long arg) {
  int minor = MINOR(inode->i_rdev)-1;
	switch(cmd) {
	 case IOCTL_DOT_DURATION:
     if(arg < 0) {
        printk("Dlugosc musi byc wieksza niz 0"); 
        return -1;
     }
     printk("\nmorse_ioctl (driver): setting dot_duration to %d\n", (int)arg);
     devices[minor].dot_duration=(int)arg;
     printk("\nmorse_ioctl (driver): dot_duration is now %d for minor: %d\n", devices[minor].dot_duration, minor+1);
	   break;
	 default:
	   printk("ioctl: unknown command");
		 break;
	 }
	return 0;
}


struct file_operations morse_fops = {
	0,
  0,
  morse_write,
	0,
	0,
  morse_ioctl,
	0,
  morse_open,
  morse_release
};


int init_module(void) {
  int i;
  struct morse_device _default = {
    -1,
    500
  };
  for(i=0;i<MAX_DEVICES;i++) {
    devices[i] = _default; 
  }
  printk("MORSE initialized\n");
  register_chrdev(MORSE_MAJOR, "morse_fops", &morse_fops);
  return 0;
}
void cleanup_module(void) {
  printk("BYE\n");
	unregister_chrdev(MORSE_MAJOR, "morse_fops");
}
