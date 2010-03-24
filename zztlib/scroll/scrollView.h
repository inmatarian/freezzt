/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SCROLL_VIEW_H
#define SCROLL_VIEW_H

class AbstractScrollModel;

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

  private:
    ScrollViewPrivate *d;
};

#endif /* SCROLL_VIEW_H */

