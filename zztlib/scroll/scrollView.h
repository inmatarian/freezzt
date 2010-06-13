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

    void setModel( AbstractScrollModel *model );
    AbstractScrollModel *model() const;

    void doKeypress( int keycode, int unicode );

    enum State {
      None,
      Opening,
      Opened,
      Closing,
      Closed
    };

    State state() const;
    void open();
    void close();

    ZString data() const;
    AbstractScrollModel::Action action() const;

  private:
    ScrollViewPrivate *d;
};

#endif /* SCROLL_VIEW_H */

