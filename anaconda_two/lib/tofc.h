/* $Id$
 * Kohei Kajimoto, 2008/05/08
 */
#ifndef TOFC_H_
#define TOFC_H_

#include <stdio.h>
#include <sys/types.h>

#ifdef ROOT
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#endif

#include "libpcan.h"

#define PCAN_DEVICE_PATTERN      "/dev/pcan*"
#define TRAY_CH_MAX 192
#define TRAY_MAX  130
#define TCPU_SCAN_MAX 20

#define URI_BASE "file://tofcontrol.starp.bnl.gov"
typedef struct {
  size_t    dl_len;
  char    **dl_path;
  WORD     *dl_irq;
} devlist;

typedef struct {
  char      mcu_ver[4];
  char      fgpa_ver[4];
  unsigned int board_id;
} status;

typedef struct {
  char         data[128];
  unsigned int len;
  unsigned int check_sum;
} config;


class tofc
{

private:

#ifdef ROOT
  TFile *rootf;
  TH1F *hch;
  TH2F *hnr;
  TH1F *hpid;
  TH2F *hcorr;
#endif

  int ech[TRAY_CH_MAX];
  int nevent;
  time_t tm;
  FILE *fp;


  char fname[128];
  char fbase[128];
  char dname[128];
  char dbase[128];
  char rname[128];

  HANDLE handle;

  int tcpu_list[TCPU_SCAN_MAX];

  unsigned long long tray_chip_list[TRAY_MAX];
  unsigned int tray_sn_list[TRAY_MAX];
  int user_id;
  int tray_id;
  int tray_sn;
  int threshold;

  int tcpu;

  void make_plots();
public:
  tofc();
  tofc(WORD dev_id);
  tofc(const char *);
  virtual ~tofc();

  HANDLE get_handle() { return handle; }

  static int TOFC_DEBUG;


  HANDLE set_handle(HANDLE h) { handle = h; return h; }

  static HANDLE pcan_dev_open(const char *dname);
  static int pcan_dev_scan(devlist *dl);

  static void pcan_diag(HANDLE h);

  static int get_position(int tsn);
  static int get_gas_line(int tsn);
  static int get_dist_box(int tsn);

  unsigned long long tofc::pcan_tcpu_get_chip_sn(HANDLE h, WORD tcpu);

  static double pcan_tdig_read_temp(HANDLE h, WORD tcpu, WORD tdig);
  static void pcan_config(HANDLE h, WORD tcpu, WORD tdig, WORD tdc, config *cfg);

  static void pcan_print_msg(TPCANMsg *msg);
  static void pcan_print_rdmsg(TPCANRdMsg *rmsg);

  static void pcan_tcpu_reset(HANDLE h, WORD tcpu);
  void pcan_tcpu_scan();
  int  get_tcpu(int i) { return tcpu_list[i]; }

  static void pcan_tdig_set_threshold(HANDLE h, WORD tcpu, WORD tdig, double threshold);

  void load_tray_db(const char *fname);
  int get_tray_id(int tcpu);

#ifdef ROOT
  gint pcan_noiserun(HANDLE h, int uid, int thr);
  void pcan_nstop(HANDLE h, gint tag);
  void pcan_monitor(gpointer data, gint src, GdkInputCondition cond);
  static void pcan_monitor_wrapper(gpointer data, gint src, GdkInputCondition cond);
#endif

};
#endif /*TOFC_H_*/
