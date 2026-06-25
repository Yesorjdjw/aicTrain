/*
 *  stENG_drv.c — dual servo PWM driver (S5P6818)
 *  PWM2 (GPIOC14): left/right servo
 *  PWM3 (GPIOD0):  up/down servo
 *
 *  Device node: /dev/sm_stENG
 *  ioctl(fd, duty, PWM2_CTL) — set PWM2 duty cycle
 *  ioctl(fd, duty, PWM3_CTL) — set PWM3 duty cycle
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/device.h>

/* ---- PWM register map ---- */
typedef struct {
	unsigned int TCFG0;
	unsigned int TCFG1;
	unsigned int TCON;
	unsigned int TCNTB0;
	unsigned int TCMPB0;
	unsigned int TCNTO0;
	unsigned int TCNTB1;
	unsigned int TCMPB1;
	unsigned int TCNTO1;
	unsigned int TCNTB2;
	unsigned int TCMPB2;
	unsigned int TCNTO2;
	unsigned int TCNTB3;
	unsigned int TCMPB3;
	unsigned int TCNTO3;
	unsigned int TCNTB4;
	unsigned int TCNTO4;
	unsigned int TINT_CSTAT;
} pwm;

/* ---- hardware addresses ---- */
#define GPIOD_ADDR      0xc001d000
#define GPIOD_ALT_ADDR  0xc001d020
#define GPIOC_ADDR      0xc001c000
#define GPIOC_ALT_ADDR  0xc001c020
#define PWM_ADDR        0xc0018000

/* ---- ioctl channel selectors ---- */
#define PWM2_CTL  0x2
#define PWM3_CTL  0x3

/* ---- PWM timing: prescaler=250, mux=1/16, period=644 ticks ---- */
#define PRESCALER  249
#define MUX_DIV    0x4
#define TCNT_VAL   644

static volatile unsigned int *gpiod_base;
static volatile unsigned int *gpiod_alt;
static volatile unsigned int *gpioc_base;
static volatile unsigned int *gpioc_alt;
static volatile pwm *pwm_base;

static unsigned int major;
static struct class *myclass;

static int eng_open(struct inode *ino, struct file *filp)
{
	unsigned int *en;

	/* ---- PWM2 (GPIOC14): left/right ---- */
	en = gpioc_base + 1;
	*en |= (1 << 14);
	*gpioc_alt = (*gpioc_alt & ~(0x3 << 28)) | (0x2 << 28);
	*gpioc_base &= ~(1 << 14);

	pwm_base->TCFG0 = (pwm_base->TCFG0 & ~(0xFF << 8)) | (PRESCALER << 8);
	pwm_base->TCFG1 = (pwm_base->TCFG1 & ~(0xF << 8))  | (MUX_DIV << 8);
	pwm_base->TCNTB2 = TCNT_VAL;
	pwm_base->TCMPB2 = 0;

	pwm_base->TCON |=  (1 << 13);   /* manual update */
	pwm_base->TCON &= ~(1 << 13);
	pwm_base->TCON |=  (1 << 15);   /* auto reload  */
	pwm_base->TCON |=  (1 << 12);   /* start        */

	/* ---- PWM3 (GPIOD0): up/down ---- */
	en = gpiod_base + 1;
	*en |= (1 << 0);
	*gpiod_alt |= (0x2 << 0);
	*gpiod_base &= ~(1 << 0);

	pwm_base->TCFG1 = (pwm_base->TCFG1 & ~(0xF << 12)) | (MUX_DIV << 12);
	pwm_base->TCNTB3 = TCNT_VAL;
	pwm_base->TCMPB3 = 0;

	pwm_base->TCON |=  (1 << 17);   /* manual update */
	pwm_base->TCON &= ~(1 << 17);
	pwm_base->TCON |=  (1 << 19);   /* auto reload  */
	pwm_base->TCON |=  (1 << 16);   /* start        */

	return 0;
}

static long eng_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	if (arg == PWM2_CTL)
		pwm_base->TCMPB2 = cmd;
	else if (arg == PWM3_CTL)
		pwm_base->TCMPB3 = cmd;
	return 0;
}

static int eng_release(struct inode *ino, struct file *filp)
{
	pwm_base->TCON &= ~((1 << 16) | (1 << 12));  /* stop both timers */
	return 0;
}

static const struct file_operations fops = {
	.open           = eng_open,
	.release        = eng_release,
	.unlocked_ioctl = eng_ioctl,
	.compat_ioctl   = eng_ioctl,
	.owner          = THIS_MODULE,
};

static int __init my_mod_init(void)
{
	printk(KERN_INFO "stENG driver loading\n");

	major = register_chrdev(0, "myEng", &fops);
	if (major <= 0) {
		printk(KERN_ERR "register chrdev failed\n");
		return -EBUSY;
	}

	myclass = class_create(THIS_MODULE, "myEngclass");
	device_create(myclass, NULL, MKDEV(major, 0), NULL, "sm_stENG");

	gpiod_base = ioremap(GPIOD_ADDR, 0x8);
	gpiod_alt  = ioremap(GPIOD_ALT_ADDR, 0x4);
	gpioc_base = ioremap(GPIOC_ADDR, 0x8);
	gpioc_alt  = ioremap(GPIOC_ALT_ADDR, 0x4);
	pwm_base   = ioremap(PWM_ADDR, sizeof(pwm));

	if (!gpiod_base || !gpiod_alt || !gpioc_base || !gpioc_alt || !pwm_base) {
		printk(KERN_ERR "ioremap failed\n");
		return -EBUSY;
	}

	return 0;
}

static void __exit my_mod_exit(void)
{
	pwm_base->TCON &= ~((1 << 16) | (1 << 12));

	device_destroy(myclass, MKDEV(major, 0));
	class_destroy(myclass);
	unregister_chrdev(major, "myEng");

	iounmap(gpiod_base);
	iounmap(gpiod_alt);
	iounmap(gpioc_base);
	iounmap(gpioc_alt);
	iounmap(pwm_base);

	printk(KERN_INFO "stENG driver removed\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zzz");
MODULE_DESCRIPTION("dual servo PWM driver");
