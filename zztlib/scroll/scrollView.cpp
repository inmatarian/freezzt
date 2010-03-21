/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include "debug.h"
#include "defines.h"
#include "abstractPainter.h"
#include "abstractScrollModel.h"
#include "scrollView.h"

class ScrollViewPrivate
{
  public:
    ScrollViewPrivate( ScrollView *pSelf );

    void drawContainer( AbstractPainter *painter ) const;
    void drawTitle( AbstractPainter *painter ) const;
    void drawLines( AbstractPainter *painter, int visualStart,
                    int virtualStart, int count ) const;

  public:
    AbstractScrollModel *model;

  private:
    ScrollView *self;
};

ScrollViewPrivate::ScrollViewPrivate( ScrollView *pSelf )
  : model( 0 ),
    self( pSelf )
{
  /* */
}

void ScrollViewPrivate::drawContainer( AbstractPainter *painter ) const
{
  // range: column 5, row 3, to column 53, row 21
  using namespace Defines;
  painter->paintChar(  5,  3, PIPE_NSE_DH, 0x0f );
  painter->paintChar(  6,  3, PIPE_SWE_DH, 0x0f );
  painter->paintChar( 52,  3, PIPE_SWE_DH, 0x0f );
  painter->paintChar( 53,  3, PIPE_NSW_DH, 0x0f );
  painter->paintChar(  5, 21, PIPE_NSE_DH, 0x0f );
  painter->paintChar(  6, 21, PIPE_NWE_DH, 0x0f );
  painter->paintChar( 52, 21, PIPE_NWE_DH, 0x0f );
  painter->paintChar( 53, 21, PIPE_NSW_DH, 0x0f );

  painter->paintChar(  6, 4, PIPE_NS, 0x0f );
  painter->paintChar( 52, 4, PIPE_NS, 0x0f );
  painter->paintChar(  6, 5, PIPE_NSE_DH, 0x0f );
  painter->paintChar( 52, 5, PIPE_NSW_DH, 0x0f );

  // pipe rows
  for ( int i = 7; i <= 51; i++ )
  {
    painter->paintChar( i,  3, PIPE_WE_DH, 0x0f );
    painter->paintChar( i,  5, PIPE_WE_DH, 0x0f );
    painter->paintChar( i, 21, PIPE_WE_DH, 0x0f );
  }

  // pipe coulmns
  for ( int i = 6; i <= 20; i++ )
  {
    painter->paintChar(  6, i, PIPE_NS, 0x0f );
    painter->paintChar(  7, i, ' ', 0x1f );
    painter->paintChar(  8, i, ' ', 0x1f );
    painter->paintChar( 50, i, ' ', 0x1f );
    painter->paintChar( 51, i, ' ', 0x1f );
    painter->paintChar( 52, i, PIPE_NS, 0x0f );
  }

  // shadow
  for ( int i = 4; i <= 20; i++ )
  {
    painter->paintChar(  5, i, ' ', 0x0f );
    painter->paintChar( 53, i, ' ', 0x0f );
  }

  // arrows
  painter->paintChar(  7, 12, 0xaf, 0x1c );
  painter->paintChar( 51, 12, 0xae, 0x1c );
}

void ScrollViewPrivate::drawTitle( AbstractPainter *painter ) const
{
  const int rowWidth = 45;

  std::string displayLine = model->getTitleMessage();
  const int len = displayLine.length();

  if ( len > rowWidth ) {
    displayLine.erase(rowWidth);
  }
  else if ( len < rowWidth ) {
    const int total = rowWidth - len;
    const int half = total / 2;
    displayLine.insert( len, half, ' ' );
    displayLine.insert( 0, total - half, ' ' );
  }

  painter->drawText( 7, 4, 0x1e, displayLine );
}

void ScrollViewPrivate::drawLines( AbstractPainter *painter, int visualStart,
                                   int virtualStart, int count ) const
{
  if (!model) return;
  const int rowWidth = 41;

  for ( int i = 0; i <= count; i++ )
  {
    const int dln = visualStart + i + 6;
    const int vln = virtualStart + i;

    std::string displayLine = model->getLineMessage( vln );
    const int len = displayLine.length();
    if ( len > rowWidth ) {
      displayLine.erase(rowWidth);
    }
    else if ( len < rowWidth ) {
      displayLine.insert( len, rowWidth - len, ' ' );
    }

    painter->drawText( 9, dln, 0x1e, displayLine ); 
  }
}

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
  d->drawContainer( painter );
  d->drawTitle( painter );
  d->drawLines( painter, 0, 0, 14 );
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

