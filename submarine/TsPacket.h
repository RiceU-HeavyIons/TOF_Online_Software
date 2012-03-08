// $Id: TsPacket.h,v 1.3 2007/08/21 10:52:53 tofp Exp $
// Author: Kohei Kajimoto 2006/08/04

/*************************************************************************
 * Copyright (C) 2006, 2007, Kohei Kajimoto.                             *
 * All rights reserved.                                                  *
 *************************************************************************/

#ifndef TSPACKET_H_
#define TSPACKET_H_

#include <cstdio>

using namespace std;

#include <Riostream.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TStyle.h>

//#include "TsConfig.h"

class TsPacket {

 public:

  Short_t tdig;
  int out_tdc;
  int out_channel;


  UChar_t id;               // TSPacket ID
  UChar_t inter;            // Interpolation factor for LE
  UInt_t  edge_time;        // Time measurments in 100ps bins
  UChar_t tdc;              // Programmed ID of TDC
  UChar_t channel;          // TDC channel number LE(0-7)/TE(0-31)

 private:

  Short_t tdc_ch;//don't yet know how this is different from above

  //dylan
  int mrpc;
  int pad;

  UInt_t trig;
  UInt_t count;
  UInt_t token;
  UInt_t daq;
  UInt_t board_id;
  UInt_t num;

  UInt_t fTSPacket;

  /*
   * private functions
   */
  


  static Short_t leading(UChar_t tdc, UChar_t ch) {
    UChar_t tdig_no = tdc/4;
    UChar_t tdc_no = tdc - tdig_no;
    return (8*tdc_no + ch);
  }


  //outdated
  //  static Short_t trailing(UChar_t tdc, UChar_t ch) {
  //    UChar_t tdig_no = tdc/4;
  //    UChar_t tdc_no = tdig_no;
  //    return (24*tdc_no + ch);
  //  }

  Short_t tdc_map(Short_t tdc_ch);

  void decode(UInt_t pack);

  /**
   * public functions
   */
 public:

  TsPacket(UInt_t pack);
  TsPacket();
  ~TsPacket();
	
  //	static TsConfig *fConfig;
	
  //	static void SetConfig(TsConfig *conf) { fConfig = conf; };
	
  //	static const int MODE_NON  = 0;
  //	static const int MODE_TAMP = 1;
  //	static const int MODE_TINO = 2;
  //	static const int MODE_TAMP2 = 3;

  static const unsigned int ID_TDC_HEADER     = 0x02;  // 02
  static const unsigned int ID_LEADING_EDGE   = 0x04;  // 04
  static const unsigned int ID_TRAILING_EDGE  = 0x05;  // 05
  static const unsigned int ID_MULTIPLICITY   = 0x09;  // 09
  static const unsigned int ID_HEADER_TRIGGER = 0x0a;  // 10
  static const unsigned int ID_HEADER_DEBUG   = 0x0b;  // 11
  static const unsigned int ID_GEOGRAHICAL    = 0x0c;  // 12
  static const unsigned int ID_HEADER_TAG     = 0x0d;  // 13
  static const unsigned int ID_TDIG_SEPARATOR = 0x0e;  // 14
  static const unsigned int ID_RESERVED       = 0x0f;  // 15


  UChar_t GetID() { return id; }

  bool IsLE() { return (id == ID_LEADING_EDGE); }
  bool IsTE() { return (id == ID_TRAILING_EDGE); }
  bool IsSeparater() { return (id == ID_TDC_HEADER); }

  Short_t GetChannel() { return tdc_ch; }
  int GetIdChannel() { return (int) channel; }


  Short_t GetChannelTray4() { //this is what is called in analysis program!
    const Short_t vmap[3][8] = {
      // ch:  0   1   2   3   4   5   6   7 //
      { 3, 13,  8, 15, 10,  1,  6,  5}, // U2 //
      {17, 18,  2,  7, 20, 22,  0,  4}, // U3 //
      {23, 21, 16, 14,  9, 19, 12, 11}  // U4 //
    };
  
    /*
    //values below are for regular tray
    const Short_t vmap[3][8] = {
    // ch:  0   1   2   3   4   5   6   7 //
    { 2, 16,  9, 14,  7,  4, 11,  0}, // U2 //
    {12, 23,  3, 10, 21, 19,  5,  1}, // U3 //
    {18, 20, 13, 15,  8, 22, 17,  6}  // U4 //
    };
    */

    Short_t ux   = tdc & 0x3;
    tdig = tdc >> 2;
    int ch24=vmap[ux][channel];
    Short_t ch = 24*tdig + ch24;
    
    out_tdc=(int)tdc;
    out_channel=(int)channel;
    //these are set for the tof/submarine, not mtd
    
    
    return ch;
  }

  UInt_t GetEdgeTime();

  // for Separatro Word
  UInt_t GetBID() { return board_id; }
  UInt_t GetNumItems() { return num; }
  UInt_t GetCount() { return count; }

  void Print();
};
#endif
