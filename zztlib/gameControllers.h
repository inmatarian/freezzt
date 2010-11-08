/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef  GAMECONTROLLERS_H
#define  GAMECONTROLLERS_H

#include <vector>

class AbstractFileModelFactory;
class AbstractMusicStream;
class ControllerShare;
class FreeZZTManager;
class PlayModeInfoBarWidget;
class ScrollView;
class TextInputWidget;
class TitleModeInfoBarWidget;

// ---------------------------------------------------------------------------

class ControllerInterface
{
  public:
    ControllerInterface();

    void enter( FreeZZTManager *pManager, ControllerShare *pShare );
    void leave();

    virtual void doKeypress( int keycode, int unicode ) { /* */ };
    virtual void doUpdate() { /* */ };
    virtual void doPaint( AbstractPainter *painter ) { /* */ };

  protected:
    virtual void enter_impl() { /* */ };
    virtual void leave_impl() { /* */ };

  protected:
    FreeZZTManager *manager;
    ControllerShare *share;
};

// ---------------------------------------------------------------------------

class PlayController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doKeypress( int keycode, int unicode );
    virtual void doUpdate();
    virtual void doPaint( AbstractPainter *painter );
};

// ---------------------------------------------------------------------------

class TitleController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doKeypress( int keycode, int unicode );
    virtual void doUpdate();
    virtual void doPaint( AbstractPainter *painter );
  protected:
    virtual void enter_impl();
};

// ---------------------------------------------------------------------------

class WorldMenuController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doKeypress( int keycode, int unicode );
    virtual void doUpdate();
    virtual void doPaint( AbstractPainter *painter );
  protected:
    virtual void enter_impl();
    virtual void leave_impl();
};

// ---------------------------------------------------------------------------

class TextViewController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doKeypress( int keycode, int unicode );
    virtual void doUpdate();
    virtual void doPaint( AbstractPainter *painter );
  protected:
    virtual void enter_impl();
    virtual void leave_impl();
};

// ---------------------------------------------------------------------------

class CheatController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doKeypress( int keycode, int unicode );
    virtual void doPaint( AbstractPainter *painter );
  protected:
    virtual void enter_impl();
    virtual void leave_impl();
};

// ---------------------------------------------------------------------------

class PickSpeedController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doKeypress( int keycode, int unicode );
    virtual void doPaint( AbstractPainter *painter );
  protected:
    virtual void enter_impl();
    virtual void leave_impl();
};

// ---------------------------------------------------------------------------

class PauseController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doKeypress( int keycode, int unicode );
    virtual void doPaint( AbstractPainter *painter );
  protected:
    virtual void enter_impl();
    virtual void leave_impl();
};

// ---------------------------------------------------------------------------

class TransitionController: public ControllerInterface
{
  public:
    static ControllerInterface *create();

    virtual void doUpdate();
    virtual void doPaint( AbstractPainter *painter );
  protected:
    virtual void enter_impl();

    int clock;
    bool forward;
};

// ---------------------------------------------------------------------------

class ControllerShare
{
  public:
    ControllerShare();

    ControllerInterface *currentController;
    ControllerInterface *nextController;

    GameWorld *world;
    AbstractFileModelFactory *fileModelFactory;
    AbstractMusicStream *musicStream;
    TitleModeInfoBarWidget titleModeInfoBarWidget;
    PlayModeInfoBarWidget playModeInfoBarWidget;
    TextInputWidget textInputWidget;
    ScrollView scrollView;
    std::vector<int> transitionList;
    int transitionNextBoard;
    bool quitting;
};

#endif

