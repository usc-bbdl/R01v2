--------------------------------------------------------------------------
-- okPipeOut.vhd
--
-- This entity simulates the "Output Pipe" endpoint.
--
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use work.parameters.all;
use work.mappings.all;

entity okPipeOut is
	port (
		ok1            : in   std_logic_vector(30 downto 0);
		ok2            : out  std_logic_vector(16 downto 0);
		ep_addr        : in   std_logic_vector(7 downto 0);
		ep_read        : out  std_logic;
		ep_datain      : in  std_logic_vector(15 downto 0)
	);
end okPipeOut;

architecture arch of okPipeOut is
	signal	ti_read  : std_logic;
	signal	ti_addr  : std_logic_vector(7 downto 0);
	
begin
	ti_read          <= ok1(OK_TI_READ);
	ti_addr          <= ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START);

	ok2(OK_DATAOUT_END downto OK_DATAOUT_START) <= ep_datain when (ti_addr = ep_addr) else (others => '0');
	ok2(OK_READY) <= '1' when (ti_addr = ep_addr) else ('0');
	ep_read <= '1' when ((ti_read = '1') and (ti_addr = ep_addr)) else '0';

end arch;
