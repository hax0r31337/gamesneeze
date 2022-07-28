#pragma once

#include <cstdint>

class INetMessage;
class INetChannelHandler;

#define FLOW_OUTGOING    0
#define FLOW_INCOMING    1
#define MAX_FLOWS        2        // in & out

class INetChannelInfo {
public:

    enum {
        GENERIC = 0,    // must be first and is default group
        LOCALPLAYER,    // bytes for local player entity update
        OTHERPLAYERS,    // bytes for other players update
        ENTITIES,        // all other entity bytes
        SOUNDS,            // game sounds
        EVENTS,            // event messages
        USERMESSAGES,    // user messages
        ENTMESSAGES,    // entity messages
        VOICE,            // voice data
        STRINGTABLE,    // a stringtable update
        MOVE,            // client move cmds
        STRINGCMD,        // string command
        SIGNON,            // various signondata
        TOTAL,            // must be last and is not a real group
    };

    virtual const char* GetAddress( void ) const = 0; // get channel IP address as string
    virtual float GetTime( void ) const = 0;    // current net time
    virtual float GetTimeConnected( void ) const = 0;    // get connection time in seconds
    virtual int GetBufferSize( void ) const = 0;    // netchannel packet history size
    virtual int GetDataRate( void ) const = 0; // send data rate in byte/sec

    virtual bool IsLoopback( void ) const = 0;    // true if loopback channel
    virtual bool IsTimingOut( void ) const = 0;    // true if timing out
    virtual bool IsPlayback( void ) const = 0;    // true if demo playback

    virtual float GetLatency( int flow ) const = 0;     // current latency (RTT), more accurate but jittering
    virtual float GetAvgLatency( int flow ) const = 0; // average packet latency in seconds
    virtual float GetAvgLoss( int flow ) const = 0;     // avg packet loss[0..1]
    virtual float GetAvgChoke( int flow ) const = 0;     // avg packet choke[0..1]
    virtual float GetAvgData( int flow ) const = 0;     // data flow in bytes/sec
    virtual float GetAvgPackets( int flow ) const = 0; // avg packets/sec
    virtual int GetTotalData( int flow ) const = 0;     // total flow in/out in bytes
    virtual int GetSequenceNr( int flow ) const = 0;    // last send seq number
    virtual bool IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
    virtual float GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
    virtual int GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
    virtual bool GetStreamProgress( int flow, int* received, int* total ) const = 0;  // TCP progress if transmitting
    virtual float GetTimeSinceLastReceived( void ) const = 0;    // get time since last recieved packet in seconds
    virtual float GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;

    virtual void GetPacketResponseLatency( int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke ) const = 0;

    virtual void GetRemoteFramerate( float* pflFrameTime, float* pflFrameTimeStdDeviation ) const = 0;

    virtual float GetTimeoutSeconds() const = 0;
    virtual const char* GetName( void ) const = 0;    // get channel name
};

class bf_write {
public:
  char pad_0000[12];        // 0x0000
  int32_t m_nDataBytes;     // 0x000C
  int32_t m_nDataBits;      // 0x0010
  int32_t m_iCurBit;        // 0x0014
  bool m_bOverflow;         // 0x0018
  bool m_bAssertOnOverflow; // 0x0019
  char pad_001A[2];         // 0x001A
  char *m_pDebugName;       // 0x001C
};                          // Size: 0x0024

class INetChannel : public INetChannelInfo {
public:
  virtual ~INetChannel(void){};   // 27
  virtual void Destroy(void) = 0; // calls destructor.
  virtual void SetDataRate(float rate) = 0;
  virtual void __unknown1(void) = 0; // 30
  virtual bool RegisterMessage(INetMessage *msg) = 0;
  virtual bool StartStreaming(unsigned int challengeNr) = 0;
  virtual void ResetStreaming(void) = 0;
  virtual void SetTimeout(float seconds) = 0;
  virtual void SetDemoRecorder(/*IDemoRecorder*/ void *recorder) = 0; // 35
  virtual void SetChallengeNr(unsigned int chnr) = 0;
  virtual void Reset(void) = 0;
  virtual void Clear(void) = 0;
  virtual void Shutdown(const char *reason) = 0;
  virtual void ProcessPlayback(void) = 0; // 40
  virtual bool ProcessStream(void) = 0;
  virtual void ProcessPacket(/*struct netpacket_s*/ void *packet,
                             bool bHasHeader) = 0;
  virtual bool SendNetMsg(INetMessage &msg, bool bForceReliable = false,
                          bool bVoice = false) = 0;
#ifdef POSIX
  FORCEINLINE bool SendNetMsg(INetMessage const &msg,
                              bool bForceReliable = false,
                              bool bVoice = false) {
    return SendNetMsg(*((INetMessage *)&msg), bForceReliable, bVoice);
  }
#endif
  virtual bool SendData(/*bf_write&*/ void *msg, bool bReliable = true) = 0;
  virtual bool SendFile(const char *filename,
                        unsigned int transferID) = 0; // 45
  virtual void DenyFile(const char *filename, unsigned int transferID) = 0;
  virtual void
  RequestFile_OLD(const char *filename,
                  unsigned int transferID) = 0; // DEFUNCT. Errors when called.
  virtual void SetChoked(void) = 0;
  virtual int SendDatagram(/*bf_write*/ void *data) = 0;
  virtual bool Transmit(bool onlyReliable = false) = 0; // 50
  virtual void /*const netadr_t& */ GetRemoteAddress(void) const = 0;
  virtual INetChannelHandler *GetMsgHandler(void) const = 0;
  virtual int GetDropNumber(void) const = 0;
  virtual int GetSocket(void) const = 0;
  virtual unsigned int GetChallengeNr(void) const = 0; // 55
  virtual void GetSequenceData(int &nOutSequenceNr, int &nInSequenceNr,
                               int &nOutSequenceNrAck) = 0;
  virtual void SetSequenceData(int nOutSequenceNr, int nInSequenceNr,
                               int nOutSequenceNrAck) = 0;
  virtual void UpdateMessageStats(int msggroup, int bits) = 0;
  virtual bool CanPacket(void) const = 0;
  virtual bool IsOverflowed(void) const = 0; // 60
  virtual bool IsTimedOut(void) const = 0;
  virtual bool HasPendingReliableData(void) = 0;
  virtual void SetFileTransmissionMode(bool bBackgroundMode) = 0;
  virtual void SetCompressionMode(bool bUseCompression) = 0; // 64
  virtual unsigned int RequestFile(const char *filename) = 0;
  virtual void SetMaxBufferSize(bool bReliable, int nBytes,
                                bool bVoice = false) = 0; // 66
  /* VIRTUAL SOUP START */
  virtual void __unknown2(void) = 0;
  virtual void __unknown3(void) = 0;
  virtual void __unknown4(void) = 0;
  virtual void __unknown5(void) = 0;
  /* CONTAINS SOME OF THESE
  virtual float GetTimeSinceLastReceived( void ) const = 0;    // get time since
  last received packet in seconds virtual bool IsNull() const = 0; virtual int
  GetNumBitsWritten( bool bReliable ) = 0; virtual void SetInterpolationAmount(
  float flInterpolationAmount ) = 0; virtual void SetRemoteFramerate( float
  flFrameTime, float flFrameTimeStdDeviation ) = 0;
  */
  // Max # of payload bytes before we must split/fragment the packet
  virtual void SetMaxRoutablePayloadSize(int nSplitSize) = 0; // 71
  virtual int GetMaxRoutablePayloadSize() = 0;
  virtual int GetProtocolVersion() = 0;
  virtual void __unknown6(void) = 0;
  virtual void __unknown7(void) = 0;
  virtual void __unknown8(void) = 0;
  virtual void __unknown9(void) = 0;
  virtual void *GetChannelEncryptionKey(void) = 0;
  virtual void __unknown11(void) = 0;

  char pad_0000[0x20];         // 0x0000 - Minus 8 for vtable.
  bool m_bProcessingMessages;  // 0x0028
  bool m_bShouldDelete;        // 0x0029
  char pad_002A[2];            // 0x002A
  int32_t m_nOutSequenceNr;    // 0x002C
  int32_t m_nInSequenceNr;     // 0x0030
  int32_t m_nOutSequenceNrAck; // 0x0034
  int32_t m_nOutReliableState; // 0x0038
  int32_t m_nInReliableState;  // 0x003C
  int32_t m_nChokedPackets;    // 0x0040
  bf_write m_StreamReliable;   // 0x0044
  char pad_0068[12];           // 0x0068
  bf_write m_StreamUnreliable; // 0x0074
  char pad_0098[112];          // 0x0098
  char m_szServerIP[32]; // 0x0108 This really probably is a netadr_t around
                         // here, but whatever
  char pad_0128[36];      // 0x0128
  float m_flLastReceived; // 0x014C Time last received
  char pad_0150[4];       // 0x0150
  float m_flConnectTime;  // 0x0154
  int32_t m_nRate;        // 0x0158
  char pad_015C[16768];   // 0x015C
  char N00000B08[32];     // 0x42DC
  char pad_42FC[4];       // 0x42FC
  float timeout;          // 0x4300 cl_timeout
  char pad_4304[52];      // 0x4304
  float interpAmount;     // 0x4338
  float serverFrameTime;  // 0x433C  Figured this out by doing the classic throw
                         // a grenade into 100 negevs on the ground.
  float serverFrameTimestdDeviation; // 0x4340 Also correlates to net_graph
                                     // numbers.
  float maxRoutablePayloadSize; // 0x4344
  int32_t splitPacketSequence;  // 0x4348
};