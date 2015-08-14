--------------------------------------------------------------------------
-- okWireIn.vhd
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

entity okWireIn is
	port (
		ok1            : in   std_logic_vector(30 downto 0);
		ok2            : out  std_logic_vector(16 downto 0);
		ep_addr        : in   std_logic_vector(7 downto 0);
		ep_dataout     : out  std_logic_vector(15 downto 0)
	);
end okWireIn;

architecture arch of okWireIn is
	signal ti_clock       : std_logic;
	signal ti_reset       : std_logic;
	signal ti_write       : std_logic;
	signal ti_wireupdate  : std_logic;
	signal ti_addr        : std_logic_vector(7 downto 0);
	signal ti_datain      : std_logic_vector(15 downto 0);
	signal ep_datahold    : std_logic_vector(15 downto 0) := x"0000";

begin
	ti_clock         <= ok1(OK_TI_CLK);
	ti_reset         <= ok1(OK_TI_RESET);
	ti_write         <= ok1(OK_TI_WRITE);
	ti_addr          <= ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START);
	ti_datain        <= ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START);
	ti_wireupdate    <= ok1(OK_TI_WIREUPDATE);

	process is
	begin
		wait until rising_edge(ti_clock);
		if ((ti_write = '1') and (ti_addr = ep_addr)) then
			ep_datahold <= ti_datain;
			wait for 0 ns;
		end if;
		if (ti_wireupdate = '1') then
			wait for TDOUT_DELAY;
			ep_dataout <= ep_datahold;
			wait for 0 ns;
		end if;
		if (ti_reset = '1') then
			wait for TDOUT_DELAY;
			ep_datahold <= x"0000";
			wait for 0 ns;
			ep_dataout <= x"0000";
			wait for 0 ns;
		end if;
		
	end process;
	
end arch;
