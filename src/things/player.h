/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "zztThing.h"

namespace ZZTThing {

class Player : public AbstractThing
{
  public:
    Player();
    virtual unsigned char entityID() const { return ZZTEntity::Player; };
    virtual unsigned char tile() const { return 0x02; };
    virtual bool pushes() const { return true; };

    bool tryEnterBoard( int nx, int ny, int dx, int dy );

  protected:
    virtual void exec_impl();

    virtual void handleEdgeOfBoard( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
    virtual void handlePassage( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
    virtual void handleForest( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
    virtual void handleAmmo( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
    virtual void handleTorch( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
    virtual void handleGem( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
    virtual void handleKey( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
    virtual void handleDoor( const ZZTEntity &ent, int ox, int oy, int dx, int dy );
};

}; // namespace

#endif // PLAYER_H

