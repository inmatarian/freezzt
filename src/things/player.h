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
  protected:
    virtual void exec_impl();

    void interact( int dx, int dy );
    void handleEdgeOfBoard( int dx, int dy );
    void handleForest( int dx, int dy );
};

}; // namespace

#endif // PLAYER_H

