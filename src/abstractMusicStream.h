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

    AbstractMusicStream() : m_priority(None) { /* */ };
    virtual ~AbstractMusicStream() { /* */ };

    /// Parses a ZZT Song out into notes
    void playMusic( const char *song, PriorityType priority );

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

  protected:
    /// implement in derived class
    virtual void begin() = 0;
    /// implement in derived class
    virtual void clear() = 0;
    /// implement in derived class
    virtual bool hasNotes() const = 0;
    /** addNote queues up notes to play
     * implement in derived class
     * @tone true for tones, false for drums
     * @key 40 is Middle C for tones
     * @ticks 32 ticks is a whole note.
     */
    virtual void addNote( bool tone, int key, int ticks ) = 0;
    /// implement in derived class
    virtual void end() = 0;

  private:
    PriorityType m_priority;
};

#endif /* __ABSTRACT_MUSIC_STREAM_H__ */

