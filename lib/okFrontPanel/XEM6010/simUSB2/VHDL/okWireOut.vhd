--------------------------------------------------------------------------
-- okWireOut.vhd
--
-- This entity simulates the "Wire In" endpoint.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use work.parameters.all;
use work.mappings.all;

entity okWireOut is
	port (
		ok1            : in   std_logic_vector(30 downto 0);
		ok2            : out  std_logic_vector(16 downto 0);
		ep_addr        : in   std_logic_vector(7 downto 0);
		ep_datain      : in	  std_logic_vector(15 downto 0)
	);
end okWireOut;

architecture arch of okWireOut is
	signal ti_clock         : std_logic;
	signal ti_reset       : std_logic;
	signal ti_read        : std_logic;
	signal ti_wireupdate  : std_logic;
	signal ti_addr        : std_logic_vector(7 downto 0);
	signal wirehold       : std_logic_vector(15 downto 0);

begin
	ti_clock         <= ok1(OK_TI_CLK);
	ti_reset         <= ok1(OK_TI_RESET);
	ti_read          <= ok1(OK_TI_READ);
	ti_addr          <= ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START);
	ti_wireupdate    <= ok1(OK_TI_WIREUPDATE);

	ok2(OK_DATAOUT_END downto OK_DATAOUT_START) <= wirehold when (ti_addr = ep_addr) else (others => '0');
	ok2(OK_READY) <= '0';
	
	process is
	begin
		wait until rising_edge(ti_clock);
		
		if (ti_wireupdate = '1') then
				wirehold <= ep_datain;
				wait for 0 ns;
		end if;

		if (ti_reset = '1') then
			wirehold <= x"0000";
			wait for 0 ns;
		end if;
		
	end process;
	
end arch;
