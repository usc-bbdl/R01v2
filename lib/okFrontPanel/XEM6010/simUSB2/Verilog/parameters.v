//------------------------------------------------------------------------
// parameters.v
//
// Description:
//  This file contains simulation delay parameters to control data 
//  propagation timing in behavioral simulations.
//
//------------------------------------------------------------------------
// Copyright (c) 2005-2010 Opal Kelly Incorporated
// $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
//------------------------------------------------------------------------
parameter UPDATE_TO_READOUT_CLOCKS = 15;    // Specifies the number if TI_CLK cycles between a trigger out update and readout.
                                            // Lengthen this if EP_CLK << TI_CLK.

parameter Tti  = 10.416;  //48Mhz
parameter Tep  = 5;       //100Mhz 

parameter TDOUT_DELAY    = 0;
parameter TTRIG_DELAY    = 0;
