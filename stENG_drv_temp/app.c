/*
 * app.c -- simple dual servo control via stENG driver
 * PWM2 (GPIOC14): left/right
 * PWM3 (GPIOD0): up/down
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEV_PATH "/dev/sm_stENG"
#define PWM2_CTL 0x2
#define PWM3_CTL 0x3

#define LR_MIN    25
#define LR_MAX    125
#define LR_CENTER 75

#define UD_MIN    17
#define UD_MAX    117
#define UD_CENTER 67

int main(void)
{
    int fd, lr = LR_CENTER, ud = UD_CENTER;
    int sel;

    fd = open(DEV_PATH, O_RDWR);
    if (fd < 0) {
        perror("open " DEV_PATH);
        return 1;
    }

    ioctl(fd, LR_CENTER, PWM2_CTL);
    ioctl(fd, UD_CENTER, PWM3_CTL);

    while (1) {
        printf("\n 1: left  2: right  3: up  4: down  0: quit\n");
        printf(" LR [%3d/%3d]  UD [%3d/%3d]\n", lr, LR_MAX, ud, UD_MAX);
        printf(" choose: ");
        if (scanf("%d", &sel) != 1) break;

        switch (sel) {
        case 0:
            goto quit;
        case 1:
            if (lr < LR_MAX) lr += 1;
            break;
        case 2:
            if (lr > LR_MIN) lr -= 1;
            break;
        case 3:
            if (ud < UD_MAX) ud += 1;
            break;
        case 4:
            if (ud > UD_MIN) ud -= 1;
            break;
        default:
            continue;
        }

        ioctl(fd, (unsigned int)lr, PWM2_CTL);
        ioctl(fd, (unsigned int)ud, PWM3_CTL);
    }
quit:
    ioctl(fd, LR_CENTER, PWM2_CTL);
    ioctl(fd, UD_CENTER, PWM3_CTL);
    close(fd);
    return 0;
}
