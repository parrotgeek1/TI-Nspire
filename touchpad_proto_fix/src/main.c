#include <libndls.h>
#include <os.h>

void showmsg(char *mesg){
    show_msgbox("HWRevFix by parrotgeek1", mesg);
}


static const unsigned read_nand_31_addrs[]            = {    0x10071F5C, 0x10071EC4, 0x10071658, 0X100715E8, 0x1006E0AC, 0x1006E03C};
static const unsigned write_nand_31_addrs[]            = {    0x10072298, 0x10072200, 0x10071994, 0x10071924, 0x1006E3E8, 0x1006E378};
static const unsigned nand_erase_range_31_addrs[]    = {    0x100724FC, 0x10072464, 0x10071BF8, 0x10071B88, 0x1006E64C, 0x1006E5DC};

#define read_nand_31        SYSCALL_CUSTOM(read_nand_31_addrs ,void,  void* dest, int size, int offset, int, int percent_max, void *progress_cb )
#define write_nand_31        SYSCALL_CUSTOM(write_nand_31_addrs,int,  void *source,int size, unsigned int endoffset)
#define nand_erase_range_31    SYSCALL_CUSTOM(nand_erase_range_31_addrs,int, int offset,int end)

#define PAGE_SIZE 512

// backward compatible
void bc_read_nand(void* dest, int size, int offset, int unknown, int percent_max, void *progress_cb) {
    if (nl_ndless_rev() < 989) // Ndless 3.1
        read_nand_31(dest, size, offset, unknown, percent_max, progress_cb);
    else
        read_nand(dest, size, offset, unknown, percent_max, progress_cb);
}

int bc_write_nand(char* data, int size, int start) {
    if (nl_ndless_rev() < 989) // Ndless 3.1
        write_nand_31(data, size, start);
    else
        write_nand(data, size, start);
    return 0;
}

int bc_nand_erase_range(int start, int end) {
    if (nl_ndless_rev() < 989) // Ndless 3.1
        nand_erase_range_31(start, end);
    else
        nand_erase_range(start, end);
    return 0;
}

int main(void) {
    assert_ndless_rev(801);

    char *buf = malloc(0x20*PAGE_SIZE);

    if(!buf) {
        showmsg("ERROR: Out of memory!\n\nTry restarting your calculator with the reset button.");
        return 0;
    }

    memset(buf,0,0x20*PAGE_SIZE);
    bc_read_nand(buf,0x20*PAGE_SIZE,0x0,0,0,NULL);

    if(buf[0x804] != 0x0c) {
        showmsg("This program is only supported on CAS monochrome calculators. If you are having touchpad problems on another calculator, please ask me for help.");
        return 0;
    }

    if(buf[0x806] == 0x02) {
        showmsg("Your calculator's hardware revision is already correct. If you are having touchpad problems in spite of this, please ask me for help.");
        return 0;
    }

    buf[0x806] = 0x02;

    bc_nand_erase_range(0x0,(0x20*PAGE_SIZE)-1);

    bc_write_nand(buf,0x20*PAGE_SIZE,0x0);

    memset(buf,0,0x20*PAGE_SIZE);
    bc_read_nand(buf,0x20*PAGE_SIZE,0x0,0,0,NULL);

    if(buf[0x804] != 0x0c) {
        showmsg("FATAL ERROR 1\n\nDO NOT TURN OFF YOUR CALCULATOR. ASK ME FOR HELP!");
        return 0;
    }

    if(buf[0x806] != 0x02) {
        showmsg("FATAL ERROR 2\n\nDO NOT TURN OFF YOUR CALCULATOR. ASK ME FOR HELP!");
        return 0;
    }

    free(buf);


    showmsg("SUCCESS!\n\nPress Enter to reboot your calculator.");

    *(volatile unsigned*)0x900A0008 = 2; //CPU reset

    return 0;
}
