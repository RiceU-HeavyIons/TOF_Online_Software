// $Id: TsPacket.cpp,v 1.4 2007/08/21 10:52:53 tofp Exp $
// Author: Kohei Kajimoto 2006/08/04

/*************************************************************************
 * Copyright (C) 2006, 2007, Kohei Kajimoto.                             *
 * All rights reserved.                                                  *
 *************************************************************************/

#include "TsPacket.h"

//TsConfig *TsPacket::fConfig = NULL;
 
/**
 * public functions
 */


TsPacket::TsPacket()
{
  cout<<"do nothing."<<endl;

}
 
TsPacket::TsPacket(unsigned int pack)
{
  decode(pack);
}

TsPacket::~TsPacket()
{
  // do nothing
}

UInt_t TsPacket::GetEdgeTime() {

  UInt_t r;

  if (id == ID_LEADING_EDGE) {
    r = (edge_time << 2) + inter;

    //    offset subtraction for ts20051114
    //    	if (tdc_ch >= 51 && tdc_ch <= 53) {
    //    		r -= 39950;
    //    	}
    return r;
  }
  if (id == ID_TRAILING_EDGE)
    return (edge_time << 2);
      
  return 0;
}

void TsPacket::Print() {
  if (id == 0x02) { cout<<"tdc header"<<endl;}
  if (id == 0x04) { cout<<"leading edge"<<endl;}
  if (id == 0x05) { cout<<"trailing edge"<<endl;}
  if (id == 0x09) { cout<<"multiplicity"<<endl;}
  if (id == 0x0a) { cout<<"header trigger"<<endl;}
  if (id == 0x0b) { cout<<"header debug"<<endl;}
  if (id == 0x0c) { cout<<"geographical"<<endl;}
  if (id == 0x0d) { cout<<"header tag"<<endl;}
  if (id == 0x0e) { cout<<"tdig seperator"<<endl;}
  if (id == 0x0f) { cout<<"reserved id"<<endl;}

  cout << hex;
  //   cout << "TSPacket id: ";
  // cout.fill('0');
  // cout.width(2);
  // cout << (short)id<<endl;

  if (id == 0x04) {
    cout << " tdc: ";
    cout.width(2);
    cout << (short)tdc<<endl;
    cout << " ch: ";
    cout.width(2);
    cout << (short)channel<<endl;
    cout << " val: ";
    cout.width(6);
    cout << edge_time<<endl;
    cout << "(";
    cout.fill('0');
    cout.width(2);
    cout << (short)inter<<endl;
    cout << ")";

  } else if (id == 0x05) {
    cout << " tdc: " << setw(2) << (short)tdc<<endl;
    cout << " ch: " << setw(2) << (short)channel<<endl;
    cout << " val: " << setw(6) << edge_time<<endl;

  } else if (id == 0x0a) {
    cout << " trig: " << trig<<endl;
    cout << " daq: " << daq<<endl;
    cout << " token: " << token<<endl;

  } else if (id == 0x0b) {
    cout << " header debug data: ";
    cout << fTSPacket<<endl;

  } else if (id == 0x0e) {
    cout << " board id: " << board_id<<endl;
    cout << " num: " << num<<endl;
    cout << " count: " << count<<endl;
  } else {
    cout << " unknown id";
  }
  cout << endl;
}

/*
 * private functions
 */
void TsPacket::decode(UInt_t pack) {

  fTSPacket = pack;	// save raw packet
  id = (fTSPacket & 0xF0000000) >> 28; // extract id

  if ( id == ID_LEADING_EDGE || id== ID_TRAILING_EDGE) { //  leading edge
    tdc       = (fTSPacket & 0x0F000000) >> 24;
    channel   = (fTSPacket & 0x00E00000) >> 21;

    inter     = (fTSPacket & 0x00180000) >> 19;
    edge_time = (fTSPacket & 0x0007FFFF);
    tdc_ch = tdc_map( leading(tdc, channel) );

  } 
  else if ( id == 0x0a ) {		// header trigger data
    trig  = (fTSPacket & 0x000F0000) >> 16;
    daq   = (fTSPacket & 0x0000F000) >> 12;
    token = (fTSPacket & 0x00000FFF);
    tdc_ch = -1;

  } 
  else if ( id == 0x0b ) {		// header debug data

  } 
  else if ( id == 0x0e ) {		// TDIG separator word
    board_id = (fTSPacket & 0x0F000000) >> 24;
    num      = (fTSPacket & 0x0000FF00) >> 8;
    count    = (fTSPacket & 0x000000FF);

    tdc_ch = -1;
  }
  else {				// unknown TSPacket
    tdc_ch = -1;
  }
}

Short_t TsPacket::tdc_map(Short_t tdc_ch) {
  //this does nothing?	
  return tdc_ch;
}

