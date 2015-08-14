--------------------------------------------------------------------------
-- okWireOR.vhd
--
-- This module implements the okWireOR for simulation usage.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity okWireOR is
	generic (
		N     : integer := 1
	);
	port (
		ok2   : out std_logic_vector(16 downto 0);
		ok2s  : in  std_logic_vector(N*17-1 downto 0)
	);
end okWireOR;

architecture archWireOR of okWireOR is
begin
	process (ok2s)
		variable ok2_int : STD_LOGIC_VECTOR(16 downto 0);
	begin
		ok2_int := b"0_0000_0000_0000_0000";
		for i in N-1 downto 0 loop
			ok2_int := ok2_int or ok2s( (i*17+16) downto (i*17) );
		end loop;
		ok2 <= ok2_int;
	end process;
end archWireOR;
