#include "boardinfo.h"

#define JSetOldDPMSLevel    CardData[14]
#define JDPMSLevel          CardData[15]

#define VENDOR_E3B            3643
#define VENDOR_MATAY          44359
#define DEVICE_FIRESTORM      200
#define DEVICE_PROMETHEUS     1
#define BOARD_NAME          "Prometheus"
#define BOARD_TYPE          BT_Prometheus

#define LIB(a) struct Library *##a = cb->cb_##a

#ifdef DEBUG
#define D(x) x
void kprintf(STRPTR format, ...);
#else
#define D(x)
#endif

/* DMAMemChunk structure describes single DMA memory chunk. */

struct DMAMemChunk
  {
    struct MinNode  dmc_Node;         /* for linking into list */
    APTR            dmc_Address;      /* logical (CPU) address */
    struct Task    *dmc_Owner;        /* memory owner (NULL if none) */
    ULONG           dmc_Size;         /* chunk size in bytes */
    APTR            dmc_AlignedAddr;  /* Aligned on 4K address */
  };

struct CardBase
  {
    struct Library        cb_Library;
    UBYTE                 cb_Flags;
    UBYTE                 cb_Pad;
    struct Library       *cb_SysBase;
    struct Library       *cb_ExpansionBase;
    APTR                  cb_SegList;
    STRPTR                cb_Name;

    /* standard fields ends here */

    struct Library         *cb_PrometheusBase;
    BOOL                    cb_DMAMemGranted;
    APTR                    cb_LegacyIOBase;
    APTR                    cb_MemPool;         /* for DMA memory management lists */
    struct MinList          cb_MemList;
//    struct DMAMemChunk      cb_MemRoot;
    struct SignalSemaphore *cb_MemSem;
	UBYTE					monitorSwitchType;
  };

BOOL Init3dfxVoodoo(struct CardBase *cb, struct BoardInfo *bi);      // check Banshee/Voodoo3/4/5 based cards
BOOL Init3DLabsPermedia2(struct CardBase *cb, struct BoardInfo *bi); // check Permedia2 based cards (3DLabs/TI)
BOOL InitCirrusGD5446(struct CardBase *cb, struct BoardInfo *bi);    // check GD5446 based Cirrus cards
BOOL InitS3ViRGE(struct CardBase *cb, struct BoardInfo *bi);         // check ViRGE based S3 cards
BOOL InitATIRadeon(struct CardBase *cb, struct BoardInfo *bi);      // check ATIRadeon based cards
VOID InitDMAMemory(struct CardBase *cb, APTR memory, ULONG size);

void RegisterIntServer(struct CardBase *cb, void *board, struct Interrupt *interrupt);
void RegisterOwner(struct CardBase *cb, void *board, struct Node *driver);

BOOL InitCard(__REGA0(struct BoardInfo *bi), __REGA1(char **ToolTypes), __REGA6(struct CardBase *base));
BOOL FindCard(__REGA0(struct BoardInfo *bi), __REGA6(struct CardBase *base));
APTR AllocDMAMemory(__REGD0(ULONG size), __REGA6(struct CardBase *cb));
void FreeDMAMemory(__REGA0(APTR membase), __REGD0(ULONG memsize), __REGA6(struct CardBase *cb));

#define NewList(l) (((struct List*)l)->lh_TailPred = (struct Node*)(l), \
                    ((struct List*)l)->lh_Tail = 0, \
                    ((struct List*)l)->lh_Head = (struct Node*)&(((struct List*)l)->lh_Tail))
