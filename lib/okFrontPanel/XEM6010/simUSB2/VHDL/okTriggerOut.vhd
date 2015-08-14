--------------------------------------------------------------------------
-- okTriggerOut.vhd
--
-- This entity simulates the "Trigger Out" endpoint.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use work.parameters.all;
use work.mappings.all;

entity okTriggerOut is
	port (
		ok1            : in   std_logic_vector(30 downto 0);
		ok2            : out  std_logic_vector(16 downto 0);
		ep_addr        : in   std_logic_vector(7 downto 0);
		ep_clk         : in   std_logic;
		ep_trigger     : in   std_logic_vector(15 downto 0)
	);
end okTriggerOut;

architecture arch of okTriggerOut is
	signal ti_clock        : std_logic;
	signal ti_reset        : std_logic;
	signal ti_read         : std_logic;
	signal ti_trigupdate   : std_logic;
	signal ti_addr         : std_logic_vector(7 downto 0);

	signal	eptrig         : std_logic_vector(15 downto 0) := x"0000";
	signal	ep_trigger_p1  : std_logic_vector(15 downto 0) := x"0000";
	signal	trighold       : std_logic_vector(15 downto 0) := x"0000";
	signal	captrig	       : std_logic := '0';
	signal	captrig_p1     : std_logic := '0';
	signal	captrig_p2     : std_logic := '0';

begin
	ti_clock         <= ok1(OK_TI_CLK);
	ti_reset         <= ok1(OK_TI_RESET);
	ti_read          <= ok1(OK_TI_READ);
	ti_addr          <= ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START);
	ti_trigupdate    <= ok1(OK_TI_TRIGUPDATE);
	
	ok2(OK_DATAOUT_END downto OK_DATAOUT_START) <= trighold when (ti_addr = ep_addr) else (others => '0');
	ok2(OK_READY) <= '0';
	
	process is
	begin
		wait until (rising_edge(ti_clock));
		if (ti_reset = '1') then
			captrig <= '0';
			wait for 0 ns;
		elsif (ti_trigupdate = '1') then
			captrig <= not(captrig);
			wait for 0 ns;
		end if;
		
	end process;
	
	process is
	begin
		wait until (rising_edge(ti_reset) or rising_edge(ep_clk));
		if (ti_reset = '1') then
			wait for 0 ns;
			trighold <= x"0000";
			wait for 0 ns;
			eptrig <= x"0000";
			wait for 0 ns;
			ep_trigger_p1 <= x"0000";
			wait for 0 ns;
			captrig_p1 <= '0';
			wait for 0 ns;
			captrig_p2 <= '0';
			wait for 0 ns;
		elsif (ep_clk = '1') then
			captrig_p1 <= captrig;
			captrig_p2 <= captrig_p1;
			ep_trigger_p1 <= ep_trigger;
			if ((captrig_p1 xor captrig_p2) = '1') then
				trighold <= eptrig;
				wait for 0 ns;
				eptrig <= ep_trigger;
				wait for 0 ns;
			else
				eptrig <= (eptrig or (ep_trigger and not ep_trigger_p1));
				wait for 0 ns;
			end if;
		end if;
		
	end process;

end arch;
