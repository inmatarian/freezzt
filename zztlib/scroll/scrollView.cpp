/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include "debug.h"
#include "zstring.h"
#include "defines.h"
#include "abstractPainter.h"
#include "abstractScrollModel.h"
#include "scrollView.h"

class ScrollViewPrivate
{
  public:
    ScrollViewPrivate( ScrollView *pSelf );

    enum State {
      None,
      Opening,
      Opened,
      Closing,
      Closed
    };

    void drawContainer( AbstractPainter *painter, int size ) const;
    void drawTitle( AbstractPainter *painter ) const;
    void drawLines( AbstractPainter *painter, int virtualStart ) const;
    void drawTransition( AbstractPainter *painter, int lines ) const;

    void handleInvoked( int line );
    void moveScroll( int dir, int amount );

  public:
    AbstractScrollModel *model;
    int line;
    State state;
    int clock;
    AbstractScrollModel::Action action;

  private:
    ScrollView *self;
};

ScrollViewPrivate::ScrollViewPrivate( ScrollView *pSelf )
  : model( 0 ),
    line( 0 ),
    state( None ),
    clock( 0 ),
    action( AbstractScrollModel::None ),
    self( pSelf )
{
  /* */
}

void ScrollViewPrivate::drawContainer( AbstractPainter *painter, int size ) const
{
  const int top = 13 - ( size + 2 ); 
  const int name = top + 1;
  const int mid = top + 2;
  const int bottom = 13 + size;

  // range: column 5, row 3, to column 53, row 21
  using namespace Defines;
  painter->paintChar(  5, top, PIPE_NSE_DH, 0x0f );
  painter->paintChar(  6, top, PIPE_SWE_DH, 0x0f );
  painter->paintChar( 52, top, PIPE_SWE_DH, 0x0f );
  painter->paintChar( 53, top, PIPE_NSW_DH, 0x0f );
  painter->paintChar(  5, bottom, PIPE_NSE_DH, 0x0f );
  painter->paintChar(  6, bottom, PIPE_NWE_DH, 0x0f );
  painter->paintChar( 52, bottom, PIPE_NWE_DH, 0x0f );
  painter->paintChar( 53, bottom, PIPE_NSW_DH, 0x0f );

  painter->paintChar(  6, name, PIPE_NS, 0x0f );
  painter->paintChar( 52, name, PIPE_NS, 0x0f );
  painter->paintChar(  6, mid, PIPE_NSE_DH, 0x0f );
  painter->paintChar( 52, mid, PIPE_NSW_DH, 0x0f );

  // pipe rows
  for ( int i = 7; i <= 51; i++ )
  {
    painter->paintChar( i, top, PIPE_WE_DH, 0x0f );
    painter->paintChar( i, mid, PIPE_WE_DH, 0x0f );
    painter->paintChar( i, bottom, PIPE_WE_DH, 0x0f );
  }

  // pipe coulmns
  for ( int i = (mid + 1); i <= (bottom - 1); i++ )
  {
    painter->paintChar(  6, i, PIPE_NS, 0x0f );
    painter->paintChar(  7, i, ' ', 0x1f );
    painter->paintChar(  8, i, ' ', 0x1f );
    painter->paintChar( 50, i, ' ', 0x1f );
    painter->paintChar( 51, i, ' ', 0x1f );
    painter->paintChar( 52, i, PIPE_NS, 0x0f );
  }

  // shadow
  for ( int i = (top + 1); i <= (bottom - 1); i++ )
  {
    painter->paintChar(  5, i, ' ', 0x0f );
    painter->paintChar( 53, i, ' ', 0x0f );
  }

  // arrows
  painter->paintChar(  7, 13, 0xaf, 0x1c );
  painter->paintChar( 51, 13, 0xae, 0x1c );
}

static const int SCROLL_NAME_WIDTH = 45;
static const int SCROLL_TEXT_WIDTH = 41;

void ScrollViewPrivate::drawTitle( AbstractPainter *painter ) const
{
  ZString displayLine = model->getTitleMessage();
  const int len = displayLine.length();

  if ( len > SCROLL_NAME_WIDTH ) {
    displayLine.erase(SCROLL_NAME_WIDTH);
  }
  else if ( len < SCROLL_NAME_WIDTH ) {
    const int total = SCROLL_NAME_WIDTH - len;
    const int half = total / 2;
    displayLine.insert( len, half, ' ' );
    displayLine.insert( 0, total - half, ' ' );
  }

  painter->drawText( 7, 4, 0x1e, displayLine );
}

void ScrollViewPrivate::drawLines( AbstractPainter *painter, int virtualStart ) const
{
  if (!model) return;
  const int count = 14;
  const int maxLines = model->lineCount();

  for ( int i = 0; i <= count; i++ )
  {
    const int dln = i + 6;
    const int vln = virtualStart + i - 7;
    ZString displayLine;
    int color = 0x10;

    if ( vln >= 0 && vln < maxLines ) {
      displayLine = model->getLineMessage( vln );

      if ( model->isHighlighted( vln ) )
        color |= 0x0f;
      else
        color |= 0x0e;

      if ( model->getAction(vln) == AbstractScrollModel::SendMessage ) {
        displayLine.insert( 0, "> " );
      }
    }
    else if ( vln == -1 || vln == maxLines ) {
      displayLine = "   - - - - - - - - - - - - - - - - - -   ";
      color |= 0x0e;
    }

    const int len = displayLine.length();
    if ( len > SCROLL_TEXT_WIDTH ) {
      displayLine.erase(SCROLL_TEXT_WIDTH);
    }
    else if ( len < SCROLL_TEXT_WIDTH ) {
      const int spaces = SCROLL_TEXT_WIDTH - len;
      if ( model->isCentered( vln ) ) {
        const int prefix = spaces / 2;
        displayLine.insert( 0, prefix, ' ' );
        displayLine.insert( len+prefix, spaces - prefix, ' ' );
      }
      else {
        displayLine.insert( len, spaces, ' ' );
      }
    }

    painter->drawText( 9, dln, color, displayLine ); 
  }
}

void ScrollViewPrivate::drawTransition( AbstractPainter *painter, int lines ) const
{
  drawContainer( painter, lines );

  const int name = 14 - ( lines + 2 );
  const ZString displayLine( SCROLL_TEXT_WIDTH, ' ' );

  for ( int i = 0; i < lines; i++ ) {
    painter->drawText( 9, 13 - i, 0x10, displayLine ); 
    painter->drawText( 9, 13 + i, 0x10, displayLine ); 
  }

  painter->drawText( 7, name, 0x10, ZString(SCROLL_NAME_WIDTH, ' ') ); 
}

void ScrollViewPrivate::moveScroll( int dir, int amount )
{
  const int maxLine = model->lineCount() - 1;
  if ( dir != 0 ) {
    line += ( dir * amount );
    if ( line < 0 ) line = 0;
    else if ( line > maxLine ) line = maxLine;
  }
  else if ( amount != 0 ) {
    line = maxLine;
  }
  else {
    line = 0;
  }
}

void ScrollViewPrivate::handleInvoked( int line )
{
  action = model->getAction(line);
  self->close();
}

// ---------------------------------------------------------

ScrollView::ScrollView()
  : d( new ScrollViewPrivate(this) )
{
  /* */
}

ScrollView::~ScrollView()
{
  delete d;
  d = 0;
}

void ScrollView::paint( AbstractPainter *painter )
{
  if (isOpened()) {
    d->drawContainer( painter, 8 );
    d->drawTitle( painter );
    d->drawLines( painter, d->line );
  }
  else {
    d->drawTransition( painter, isOpening() ? d->clock : 8 - d->clock );
  }
}

void ScrollView::update()
{
  switch (d->state)
  {
    case ScrollViewPrivate::Opening: {
      d->clock += 1;
      if (d->clock >= 8) {
        d->state = ScrollViewPrivate::Opened;
      }
      break;
    }

    case ScrollViewPrivate::Closing: {
      d->clock += 1;
      if (d->clock >= 8) {
        d->state = ScrollViewPrivate::Closed;
      }
      break;
    }

    default: break;
  }
}

void ScrollView::setModel( AbstractScrollModel *model )
{
  zdebug() << "ScrollView model set to" << model;
  d->model = model;
}

AbstractScrollModel *ScrollView::model() const
{
  return d->model;
}

void ScrollView::doKeypress( int keycode, int unicode )
{
  if (!d->model) return;
  if (!isOpened()) return;

  using namespace Defines;
  const int pageSize = 8;
  switch ( keycode ) {
    case Z_Up:   d->moveScroll( -1, 1 ); break;
    case Z_Down: d->moveScroll(  1, 1 ); break;
    case Z_Home: d->moveScroll(  0, 0 ); break;
    case Z_End:  d->moveScroll(  0, 1 ); break;
    case Z_PageUp:   d->moveScroll( -1, pageSize ); break;
    case Z_PageDown: d->moveScroll(  1, pageSize ); break;
    case Z_Enter:  d->handleInvoked(d->line); break;
    case Z_Escape: close(); break;
    default: break;
  }
}

void ScrollView::open()
{
  d->moveScroll( 0, 0 );
  d->state = ScrollViewPrivate::Opening;
  d->clock = 0;
  d->action = AbstractScrollModel::None;
}
 
void ScrollView::close()
{
  d->state = ScrollViewPrivate::Closing;
  d->clock = 0;
}

ZString ScrollView::data() const
{
  return d->model->getLineData(d->line);
}

AbstractScrollModel::Action ScrollView::action() const
{
  return d->action;
}

bool ScrollView::isOpening() const
{
  return d->state == ScrollViewPrivate::Opening;
}

bool ScrollView::isOpened() const
{
  return d->state == ScrollViewPrivate::Opened;
}

bool ScrollView::isClosing() const
{
  return d->state == ScrollViewPrivate::Closing;
}

bool ScrollView::isClosed() const
{
  return d->state == ScrollViewPrivate::Closed;
}

