/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SCROLL_MODEL_H
#define SCROLL_MODEL_H

class ScrollModelPrivate;
class ScrollModel
{
  public:
    ScrollModel();
    virtual ~ScrollModel();

  private:
    ScrollModelPrivate *d;
};

#endif /* SCROLL_MODEL_H */

