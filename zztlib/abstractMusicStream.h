/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __ABSTRACT_MUSIC_STREAM_H__
#define __ABSTRACT_MUSIC_STREAM_H__

/// interface for playing zzt songs
class AbstractMusicStream
{
  public:
    enum PriorityType {
      None = 0,
      Lowest,
      Low,
      Medium,
      High,
      Highest
    };

    AbstractMusicStream() : m_priority(None), m_begun(false), m_quiet(false) { /* */ };
    virtual ~AbstractMusicStream() { /* */ };

    /// Parses a ZZT Song out into notes
    void playMusic( const ZString &song, PriorityType priority = Medium );

    enum EventType {
      Ricochet,
      Transport,
      Forest,
      LockedDoor,
      Water,
      EnergizerOn,
      EnergizerSong,
      EnergizerEnd,
      Key,
      Door,
      Gem,
      Ammo,
      Torch,
      Shoot,
      Ouch,
      Passage,
      Push,
      Breakable,
      KillEnemy,
      TorchExtinguish,
      InvisibleWall,
      Scroll,
      Duplicator,
      BombSet,
      BombTick,
      BombTock,
      BombBoom,
      FootStep,
      ParseError
    };

    /// convienence function for playing common songs
    void playEvent( EventType event );

    /// template pattern method, calls begin_impl
    void begin() { m_begun = true; begin_impl(); };

    /// template pattern method, calls end_impl
    void end() { m_begun = false; end_impl(); };

    /// accessor
    bool isQuiet() const { return m_quiet; };

    /// changes the sound volume
    void setQuiet( bool quiet );

  protected:
    /// start of cycle, implement in derived class
    virtual void begin_impl() = 0;

    /// wipe out existing notes, implement in derived class
    virtual void clear() = 0;

    /// if there's anything in the queue, implement in derived class
    virtual bool hasNotes() const = 0;

    /** addNote queues up notes to play
     * implement in derived class
     * @tone true for tones, false for drums
     * @key 40 is Middle C for tones
     * @ticks 32 ticks is a whole note.
     */
    virtual void addNote( bool tone, int key, int ticks ) = 0;

    /// end of cycle, implement in derived class
    virtual void end_impl() = 0;

  private:
    PriorityType m_priority;
    bool m_begun;
    bool m_quiet;
};

#endif /* __ABSTRACT_MUSIC_STREAM_H__ */

