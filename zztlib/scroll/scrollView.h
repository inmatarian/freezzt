/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SCROLL_VIEW_H
#define SCROLL_VIEW_H

#include "abstractScrollModel.h"

class AbstractPainter;

class ScrollViewPrivate;
class ScrollView
{
  public:
    ScrollView();
    virtual ~ScrollView();

    void paint( AbstractPainter *painter );
    void update();

    void setModel( AbstractScrollModel *model );
    AbstractScrollModel *model() const;

    void doKeypress( int keycode, int unicode );

    void open();
    void close();

    bool isOpening() const;
    bool isOpened() const;
    bool isClosing() const;
    bool isClosed() const;

    ZString data() const;
    AbstractScrollModel::Action action() const;

  private:
    ScrollViewPrivate *d;
};

#endif /* SCROLL_VIEW_H */

