/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __ABSTRACT_PLATFORM_SERVICES_H__
#define __ABSTRACT_PLATFORM_SERVICES_H__

class AbstractPainter;
class AbstractEventLoop;
class AbstractMusicStream;
class AbstractScrollModel;

class AbstractPlatformServices
{
  public:
    /// Painter acquired before drawing a frame
    virtual AbstractPainter * acquirePainter() = 0;
    /// Current painter must be the same one returned by acquirePainter
    virtual AbstractPainter * currentPainter() = 0;
    /// Painter released after drawing a frame
    virtual void releasePainter( AbstractPainter * ) = 0;

    /// Event loop acquired before game starts
    virtual AbstractEventLoop * acquireEventLoop() = 0;
    /// Current Event loop must be the same one returned by acquire
    virtual AbstractEventLoop * currentEventLoop() = 0;
    /// Event loop released at the end of the game
    virtual void releaseEventLoop( AbstractEventLoop * ) = 0;

    /// MusicStream acquired before game starts
    virtual AbstractMusicStream * acquireMusicStream() = 0;
    /// Current MusicStream must be the same one returned by acquireMusicStream
    virtual AbstractMusicStream * currentMusicStream() = 0;
    /// MusicStream released at the end of the game
    virtual void releaseMusicStream( AbstractMusicStream * ) = 0;

    /// File Model for browsing zzt files
    virtual AbstractScrollModel * acquireFileListModel( const std::string &directory = "" ) = 0;
    /// File Model for browsing zzt files
    virtual AbstractScrollModel * currentFileListModel() = 0;
    /// File Model for browsing zzt files
    virtual void releaseFileListModel( AbstractScrollModel *model ) = 0;
};

#endif /* __ABSTRACT_PLATFORM_SERVICES_H__ */

