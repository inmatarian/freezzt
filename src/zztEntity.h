// Insert copyright and license information here.

#ifndef ZZTENTITY_H
#define ZZTENTITY_H

class ZZTEntity
{
  public:
    virtual unsigned char tile() const = 0;
    virtual unsigned char color() const = 0;
};

class EmptySpaceEntity : public ZZTEntity
{
  virtual unsigned char tile() const { return ' '; }
  virtual unsigned char color() const { return 0x07; }
};

#endif // ZZTENTITY_H

