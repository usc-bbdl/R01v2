--------------------------------------------------------------------------
-- okTriggerIn.vhd
--
-- This entity simulates the "Trigger In" endpoint.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use work.parameters.all;
use work.mappings.all;

entity okTriggerIn is
	port (
		ok1            : in   std_logic_vector(30 downto 0);
		ok2            : out  std_logic_vector(16 downto 0);
		ep_addr        : in   std_logic_vector(7 downto 0);
		ep_clk         : in   std_logic;
		ep_trigger     : out  std_logic_vector(15 downto 0):= x"0000"
	);
end okTriggerIn;

architecture arch of okTriggerIn is
	signal	ti_clock   : std_logic;
	signal	ti_reset   : std_logic;
	signal	ti_write   : std_logic;
	signal	ti_addr    : std_logic_vector(7 downto 0);
	signal	ti_datain  : std_logic_vector(15 downto 0);

	signal	eptrig     : std_logic_vector(15 downto 0) := x"0000";
	signal	eptrig_p1  : std_logic_vector(15 downto 0) := x"0000";
	signal	eptrig_p2  : std_logic_vector(15 downto 0) := x"0000";

begin
	ti_clock         <= ok1(OK_TI_CLK);
	ti_reset         <= ok1(OK_TI_RESET);
	ti_write         <= ok1(OK_TI_WRITE);
	ti_addr          <= ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START);
	ti_datain        <= ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START);

	process is
	begin
		wait until (rising_edge(ti_reset) or rising_edge(ep_clk));
		if (ti_reset = '1' ) then
			wait for TTRIG_DELAY;
			ep_trigger <= x"0000";
			eptrig_p1 <= x"0000";
			eptrig_p2 <= x"0000";
			wait for 0 ns;
		elsif (ep_clk = '1') then
		  eptrig_p1 <= eptrig;
		  eptrig_p2 <= eptrig_p1;
			wait for TTRIG_DELAY;
			ep_trigger <= eptrig_p1 xor eptrig_p2;
			wait for 0 ns;
		end if;
	end process;

	process is
	begin
		wait until (rising_edge(ti_clock));
		if (ti_reset = '1') then
			eptrig <= x"0000";
			wait for 0 ns;
		elsif ( (ti_write = '1') and (ti_addr = ep_addr) ) then
			eptrig <= (eptrig xor ti_datain);
			wait for 0 ns;
		end if;
	
	end process;
end arch;