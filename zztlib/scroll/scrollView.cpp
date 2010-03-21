/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include "debug.h"
#include "defines.h"
#include "abstractPainter.h"
#include "scrollView.h"

class ScrollViewPrivate
{
  public:
    ScrollViewPrivate( ScrollView *pSelf );

    void drawContainer( AbstractPainter *painter ) const;

  private:
    ScrollView *self;
};

ScrollViewPrivate::ScrollViewPrivate( ScrollView *pSelf )
  : self( pSelf )
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

  for ( int i = 7; i <= 51; i++ )
  {
    painter->paintChar( i,  3, PIPE_WE_DH, 0x0f );
    painter->paintChar( i,  5, PIPE_WE_DH, 0x0f );
    painter->paintChar( i, 21, PIPE_WE_DH, 0x0f );
  }

  for ( int i = 6; i <= 20; i++ )
  {
    painter->paintChar(  6, i, PIPE_NS, 0x0f );
    painter->paintChar( 52, i, PIPE_NS, 0x0f );
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



}


