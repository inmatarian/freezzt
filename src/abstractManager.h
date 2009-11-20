// Insert copyright and license information here.

#ifndef ABSTRACT_MANAGER_H
#define ABSTRACT_MANAGER_H

class AbstractManager
{
  public:
    virtual void exec() = 0;

    virtual void setFrameRate( int hertz ) = 0;
    virtual int frameRate() const = 0;
};

#endif // ABSTRACT_MANAGER_H

