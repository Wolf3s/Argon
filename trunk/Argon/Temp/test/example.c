

#include <stdio.h>
#include <stdlib.h> 
#include <kernel.h>
#include <debug.h>
#include <sifrpc.h>
#include <string.h>
#include <fileXio_rpc.h>
#include <fcntl.h>
#include <libhdd.h>

#include "SMS_Timer.h"
#include "SMS_Data.h"

extern unsigned char g_DataBuffer[SMS_DATA_BUFFER_SIZE];


//#define DEBUG

unsigned int g_IOPFlags;

void LoadHdd(void);
void CloseHdd(void);

//hdd stuff

# define SMS_IOPF_DEV9     0x00000001
# define SMS_IOPF_HDD      0x00000002

static char s_HDDArgs[] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = {
 '-', 'o', '\x00', '4',      '\x00',
 '-', 'n', '\x00', '2', '0', '\x00'
};

static char s_PFSArgs[] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = {
 '-', 'm', '\x00', '4',      '\x00',
 '-', 'o', '\x00', '1', '0', '\x00',
 '-', 'n', '\x00', '4', '0', '\x00'
};

static char s_pAudSrv [] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "AUDSRV";
static char s_pIOManX [] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "IOMANX";
static char s_pFileXIO[] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "FILEXIO";
static char s_pPS2Dev9[] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "PS2DEV9";
static char s_pPS2ATAD[] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "PS2ATAD";
static char s_pPS2HDD [] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "PS2HDD";
static char s_pPS2FS  [] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "PS2FS";
static char s_pPS2POff[] __attribute__(   (  section( ".data" ), aligned( 1 )  )   ) = "POWEROFF";

struct {

 const char* m_pName;
 void*       m_pBuffer;
 int         m_BufSize;
 int         m_nArgs;
 void*       m_pArgs;

} s_LoadParams[ 8 ] __attribute__(   (  section( ".data" )  )   ) = {
 { s_pAudSrv,  &g_DataBuffer[ SMS_AUDSRV_OFFSET   ], SMS_AUDSRV_SIZE,   0,                    NULL      },
 { s_pIOManX,  &g_DataBuffer[ SMS_IOMANX_OFFSET   ], SMS_IOMANX_SIZE,   0,                    NULL      },
 { s_pFileXIO, &g_DataBuffer[ SMS_FILEXIO_OFFSET  ], SMS_FILEXIO_SIZE,  0,                    NULL      },
 { s_pPS2POff, &g_DataBuffer[ SMS_POWEROFF_OFFSET ], SMS_POWEROFF_SIZE, 0,                    NULL      },
 { s_pPS2Dev9, &g_DataBuffer[ SMS_PS2DEV9_OFFSET  ], SMS_PS2DEV9_SIZE,  0,                    NULL      },
 { s_pPS2ATAD, &g_DataBuffer[ SMS_PS2ATAD_OFFSET  ], SMS_PS2ATAD_SIZE,  0,                    NULL      },
 { s_pPS2HDD,  &g_DataBuffer[ SMS_PS2HDD_OFFSET   ], SMS_PS2HDD_SIZE,   sizeof ( s_HDDArgs ), s_HDDArgs },
 { s_pPS2FS,   &g_DataBuffer[ SMS_PS2FS_OFFSET    ], SMS_PS2FS_SIZE,    sizeof ( s_PFSArgs ), s_PFSArgs }
};

void ReadFolders(char* szFolder);


int main()
{
	SifInitRpc(0);


	//LoadHdd();

	fioInit();


	SMS_TimerInit();

	unsigned long lStart = g_Timer;

	init_scr();
	printf("starting...\n");
	ReadFolders("host:");
	
	//Test();

	//CloseHdd();

	lStart = g_Timer - lStart;

	printf("PAK took %ld ms\n",lStart);

	scr_printf("PAK took %ld ms\n",lStart);

	SMS_TimerDestroy();

	SleepThread();

	return 1;
}

void LoadHdd(void)
{

	SMS_IOPStartHDD ();
	 hddPreparePoweroff ();

	 //mount the partition
#ifdef DEBUG
	 printf("mointing partition..");
#endif
	 
	 int pd = fileXioMount("pfs0:" , "hdd0:+viDEOs" ,O_RDONLY ); // hdd0:+[PARTITION NAME]
	 if (pd == -1)
	 {
#ifdef DEBUG
		 printf("failed to moint hdd0:+viDEOs to pfs0:\n");
#endif
		 return;
	 }

#ifdef DEBUG
	 printf("mounted partition...\n");
#endif

	



}

void CloseHdd(void)
{

	 if (fileXioUmount("pfs0:") == -1)
	 {
#ifdef DEBUG
		 printf("failed to umount pfs0:\n");
#endif
	 }
}




void ReadFolders(char* szFolder)
{
	int hFolder;
	int hTempFile;
	fio_dirent_t fdFindData;
	char szNextFolder[0x200];
	int i;

	hFolder = fioDopen(szFolder);
	scr_printf("fioDopen(%s)\n", szFolder);
	if(hFolder <= 0)
		return;

	for(i = 0; i < 0x100; i ++)
	{
		if(fioDread(hFolder, &fdFindData) <= 0)
			break;
		else
		{
			scr_printf("fioDread(%s)\n", fdFindData.name);

			if(!stricmp(fdFindData.name, ".") || !stricmp(fdFindData.name, ".."))
				continue;

			sprintf(szNextFolder, "%s/%s", szFolder, fdFindData.name);

			//if(((fdFindData.stat.mode & FIO_S_IFDIR) || (fdFindData.stat.mode & 0x20)) && bParseSubFolders)
			if((hTempFile = fioOpen(szNextFolder, O_RDONLY)) <= 0)
				ReadFolders(szNextFolder);
			else
				fioClose(hTempFile);

		}
	}

	fioDclose(hFolder);
}




static void _load_module ( int anIndex ) {

 int  lRes;
 //char lBuff[ 128 ]; sprintf ( lBuff, STR_LOADING.m_pStr, s_LoadParams[ anIndex ].m_pName );

 //GUI_Status ( lBuff );

#ifdef DEBUG
 printf("loading %s\n",s_LoadParams[anIndex].m_pName);
#endif

 SifExecModuleBuffer (
  s_LoadParams[ anIndex ].m_pBuffer, s_LoadParams[ anIndex ].m_BufSize,
  s_LoadParams[ anIndex ].m_nArgs,   s_LoadParams[ anIndex ].m_pArgs, &lRes
 );
#ifdef DEBUG
 printf("done loading..\n");
#endif

 if ( anIndex == 4 && lRes >= 0 ) g_IOPFlags |= SMS_IOPF_DEV9;

}  /* end LoadModule */



int SMS_IOPStartHDD ( void ) {

// if ( g_IOPFlags & SMS_IOPF_DEV9 ) {

#ifdef DEBUG
	 printf("loading modules for hdd...\n");
#endif
  int i;

  for (  i = 0; i < 8; ++i  ) _load_module ( i );



#ifdef DEBUG
  printf("starting hdd checks...\n");
#endif
  if (hddCheckPresent() == 0)
  {
#ifdef DEBUG
	  printf("hdd present\n");
#endif
  }
  else
  {
#ifdef DEBUG
	  printf("hdd not present\n");
#endif
  }
  if ( !hddCheckFormatted ()  ) 
  {

	  g_IOPFlags |= SMS_IOPF_HDD;
  }
  else
  {
#ifdef DEBUG
	  printf("hdd checks failed...\n");
#endif
  }

// }  /* end if */

#ifdef DEBUG
  printf("finished loading modules for hdd ...\n");
#endif
 return g_IOPFlags & SMS_IOPF_HDD;

}  /* end SMS_IOPStartHDD */

