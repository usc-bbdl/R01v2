--------------------------------------------------------------------------
-- parameters.vhd
--
-- Description:
--  This file contains simulation delay parameters to control data 
--  propagation timing in behavioral simulations.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.all;

package parameters is

	constant UPDATE_TO_READOUT_CLOCKS : integer := 15;    -- Specifies the number if TI_CLK cycles between a trigger out update and readout.
                                                        -- Lengthen this if EP_CLK << TI_CLK.

	constant Tti  : time := 10.416 ns;  --48Mhz
	constant Tep  : time := 5 ns;       --100Mhz 

	constant TDOUT_DELAY    : time := 0 ns;
	constant TTRIG_DELAY    : time := 0 ns;
 
end parameters;


package body parameters is

 
end parameters;