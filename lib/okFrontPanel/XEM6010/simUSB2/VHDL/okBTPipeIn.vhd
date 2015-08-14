--------------------------------------------------------------------------
-- okBTPipeIn.vhd
--
-- This entity simulates the "Block Throttled Input Pipe" endpoint.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use work.parameters.all;
use work.mappings.all;

entity okBTPipeIn is
	port (
		ok1            : in   std_logic_vector(30 downto 0);
		ok2            : out  std_logic_vector(16 downto 0);
		ep_addr        : in   std_logic_vector(7 downto 0);
		ep_write       : out  std_logic;
		ep_blockstrobe : out  std_logic;
		ep_dataout     : out  std_logic_vector(15 downto 0);
		ep_ready       : in   std_logic
	);
end okBTPipeIn;

architecture arch of okBTPipeIn is
	signal ti_clock       : std_logic;
	signal ti_reset       : std_logic;
	signal ti_write       : std_logic;
	signal ti_addr        : std_logic_vector(7 downto 0);
	signal ti_datain      : std_logic_vector(15 downto 0);
	signal ti_blockstrobe : std_logic;

begin

	ti_clock         <= ok1(OK_TI_CLK);
	ti_reset         <= ok1(OK_TI_RESET);
	ti_write         <= ok1(OK_TI_WRITE);
	ti_addr          <= ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START);
	ti_datain        <= ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START);
	ti_blockstrobe   <= ok1(OK_TI_BLOCKSTROBE);
	
	ep_blockstrobe <= '1' when (ti_blockstrobe = '1') and (ti_addr = ep_addr) else '0';
	ok2(OK_DATAOUT_END downto OK_DATAOUT_START) <= (others => '0');
	ok2(OK_READY) <= ep_ready when (ti_addr = ep_addr) else '0';

	process is
	begin
		wait until rising_edge(ti_clock);
		wait for TDOUT_DELAY;
		ep_write <= '0';
		if (ti_reset = '1') then
			ep_dataout <= x"0000";
		elsif ((ti_write = '1') and (ti_addr = ep_addr)) then
			ep_dataout <= ti_datain;
			ep_write <= '1';
		end if;
	end process;
end arch;
