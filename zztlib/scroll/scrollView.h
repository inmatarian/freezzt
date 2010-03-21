/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SCROLL_VIEW_H
#define SCROLL_VIEW_H

class ScrollViewPrivate;
class ScrollView
{
  public:
    ScrollView();
    virtual ~ScrollView();

  private:
    ScrollViewPrivate *d;
};

#endif /* SCROLL_VIEW_H */

