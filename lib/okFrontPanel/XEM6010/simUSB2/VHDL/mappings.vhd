 --------------------------------------------------------------------------
-- mappings.vhd
--
-- Description:
--  This file contains ok1 mappings for simulation.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.all;

package mappings is

	constant OK_TI_CLK        : integer := 27;
	constant OK_TI_RESET      : integer := 28;
	constant OK_TI_READ       : integer := 9;
	constant OK_TI_WRITE      : integer := 30;
	constant OK_TI_ADDR_START : integer := 0;
	constant OK_TI_ADDR_END   : integer := 7;
	constant OK_TI_DATAIN_START  : integer := 11;
	constant OK_TI_DATAIN_END    : integer := 26;
	constant OK_TI_WIREUPDATE    : integer := 29;
	constant OK_TI_TRIGUPDATE    : integer := 10;
	constant OK_TI_BLOCKSTROBE   : integer := 8;

	constant OK_DATAOUT_START : integer := 1;
	constant OK_DATAOUT_END   : integer := 16;
	constant OK_READY         : integer := 0;
end mappings;


package body mappings is


end mappings;