#include <kipr/botball.h>

void cwb(int byte) {
	create_write_byte(byte);
}
void pn(int note, int duration) {
    printf("%d\n", note);
	create_write_byte(note);
    msleep(50);
    create_write_byte(duration);
    msleep(50);
}
int main()
{
    printf("Hello World\n");
    create_connect();
    create_full();
    cwb(7); // reset roomba
    msleep(1000); // wait for it
    cwb(128); // turn back on
    msleep(5000);
    printf("make sdfhj\n");
    cwb(140);
    cwb(0);
    cwb(8);
    
    pn(37, 16); // C#
    pn(35, 16); // B
    pn(34, 16); // A#
    pn(35, 16); // B
    pn(38, 16); // D
    pn(42, 16); // F#
    pn(35, 16); // B
    pn(67, 16); // D
    cwb(141);
    cwb(0);
    create_drive_direct(100,100);
    msleep(10);
    msleep(10000);
    create_disconnect();
    return 0;
}
