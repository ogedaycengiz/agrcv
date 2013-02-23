#ifndef SERIPORT_H
#define SERIPORT_H

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


class SeriPort
{
public:
    SeriPort();
    void yaz(string ss);
    void portBelirle(char *_port_adi);
    bool ac();
    bool kapat();
private:
    char *port_adi;

    struct termios tio;
    struct termios stdio;
    int tty_fd;
    fd_set rdset;
};

#endif // SERIPORT_H


SeriPort::SeriPort()
{

}


void SeriPort::yaz(string ss)
{

}


bool SeriPort::ac()
{
    tty_fd = open(port_adi, O_RDWR | O_NOCTTY | O_NDELAY);
    if (tty_fd == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}


bool SeriPort::kapat()
{
    close(tty_fd);
}


void SeriPort::portBelirle(char *_port_adi)
{
    port_adi = _port_adi;
}
