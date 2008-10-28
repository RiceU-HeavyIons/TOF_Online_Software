#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <unistd.h>
#include <sys/time.h>
#include <time.h>


#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <glob.h>
#include <gtk/gtk.h>

#ifdef ROOT
#include <TCanvas.h>
#include <TStyle.h>
#include <TString.h>
#include <TLine.h>
#endif

using namespace std;

#include "libpcan.h"
#include "tofc.h"

#define TIMEOUT 4000000

int tofc::TOFC_DEBUG = 0;

#define PCAN_ERR(expr, h)                                          \
  do {                                                             \
    if ((errno = (expr)) != 0) {                                   \
      int status = (int)CAN_Status(h);                             \
      if (status > 0)                                              \
        fprintf(stderr, "CANbus error: 0x%x\n", status);           \
      else                                                         \
        fprintf(stderr, "System error: 0x%x\n", status);           \
    }                                                              \
  } while (0)

tofc::tofc()
{
}

tofc::tofc(WORD dev_id) {

  char *dev_path;

  TPDIAG tpdiag;
  char txt_buff[VERSIONSTRING_LEN];
  unsigned int i;

  WORD wBTR0BTR1 = CAN_BAUD_1M;         /* 250K, 500K */
  int nExtended  = CAN_INIT_TYPE_EX;    /* CAN_INIT_TYPE_ST */

  glob_t globb;

  globb.gl_offs = 0;
  glob(PCAN_DEVICE_PATTERN, GLOB_DOOFFS, NULL, &globb);

//  dl->dl_irq = (WORD*)malloc(globb.gl_pathc*sizeof(WORD));
//  dl->dl_path = (char**)malloc(globb.gl_pathc*sizeof(char *));

  HANDLE h = NULL;

  for(i = 0; i < globb.gl_pathc; i++) {
    if (TOFC_DEBUG) {
      printf("glob[%d] %s\n", i, globb.gl_pathv[i]);
    }
    dev_path = globb.gl_pathv[i];
    h = LINUX_CAN_Open(dev_path, O_RDWR | O_NONBLOCK);
    if (h == NULL) {
      if (TOFC_DEBUG) fprintf(stderr, "cannot open: %s\n", dev_path);
      continue;
    }
    LINUX_CAN_Statistics(h, &tpdiag);

    if (dev_id != tpdiag.wIrqLevel) {
      PCAN_ERR(CAN_Close(h), h);
      h = NULL;
      continue;
    }
//    dl->dl_irq[found] = tpdiag.wIrqLevel;
//    dl->dl_path[found] = strdup(dev_path);

    if (TOFC_DEBUG) {
      PCAN_ERR(CAN_Init(h, wBTR0BTR1, nExtended), h);
      PCAN_ERR(CAN_VersionInfo(h, txt_buff), h);
      printf("handle:   %x\n", (int)h);
      printf("dev_id:   %x\n", dev_id);
      printf("dev_name: %s\n", dev_path);
      printf("version_info: %s\n", txt_buff);
    }
    i = globb.gl_pathc;
  }

  globfree(&globb);
  handle = h;
}

tofc::tofc(const char *dname)
{
  HANDLE h;
  h = LINUX_CAN_Open((char*)dname, O_RDWR | O_NONBLOCK);
  if(h) {
    PCAN_ERR(CAN_Init(h,  CAN_BAUD_1M, CAN_INIT_TYPE_EX), h);

    if(TOFC_DEBUG) {
      TPDIAG tpdiag;
      char verstr[VERSIONSTRING_LEN];

      LINUX_CAN_Statistics(h, &tpdiag);
      PCAN_ERR(CAN_VersionInfo(h, verstr), h);
      printf("handle    : %x\n", (int)h);
      printf("irq level : %x\n", tpdiag.wIrqLevel);
      printf("device    : %s\n", dname);
      printf("version   : %s\n", verstr);
    }
  }
  handle = h;
}

tofc::~tofc()
{
}

/*********************************************************************/
/* private functions                                                 */

int errfunc(const char *epath, int eerrno) {
  fprintf(stderr, "%s: %d\n", epath, eerrno);
  return eerrno;
}

HANDLE tofc::pcan_dev_open(const char *dname)
{
  HANDLE h;
  h = LINUX_CAN_Open((char*)dname, O_RDWR | O_NONBLOCK);
  if(h) {
    PCAN_ERR(CAN_Init(h,  CAN_BAUD_1M, CAN_INIT_TYPE_EX), h);

    if(TOFC_DEBUG) {
      TPDIAG tpdiag;
      char verstr[VERSIONSTRING_LEN];

      LINUX_CAN_Statistics(h, &tpdiag);
      PCAN_ERR(CAN_VersionInfo(h, verstr), h);
      printf("handle    : %x\n", (int)h);
      printf("irq level : %x\n", tpdiag.wIrqLevel);
      printf("device    : %s\n", dname);
      printf("version   : %s\n", verstr);
    }
  }

  return h;
}

/*********************************************************************/
int tofc::pcan_dev_scan(devlist *dl)
{
  int found;

  WORD dev_id;
  char *dev_path;

  TPDIAG tpdiag;
  char txt_buff[VERSIONSTRING_LEN];
  unsigned int i;

  WORD wBTR0BTR1 = CAN_BAUD_1M;         /* 250K, 500K */
  int nExtended  = CAN_INIT_TYPE_EX;    /* CAN_INIT_TYPE_ST */

  glob_t globb;

  globb.gl_offs = 0;
  glob(PCAN_DEVICE_PATTERN, GLOB_DOOFFS, NULL, &globb);

  dl->dl_irq = (WORD*)malloc(globb.gl_pathc*sizeof(WORD));
  dl->dl_path = (char**)malloc(globb.gl_pathc*sizeof(char *));

  found = 0;
  for(i = 0; i < globb.gl_pathc; i++) {
    if (TOFC_DEBUG) {
      printf("glob[%d] %s\n", i, globb.gl_pathv[i]);
    }
    dev_path = globb.gl_pathv[i];
    HANDLE h = LINUX_CAN_Open(dev_path, O_RDWR | O_NONBLOCK);
    if(h == NULL) {
      if (TOFC_DEBUG) fprintf(stderr, "cannot open: %s\n", dev_path);
      continue;
    }
    LINUX_CAN_Statistics(h, &tpdiag);
    dev_id = tpdiag.wIrqLevel;
    dl->dl_irq[found] = tpdiag.wIrqLevel;
    dl->dl_path[found] = strdup(dev_path);


    if (TOFC_DEBUG) {
      PCAN_ERR(CAN_Init(h, wBTR0BTR1, nExtended), h);
      PCAN_ERR(CAN_VersionInfo(h, txt_buff), h);
      printf("handle:   %x\n", (int)h);
      printf("dev_id:   %x\n", dev_id);
      printf("dev_name: %s\n", dev_path);
      printf("version_info: %s\n", txt_buff);
    }
    PCAN_ERR(CAN_Close(h), h);
    found++;
  }
  dl->dl_len = found;

  globfree(&globb);
  return found;
}

/*********************************************************************/
static void set_msg(TPCANMsg *msg, ...)
{
  int i;
  va_list argptr;

  va_start(argptr, msg);

  msg->ID      = va_arg(argptr, int);
  msg->MSGTYPE = va_arg(argptr, int);
  msg->LEN     = va_arg(argptr, int);
  for(i=0; i < msg->LEN && i < 8; i++)
    msg->DATA[i] = va_arg(argptr, int);

  va_end(argptr);
}

/*********************************************************************/
void tofc::pcan_print_msg(TPCANMsg *msg)
{
  int i;

  printf("CANMsg ID: 0x%x DATA[%d]: ", msg->ID, msg->LEN);
  for(i = 0; i < msg->LEN && i < 8; i++)
    printf("0x%02x ", msg->DATA[i]);
  puts("");
}

/*********************************************************************/
void tofc::pcan_print_rdmsg(TPCANRdMsg *rmsg)
{
  int i;
  TPCANMsg *msg = &rmsg->Msg;

  printf("CANMsg ID: 0x%x DATA[%d]: ", msg->ID, msg->LEN);
  for(i = 0; i < msg->LEN && i < 8; i++)
    printf("0x%02x ", msg->DATA[i]);
  puts("");
}

/*********************************************************************/
void tofc::pcan_diag(HANDLE h)
{
  TPCANMsg msg;
  TPCANRdMsg rmsg;
  int tcpu_cid = 0x20;

  set_msg(&msg, (tcpu_cid << 4) | 0x4, MSGTYPE_STANDARD, 1, 0xb1);

  if(TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }

  PCAN_ERR(CAN_Write(h, &msg), h);
  //PCAN_ERR(CAN_Read (h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
//  printf("dwTime %d\n", rmsg.dwTime);

  if(TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }

  printf("Firmware version:\n");

  printf("  TCPU 0x%x: %x%c/%x\n", tcpu_cid,
	 rmsg.Msg.DATA[2], rmsg.Msg.DATA[1], rmsg.Msg.DATA[3]);

  // TDIGs
  for (int tdig = 0x10; tdig < 0x18; tdig++) {
    int id = (((tdig << 4) | 0x4) << 18) + tcpu_cid;
    set_msg(&msg, id, MSGTYPE_EXTENDED, 1, 0xb1);

    if(TOFC_DEBUG) {
      printf(">> ");
      tofc::pcan_print_msg(&msg);
    }

    PCAN_ERR(CAN_Write(h, &msg), h);
    PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);


    if(TOFC_DEBUG) {
      printf("<< ");
      tofc::pcan_print_rdmsg(&rmsg);
    }

    printf("  TDIG 0x%x: %x%c/%x\n", tdig,
	   rmsg.Msg.DATA[2], rmsg.Msg.DATA[1], rmsg.Msg.DATA[3]);
  }

}

/***
 * TDIG
 */

/*********************************************************************/
unsigned long long
tofc::pcan_tcpu_get_chip_sn(
    HANDLE h,
    WORD tcpu)
{
  TPCANMsg msg;
  TPCANRdMsg rmsg;

  unsigned long long ret = 0;

  set_msg(&msg, (tcpu << 4) | 4, MSGTYPE_STANDARD, 1, 0xb2);

  if(TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }

  PCAN_ERR(CAN_Write(h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, 20000), h);

  if(TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }
  for(int i = 7; i > 0; i--) {
    ret = (ret << 8) | rmsg.Msg.DATA[i];
  }
  return (ret);
}

/***
 * TDIG
 */

/*********************************************************************/
void
tofc::pcan_tdig_set_threshold(
    HANDLE h,
    WORD tcpu,
    WORD tdig,
    double threshold)
{

  TPCANMsg msg;
  TPCANRdMsg rmsg;
  unsigned val;

  val = (unsigned int)(threshold * 4095.0 / 3300.0 + 0.5);

  msg.ID      = (((tdig << 4) | 0x2) << 18) + tcpu;
  msg.MSGTYPE = MSGTYPE_EXTENDED;
  msg.LEN     = 3;
  msg.DATA[0] = 0x08;
  msg.DATA[1] = (val & 0xff);
  msg.DATA[2] = ((val >> 8) & 0x0f);

  if (TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }

  PCAN_ERR(CAN_Write(h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
//  printf("dwTime %d\n", rmsg.dwTime);

  if (TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }
}

/*********************************************************************/
double
tofc::pcan_tdig_read_temp(
    HANDLE h,
    WORD tcpu,
    WORD tdig)
{

  TPCANMsg msg;
  TPCANRdMsg rmsg;
  double temp;
  int temph;
  int templ;

  msg.ID      = (((tdig << 4) | 0x4) << 18) + tcpu;
  msg.MSGTYPE = MSGTYPE_EXTENDED;
  msg.LEN     = 1;
  msg.DATA[0] = 0x09;

  if(TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }

  PCAN_ERR(CAN_Write(h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
//  printf("dwTime %d\n", rmsg.dwTime);
  temph = rmsg.Msg.DATA[2];
  templ = rmsg.Msg.DATA[1];

  temp = (double)(temph) + (double)(rmsg.Msg.DATA[1])/100.0;

  if(TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }

  return temp;
}
/***
 * TCPU
 */

/*********************************************************************/
void tofc::pcan_tcpu_scan()
{
  int i;
  int tid;

  for (i = 0; i < TCPU_SCAN_MAX; i++) {
    tid = get_tray_id(i + 0x20);
    if(tid > 0) {
      printf("Tray %d is found at TCPU 0x%x\n", tray_sn_list[tid], i);
      tcpu_list[i] = tray_sn_list[tid];
    } else {
      tcpu_list[i] = tid;
    }
  }
}

/*********************************************************************/
void tofc::pcan_tcpu_reset(HANDLE h, WORD tcpu) {
  TPCANMsg msg;
  TPCANRdMsg rmsg;
  DWORD id = (tcpu << 4) | 0x2;

  set_msg(&msg, id, MSGTYPE_STANDARD, 3, 0xe, 0xe, 0x0);

  if (TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }

  PCAN_ERR(CAN_Write(h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);

  if (TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }

  set_msg(&msg, id, MSGTYPE_STANDARD, 5, 0xe, 0x1, 0x3, 0x1, 0x0);

  if (TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }

  PCAN_ERR(CAN_Write(h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);

  if (TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }

  while(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT) == 0) {
    if (TOFC_DEBUG) {
      printf("<< ");
      tofc::pcan_print_rdmsg(&rmsg);
    }
  }
}

/*********************************************************************/
void tofc::pcan_config(
    HANDLE h, WORD tcpu, WORD tdig, WORD tdc, config *cfg)
{
  unsigned int i, j;
  char *ptr;
  unsigned int byts, csum;
  TPCANMsg msg;
  TPCANRdMsg rmsg;

  msg.ID      = (((tdig << 4) | 0x2) << 18) + tcpu;
  msg.MSGTYPE = MSGTYPE_EXTENDED;

  /* power off tdc*/
  if (0) {
    msg.LEN = 6;
    msg.DATA[0] = 0x4 | (tdc & 0x3);
    for (i = 1; i < 6; i++)
      msg.DATA[i] = 0x00;
    if(TOFC_DEBUG) {
      printf(">> ");
      tofc::pcan_print_msg(&msg);
    }
    PCAN_ERR(CAN_Write(h, &msg), h);
    PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
    if (TOFC_DEBUG) {
      printf("<< ");
      tofc::pcan_print_rdmsg(&rmsg);
    }
  }

  /* block start */
  msg.LEN     = 1;
  msg.DATA[0] = 0x10;
  if(TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }
  PCAN_ERR(CAN_Write(h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
  if(TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }

  /* send block data */
  for (i = 0; i < cfg->len / 7; i++) {
    msg.LEN = 8;
    msg.DATA[0] = 0x20;
    ptr = &cfg->data[7*i];
    for (j = 0; j < 7; j++)
      msg.DATA[j+1] = ptr[j];
    if(TOFC_DEBUG) {
      printf(">> ");
      tofc::pcan_print_msg(&msg);
    }
    PCAN_ERR(CAN_Write(h, &msg), h);
    PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
    if(TOFC_DEBUG) {
      printf("<< ");
      tofc::pcan_print_rdmsg(&rmsg);
    }
  }

  /* send remainder */
  if (msg.LEN = cfg->len % 7) {
    msg.DATA[0] = 0x20;
    ptr = &cfg->data[7*(cfg->len / 7)];
    for (j = 0; j < msg.LEN; j++)
      msg.DATA[j+1] = ptr[j];
    msg.LEN++;
    if(TOFC_DEBUG) {
      printf(">> ");
      tofc::pcan_print_msg(&msg);
    }
    PCAN_ERR(CAN_Write(h, &msg), h);
    PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
    if(TOFC_DEBUG) {
      printf("<< ");
      tofc::pcan_print_rdmsg(&rmsg);
    }
  }

  /* block end */
  msg.LEN     = 1;
  msg.DATA[0] = 0x30;
  if(TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }
  PCAN_ERR(CAN_Write(h, &msg), h);
  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
  if(TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }

  byts = rmsg.Msg.DATA[2] + (rmsg.Msg.DATA[3] << 8);
  csum = rmsg.Msg.DATA[4] + (rmsg.Msg.DATA[5] << 8);

  if (byts != cfg->len || csum != cfg->check_sum) {
    fprintf(stderr, "block data error!\n");
  } else {
    /* destination */
    msg.LEN     = 1;
/*    msg.DATA[0] = 0x40 | (tdc & 0x3); */
    msg.DATA[0] = 0x44 | (tdc & 0x3); /* program memory */
    if (TOFC_DEBUG) {
      printf(">> ");
      tofc::pcan_print_msg(&msg);
    }
    PCAN_ERR(CAN_Write(h, &msg), h);
    PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, 1000000), h);
    if(TOFC_DEBUG) {
      printf("<< ");
      tofc::pcan_print_rdmsg(&rmsg);
    }
  }
}

/*********************************************************************/
/*
 * This portion of code is taken from TsPacket.h
 */
static int get_ch(BYTE data[])
{

    int tdc, ch;
    int tg, ux;

    tdc = (data[3] & 0x0f);
    ch  = (data[2] & 0xe0) >> 5;

    tg = tdc >> 2;
    ux = (tdc & 0x03);

    static const int vmap[3][8] = {
  /* ch:  0   1   2   3   4   5   6   7 */
        { 2, 16,  9, 14,  7,  4, 11,  0}, /* U2 */
        {12, 23,  3, 10, 21, 19,  5,  1}, /* U3 */
        {18, 20, 13, 15,  8, 22, 17,  6}  /* U4 */
    };
    return (24*tg + vmap[ux][ch]);
}

/*********************************************************************/
static int get_id(BYTE data[]) {
  return (data[3] >> 4);
}

/*********************************************************************/
static int get_half(BYTE data[]) {
  return (data[0] & 0x1);
}

/*********************************************************************/
static int get_bid(BYTE data[])
{
  return (data[3] & 0x0f);
}


/*********************************************************************/
int tofc::get_position(int tsn) {

  int position[] = {
//    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
      0,   0,   0,   0,   0,   0,   0,   2,  11,  16,
     17,  23,  15,  20,  22,  28,  21,   6,  30,   5,
     12,   8,   0,  26,  29,   3,   0,  14,   1,  18,
      9,  13,  27,   4,  19,  10,  24,   7,   0,  25
   };

  return position[tsn];
}

int tofc::get_gas_line(int tsn)
{
  int ps;

  ps = get_position(tsn);
  if(ps == 0) {
    return 0;
  } else if(ps <= 10) {
    return 1 + (ps - 1) / 5;
  } else {
    return 3 + (ps - 11) % 4;
  }
}

/*********************************************************************/
int tofc::get_dist_box(int tsn)
{
  int ps;

  ps = get_position(tsn);
  if(ps == 0) {
    return 0;
  } else if(ps <= 10) {
    return 1 + (ps - 1) / 10;
  } else {
    return 2 + ((ps - 11) % 4 / 2);
  }
}

/*********************************************************************/
void tofc::load_tray_db(const char *fname)
{
  ifstream in(fname);

  unsigned int id, sn;
  unsigned long long chip_sn;
  while(in >> dec >> id >> sn >> hex >> chip_sn) {
    tray_chip_list[id] = chip_sn;
    tray_sn_list[id] = sn;
  }
}

/*********************************************************************/
int tofc::get_tray_id(int tcpu)
{
  int i;

  unsigned long long chsn = tofc::pcan_tcpu_get_chip_sn(handle, tcpu);
  for (i = 0; i < TRAY_MAX; i++) {
    if (tray_chip_list[i] == chsn) break;
  }

  if(i == TRAY_MAX) i = -1;

  return i;
}



#ifdef ROOT
void tofc::pcan_monitor_wrapper(gpointer data, gint src, GdkInputCondition cond)
{
  tofc *tc = (tofc *)data;
  tc->pcan_monitor((gpointer)tc->handle, src, cond);
}

/*********************************************************************/
void tofc::pcan_monitor(gpointer data, gint src, GdkInputCondition cond)
{
  HANDLE h;
  TPCANRdMsg rmsg;
  int i, j;
  int id, ch;
  BYTE *ptr;

  static int geo = 1;

  h = (HANDLE)data;

  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
  if (TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }
  if (rmsg.Msg.ID == 0x201) {
    for(i = 0; i < rmsg.Msg.LEN / 4; i++) {
      ptr = &rmsg.Msg.DATA[4*i];
      fprintf(fp, "0x%08x 0x%02x%02x%02x%02x\n",
          rmsg.Msg.ID, *(ptr+3), *(ptr+2), *(ptr+1), *ptr);

      id = get_id(ptr);
      hpid->Fill(id);
      if (id == 0x2) {
      } else if(id == 0x4) {
        ch = get_ch(ptr) + TRAY_CH_MAX/2*geo;
        ech[ch]++;
      } else if(id == 0xc) {
        geo = get_half(ptr);
      } else if(id == 0xe && get_bid(ptr) == 0) { /* end of event */
        nevent++; /* increment number of events */

        /* correlation */
        for(i = 0; i <  TRAY_CH_MAX; i++) {
          if (ech[i] > 0) {
            hch->Fill(i);
            hnr->Fill(i / 6, i % 6);
            for(j = 0; j < TRAY_CH_MAX; j++) {
              if(ech[j] > 0) hcorr->Fill(i, j);
            }
          }
        }

        /* clear buffer */
        for (i = 0; i < TRAY_CH_MAX; i++) ech[i] = 0;

        if (nevent % 1000 == 0) {
          char strtm[64];
          time_t now;
          double run_time = 25.0e-6*nevent;
          time(&now);
          strftime(strtm, 64, "%Y-%m-%d %H:%M:%S", localtime(&now));
          for (i = 0; i < TRAY_CH_MAX; i++) {
            if (i % 6 == 0) printf("%3d: ", i);
            printf(" %7.2f", hch->GetBinContent(i+1)/run_time);
            if (i % 6 == 5) printf("\n");
          }
          printf("%d events -- %s (%d sec)\n", nevent, strtm, now - tm);
        }
      }
    }
  }
}


/*********************************************************************/
gint tofc::pcan_noiserun(HANDLE h, int uid, int thr)
{
  int tcpu = 0x20;
  TPCANMsg msg;
  TPCANRdMsg rmsg;
  DWORD id;
  gint fd, tag;
  int i;
  char strtm[64];

  this->user_id = uid;
  this->threshold = thr;
  this->handle = h;

  tray_id = get_tray_id(tcpu);
  if (tray_id < 0) {
    fprintf(stderr, "cannot find tray id\n");
    return NULL;
  }
  tray_sn = tray_sn_list[tray_id];

  printf("setting threshold: %d...", thr);
  for(i = 0; i < 8; i++)
    tofc::pcan_tdig_set_threshold(handle, 0x20, 0x10 + i, thr);
  printf("done\n");

  id = (tcpu << 4) | 0x2;

  set_msg(&msg, id, MSGTYPE_STANDARD, 3, 0xe, 0x2, 0x1);
  if (TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }
  PCAN_ERR(CAN_Write(handle, &msg), handle);
  PCAN_ERR(LINUX_CAN_Read_Timeout(handle, &rmsg, TIMEOUT), handle);

  time(&tm);
  strftime(strtm, 64, "%Y-%m-%d %H:%M:%S", localtime(&tm));

  strftime(fbase, 64, "%Y%m%d_%H%M%S", localtime(&tm));
  sprintf(dbase, "/home/data/wah");
  sprintf(dname, "%s/%s", dbase, fbase);
  sprintf(fname, "%s.data", dname);
  sprintf(rname, "%s.root", dname);
  printf("start noise run for tray %d -- %s\n", tray_sn, strtm);

  mkdir(dname, 0755);

  fp = fopen(fname, "w+");

  /* initialize histograms */
  nevent = 0;
  for(i = 0; i < TRAY_CH_MAX; i++) {
    ech[i] = 0;
  }

  rootf = new TFile(rname, "create");
  hch = new TH1F("hch", "noise rates (Hz)", TRAY_CH_MAX, 0, TRAY_CH_MAX);
  hnr = new TH2F("hnr", "noise rates (Hz)", 32, 0, 32, 6, 0, 6);
  hpid = new TH1F("hpid", "packet id", 16, 0, 16);
  hcorr = new TH2F("hcorr", "channel correlation",
        TRAY_CH_MAX, 0, TRAY_CH_MAX, TRAY_CH_MAX, 0, TRAY_CH_MAX);

  fd = LINUX_CAN_FileHandle(handle);
  tag = gdk_input_add(fd, GDK_INPUT_READ,
                      tofc::pcan_monitor_wrapper, (gpointer)this);

  set_msg(&msg, id, MSGTYPE_STANDARD, 3, 0xe, 0xe, 0x8);
  if (TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }
  PCAN_ERR(CAN_Write(handle, &msg), handle);
  PCAN_ERR(LINUX_CAN_Read_Timeout(handle, &rmsg, TIMEOUT), handle);

  return tag;
}

/*********************************************************************/
void
tofc::pcan_nstop(HANDLE h, gint tag)
{
  tcpu = 0x20;

  TPCANMsg msg;
  TPCANRdMsg rmsg;
  DWORD id;

  FILE *yaml;
  char yname[128];
  char srt[64];
  char stp[64];
  time_t stm;
  char cmd[256];

  int i;
  double run_time = 25.0e-6*nevent;

  id = (tcpu << 4) | 0x2;

  set_msg(&msg, id, MSGTYPE_STANDARD, 3, 0xe, 0xe, 0x0);
  if (TOFC_DEBUG) {
    printf(">> ");
    tofc::pcan_print_msg(&msg);
  }
  PCAN_ERR(CAN_Write(h, &msg), h);
  sleep(2);

  PCAN_ERR(LINUX_CAN_Read_Timeout(h, &rmsg, TIMEOUT), h);
  if(TOFC_DEBUG) {
    printf("<< ");
    tofc::pcan_print_rdmsg(&rmsg);
  }

  time(&stm);

  for(i = 0; i < TRAY_CH_MAX; i++) {
    if(i % 6 == 0) printf("%3d: ", i);
    printf(" %7.2f", hch->GetBinContent(i+1)/run_time);
    if(i % 6 == 5) printf("\n");
  }

  strftime(srt, 64, "%Y-%m-%d %H:%M:%S", localtime(&tm));
  strftime(stp, 64, "%Y-%m-%d %H:%M:%S", localtime(&stm));

  sprintf(yname, "%s/report.yaml", dname);
  yaml = fopen(yname, "w+");

  fprintf(yaml, "---\n");
  fprintf(yaml, ":tray_id: %d\n", tray_id);
  fprintf(yaml, ":tray_sn: %d\n", tray_sn);
  fprintf(yaml, ":user_id: %d\n", user_id);
  fprintf(yaml, ":start: %s\n", srt);
  fprintf(yaml, ":stop: %s\n", stp);
  fprintf(yaml, ":run_time: %d\n", stm - tm);
  fprintf(yaml, ":events: %d\n", nevent);
  fprintf(yaml, ":data_uri: %s%s\n", URI_BASE, fname);
  fprintf(yaml, ":note: ''\n");

// run config
//  int dist_box = get_dist_box(tray_sn);
//  int gas_line = get_gas_line(tray_sn);
  int dist_box = 1;
  int gas_line = 1;

  fprintf(yaml, ":n_modules: %d\n", 32);
  fprintf(yaml, ":n_pads: %d\n", 6);
  fprintf(yaml, ":threshold: %d\n", threshold);
  fprintf(yaml, ":hv_plus: %d\n", 7000);
  fprintf(yaml, ":hv_minus: %d\n", 7000);
  fprintf(yaml, ":flow_rate1: %d\n", 0);
  fprintf(yaml, ":flow_rate2: %d\n", 0);
  fprintf(yaml, ":flow_rate3: %d\n", 0);
  fprintf(yaml, ":note: 'final test for tray %03d'\n", tray_sn);
  fprintf(yaml, ":run_config_type_id: %d\n", 4); /* single tray test */
  fprintf(yaml, ":hv_channel_plus: %d\n",  2*dist_box + 99);
  fprintf(yaml, ":hv_channel_minus: %d\n", 2*dist_box + 98);
  fprintf(yaml, ":gas_channel_1: %d\n", gas_line + 99);
  fprintf(yaml, ":gas_channel_2: %d\n", 0);
  fprintf(yaml, ":gas_channel_3: %d\n", 0);
  fprintf(yaml, ":location_id: %d\n", 4); /* BNL WAH */

  fprintf(yaml, ":nr:\n");
  for (i = 0; i < TRAY_CH_MAX; i++) {
    fprintf(yaml, "- %7.5f\n", hch->GetBinContent(i+1)/run_time);
  }
  fclose(yaml);


  fclose(fp);

  TH1F *hsing = (TH1F*)hch->Clone();
  hsing->Scale(1.0/nevent);
  hcorr->Scale(1.0/nevent);
  for(int x=0; x < TRAY_CH_MAX; x++) {
    double avx = hsing->GetBinContent(x+1);
    for(int y=0; y < TRAY_CH_MAX; y++) {
      double avy = hsing->GetBinContent(y+1);
      double xy = hcorr->GetBinContent(x+1, y+1);
      double sigma = avx*(1.0-avx)*avy*(1.0-avy);
      double corr = (sigma > 0.0)? (xy - avx*avy) / sqrt(sigma) : 0.0;
      hcorr->SetBinContent(x+1, y+1, corr);
    }
  }
  hsing->Delete();

  hch->Scale(1.0/run_time);
  hnr->Scale(1.0/run_time);

  make_plots();

  hch->Write();
  hnr->Write();
  hpid->Write();
  rootf->Close();

  chdir(dbase);
  sprintf(cmd, "tar zcf %s.tgz %s", fbase, fbase);
  system(cmd);
  printf("%d events -- saved in %s.tgz\n", nevent, fbase);

  gdk_input_remove(tag);
}

void tofc::make_plots()
{
  int winw = 600+4;
  int winh = 800+28;

  gStyle->SetOptStat(0);
  TCanvas *cdummy = new TCanvas("cdummy", "cdummy", winw, winh);

  TString pdfo(dname);
  pdfo += "/report.ps";

  cdummy->Print(pdfo + "[");

//-----------------------------------------------------------------------------
  gStyle->SetPalette(1, 0);
  hnr->Draw("colz");
  cdummy->Print(pdfo);

  hch->Draw();
  cdummy->Print(pdfo);

  hpid->Draw();
  cdummy->Print(pdfo);

  gStyle->SetPalette(1, 0);
  gStyle->SetOptStat(0);
  hcorr->Draw("colz");
  int n = TRAY_CH_MAX;
  for(int i=0; i < n; ++i) {
    if(i % 24 == 0){
      gStyle->SetLineColor(kBlack);
      (new TLine(i, 0, i, n))->Draw();
      (new TLine(0, i, n, i))->Draw();
    }
  }
  cdummy->Print(pdfo);

  cdummy->Print(pdfo + "]");
  chdir(dname);
  char cmd[256];
  sprintf(cmd, "ps2pdf %s", pdfo.Data());
  system(cmd);

  cdummy->Close();
  //-----------------------------------------------------------------------------
  gStyle->SetOptStat(0);
  TCanvas *canv = new TCanvas("canv", "canv", winw, winh);
  canv->Divide(2, 2);

  canv->cd(1);
  gStyle->SetPalette(1, 0);
  hnr->Draw("colz");

  canv->cd(2);
  hch->Draw();

  canv->cd(3);
  hpid->Draw();

  canv->cd(4);
  gStyle->SetPalette(1, 0);
  hcorr->Draw("colz");
  for(int i=0; i < n; ++i) {
    if(i % 24 == 0) {
      gStyle->SetLineColor(kBlack);
      (new TLine(i, 0, i, n))->Draw();
      (new TLine(0, i, n, i))->Draw();
    }
  }

  char summary[256];
  sprintf(summary, "%s/summary.gif", dname);
  canv->Print(summary);
  canv->Close();
}

#endif
