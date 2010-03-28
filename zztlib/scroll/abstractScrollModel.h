/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef ABSTRACT_SCROLL_MODEL_H
#define ABSTRACT_SCROLL_MODEL_H

class AbstractScrollModel
{
  public:
    enum Action {
      None,
      ChangeDirectory,
      LoadFile,
      SendMessage,
      Close
    };

    virtual std::string getTitleMessage() const = 0;
    virtual std::string getLineMessage( int line ) const = 0;
    virtual std::string getLineData( int line ) const = 0;
    virtual int getLineColorFG( int line ) const = 0;
    virtual Action getAction( int line ) const = 0;
    virtual int lineCount() const = 0;
};

#endif /* ABSTRACT_SCROLL_MODEL_H */

