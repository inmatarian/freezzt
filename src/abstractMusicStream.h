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
    AbstractMusicStream() { /* */ };
    virtual ~AbstractMusicStream() { /* */ };

    enum PriorityType {
      None = 0,
      Lowest,
      Low,
      Medium,
      High,
      Highest
    };

    /// implement in derived class to pass zzt-style songs in
    virtual void playMusic( const char *song, PriorityType priority ) = 0;

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
};

#endif /* __ABSTRACT_MUSIC_STREAM_H__ */

