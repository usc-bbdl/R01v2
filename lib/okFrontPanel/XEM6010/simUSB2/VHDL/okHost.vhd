--------------------------------------------------------------------------
-- okHost.vhd
--
--	Description:
--		This file is a simulation replacement for okCore for
--		FrontPanel. It receives data from okHostCalls.v which is 
--		then restructured and timed to communicate with the endpoint
--		simulation modules.
--------------------------------------------------------------------------
-- Copyright (c) 2005-2010 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;
use work.parameters.all;
use work.mappings.all;

entity okHost is
	port (
		hi_in    : in std_logic_vector(7 downto 0);
		hi_out   : out std_logic_vector(1 downto 0) := "00";
		hi_inout : inout std_logic_vector(15 downto 0);
		hi_aa    : inout std_logic;
		ti_clk   : out std_logic;
		ok1      : out std_logic_vector(30 downto 0) := "0000000000000000000000000000000";
		ok2      : in std_logic_vector(16 downto 0)
	);
end okHost;

architecture arch of okHost is

	constant DNOP                  : STD_LOGIC_VECTOR(3 downto 0) := x"0";
	constant DReset                : STD_LOGIC_VECTOR(3 downto 0) := x"1";
	constant DSetWireIns           : STD_LOGIC_VECTOR(3 downto 0) := x"2";
	constant DUpdateWireIns        : STD_LOGIC_VECTOR(3 downto 0) := x"3";
	constant DGetWireOutValue      : STD_LOGIC_VECTOR(3 downto 0) := x"4";
	constant DUpdateWireOuts       : STD_LOGIC_VECTOR(3 downto 0) := x"5";
	constant DActivateTriggerIn    : STD_LOGIC_VECTOR(3 downto 0) := x"6";
	constant DUpdateTriggerOuts    : STD_LOGIC_VECTOR(3 downto 0) := x"7";
	constant DIsTriggered          : STD_LOGIC_VECTOR(3 downto 0) := x"8";
	constant DWriteToPipeIn        : STD_LOGIC_VECTOR(3 downto 0) := x"9";
	constant DReadFromPipeOut      : STD_LOGIC_VECTOR(3 downto 0) := x"a";
	constant DWriteToBlockPipeIn   : STD_LOGIC_VECTOR(3 downto 0) := x"b";
	constant DReadFromBlockPipeOut : STD_LOGIC_VECTOR(3 downto 0) := x"c";

	constant CReset              : STD_LOGIC_VECTOR(4 downto 0) := "00001";
	constant CSetWireIns         : STD_LOGIC_VECTOR(4 downto 0) := "00100";
	constant CUpdateWireIns      : STD_LOGIC_VECTOR(4 downto 0) := "01000";
	constant CGetWireOutValue    : STD_LOGIC_VECTOR(4 downto 0) := "00010";
	constant CUpdateWireOuts     : STD_LOGIC_VECTOR(4 downto 0) := "01000";
	constant CActivateTriggerIn  : STD_LOGIC_VECTOR(4 downto 0) := "00100";
	constant CUpdateTriggerOuts  : STD_LOGIC_VECTOR(4 downto 0) := "10000";
	constant CIsTriggered        : STD_LOGIC_VECTOR(4 downto 0) := "00010";
	constant CWriteToPipeIn      : STD_LOGIC_VECTOR(4 downto 0) := "00100";
	constant CReadFromPipeOut    : STD_LOGIC_VECTOR(4 downto 0) := "00010";
	constant CWriteToBTPipeIn    : STD_LOGIC_VECTOR(4 downto 0) := "00100";
	constant CReadFromBTPipeOut  : STD_LOGIC_VECTOR(4 downto 0) := "00010";

	signal clk_int               : STD_LOGIC;
	signal hi_dataout            : STD_LOGIC_VECTOR(15 downto 0) := (others => '0');
	signal ok1_command           : STD_LOGIC_VECTOR(4 downto 0);
	
begin

ti_clk <= hi_in(0);
clk_int <= hi_in(0);		-- hi_clk internally buffered for this process
hi_inout <= hi_dataout when (hi_in(1) = '0') else (others => 'Z');

ok1(OK_TI_CLK)           <= hi_in(0); 
ok1(OK_TI_RESET)         <= ok1_command(0); 
ok1(OK_TI_READ)          <= ok1_command(1); 
ok1(OK_TI_WRITE)         <= ok1_command(2); 
ok1(OK_TI_WIREUPDATE)    <= ok1_command(3); 
ok1(OK_TI_TRIGUPDATE)    <= ok1_command(4); 

process
	variable ep                 : STD_LOGIC_VECTOR(7 downto 0);
	variable i                  : INTEGER := 0;
	variable j                  : INTEGER := 0;
	variable k                  : INTEGER := 0;
	variable tmp_slv8          : STD_LOGIC_VECTOR(7 downto 0) := (others => '0');
	variable tmp_slv16         : STD_LOGIC_VECTOR(15 downto 0) := (others => '0');
	variable tmp_slv31         : STD_LOGIC_VECTOR(30 downto 0) := (others => '0');
	variable pipeLength         : INTEGER := 0;
	variable BlockDelayStates   : INTEGER := 0;
	variable blockSize          : INTEGER := 0;
	variable blockNum           : INTEGER := 0;
	variable ReadyCheckDelay    : INTEGER := 0;
	variable PostReadyDelay     : INTEGER := 0;
begin
	wait until (hi_in(7 downto 4)'event);
		--------------------------------------------------------------------
		-- Reset
		--------------------------------------------------------------------
		if (hi_in(7 downto 4) = DReset) then
			wait until (falling_edge(clk_int));
			hi_out(0) <= '1';
			hi_out(1) <= '0';
			wait until (falling_edge(clk_int));
			ok1(OK_TI_BLOCKSTROBE) <= '0';
			ok1_command <= CReset;
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			wait until (falling_edge(clk_int)); wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int)); wait until (falling_edge(clk_int));
			hi_out(0) <= '0';

		--------------------------------------------------------------------
		-- UpdateWireIns
		--------------------------------------------------------------------
		elsif (hi_in(7 downto 4) = DUpdateWireIns) then
			wait until (falling_edge(clk_int));
			hi_out(0) <= '1';
			for i in 0 to 31 loop
				ep := CONV_STD_LOGIC_VECTOR(i, 8);
				wait until (falling_edge(clk_int));
				ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= hi_inout;
				ok1_command <= CSetWireIns;
				ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
			end loop;
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
			wait until (falling_edge(clk_int));
			ok1_command <= CUpdateWireIns;
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');

			wait until (falling_edge(clk_int)); wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));	wait until (falling_edge(clk_int));
			hi_out(0) <= '0';
		
		--------------------------------------------------------------------
		-- UpdateWireOuts
		--------------------------------------------------------------------
		elsif (hi_in(7 downto 4) = DUpdateWireOuts) then
			wait until (falling_edge(clk_int));
			hi_out(0) <= '1';
			wait until (falling_edge(clk_int));
			ok1_command <= CUpdateWireOuts;
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			wait until (falling_edge(clk_int));
			for i in 0 to 31 loop
				ep := CONV_STD_LOGIC_VECTOR(i+32, 8);
				ok1_command <= CGetWireOutValue;
				ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
				wait until (falling_edge(clk_int));
				hi_dataout <= ok2(OK_DATAOUT_END downto OK_DATAOUT_START);
			end loop;
			ok1_command <= (others => '0');
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			wait until (falling_edge(clk_int));
			hi_dataout <= (others => '0');

			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			hi_out(0) <= '0';		

		--------------------------------------------------------------------
		-- ActivateTriggerIn
		--------------------------------------------------------------------
		elsif (hi_in(7 downto 4) = DActivateTriggerIn) then
			wait until (falling_edge(clk_int));
			hi_out(0) <= '1';
			ep := hi_inout(7 downto 0);
			wait until (falling_edge(clk_int));
			ok1_command <= CActivateTriggerIn;
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
			ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= hi_inout;
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');

			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			hi_out(0) <= '0';

		--------------------------------------------------------------------
		-- UpdateTriggerOuts
		--------------------------------------------------------------------
		elsif (hi_in(7 downto 4) = DUpdateTriggerOuts) then
			wait until (falling_edge(clk_int));
			hi_out(0) <= '1';
			ok1_command <= CUpdateTriggerOuts;
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0'); 
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			
			for i in 0 to (UPDATE_TO_READOUT_CLOCKS-1) loop
				wait until (falling_edge(clk_int));
			end loop;
			
			for i in 0 to 31 loop
				ep := CONV_STD_LOGIC_VECTOR(i+96, 8);  -- 96 is 0x60
				ok1_command <= CIsTriggered;
				ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
				wait until (falling_edge(clk_int));
				hi_dataout <= ok2(OK_DATAOUT_END downto OK_DATAOUT_START);
			end loop;
			ok1_command <= (others => '0');
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');

			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			hi_out(0) <= '0';

		--------------------------------------------------------------------
		-- WriteToPipeIn
		--------------------------------------------------------------------
		elsif (hi_in(7 downto 4) = DWriteToPipeIn) then
			wait until (falling_edge(clk_int));
			hi_out(0) <= '1';
			ep := hi_inout(7 downto 0);
			j := 0;
			tmp_slv8 := hi_inout(15 downto 8);
			BlockDelayStates := CONV_INTEGER(tmp_slv8);
			wait until (falling_edge(clk_int));
			tmp_slv31(15 downto 0) := hi_inout;
			wait until (falling_edge(clk_int));
			tmp_slv31(30 downto 16) := hi_inout(14 downto 0);
			pipeLength := CONV_INTEGER(tmp_slv31);
			for i in 1 to pipeLength loop
				wait until (falling_edge(clk_int));
				ok1_command <= CWriteToPipeIn;
				ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
				ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= hi_inout;
				j:=j+2;
				if (j = 1024) then
					for k in 1 to BlockDelayStates loop
						wait until (falling_edge(clk_int));
						ok1_command <= (others => '0');
						ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
						ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
					end loop;
					j:=0;
				end if;
			end loop;
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			hi_out(0) <= '0';

		--------------------------------------------------------------------
		-- ReadFromPipeOut
		--------------------------------------------------------------------
		elsif (hi_in(7 downto 4) = DReadFromPipeOut) then
			wait until (falling_edge(clk_int)); hi_out(0) <= '1';
			ep := hi_inout(7 downto 0);
			j:=0;
			tmp_slv8 := hi_inout(15 downto 8);
			BlockDelayStates := CONV_INTEGER(tmp_slv8);
			wait until (falling_edge(clk_int));	tmp_slv31(15 downto 0) := hi_inout;
			wait until (falling_edge(clk_int));	tmp_slv31(30 downto 16) := hi_inout(14 downto 0);
			pipeLength := CONV_INTEGER(tmp_slv31);
			for i in 1 to pipeLength loop
				ok1_command <= CReadFromPipeOut;
				ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
				wait until (falling_edge(clk_int));
				if (i = pipeLength) then
					ok1_command <= (others => '0');
				end if;
				hi_dataout <= ok2(OK_DATAOUT_END downto OK_DATAOUT_START);
				j:=j+2;
				if (j = 1024) then
					for k in 1 to BlockDelayStates loop
						ok1_command <= (others => '0');
						ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
						ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
						wait until (falling_edge(clk_int));
					end loop;
					j:=0;
				end if;
			end loop;

			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int));
			hi_out(0) <= '0';

		elsif (hi_in(7 downto 4) = DWriteToBlockPipeIn) then
			wait until (falling_edge(clk_int)); hi_out(0) <= '1';
			ep := hi_inout(7 downto 0);
			tmp_slv8 := hi_inout(15 downto 8);
			BlockDelayStates := CONV_INTEGER(tmp_slv8);
			wait until (falling_edge(clk_int)); tmp_slv31(15 downto 0) := hi_inout;
			wait until (falling_edge(clk_int)); tmp_slv31(30 downto 16) := hi_inout(14 downto 0);
			pipeLength := CONV_INTEGER(tmp_slv31);
			wait until (falling_edge(clk_int)); tmp_slv16 := hi_inout;
			blockSize := CONV_INTEGER(tmp_slv16);
			wait until (falling_edge(clk_int)); tmp_slv8 := hi_inout(7 downto 0);
			ReadyCheckDelay := CONV_INTEGER(tmp_slv8);
			tmp_slv8 := hi_inout(15 downto 8);
			PostReadyDelay := CONV_INTEGER(tmp_slv8);
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
			blockNum := pipeLength/blockSize;
			for i in 1 to blockNum loop
				for j in 1 to ReadyCheckDelay loop
					wait until (falling_edge(clk_int));
				end loop;
				while (ok2(OK_READY) = '0') loop
					wait until (falling_edge(clk_int));
				end loop;
				hi_out(0) <= '0';
				for j in 1 to PostReadyDelay loop
					wait until (falling_edge(clk_int));
				end loop;
				wait until (falling_edge(clk_int)); hi_out(0) <= '1';
				ok1(OK_TI_BLOCKSTROBE) <= '1';
				wait until (falling_edge(clk_int)); ok1(OK_TI_BLOCKSTROBE) <= '0';
				wait until (falling_edge(clk_int));
				for j in 1 to blockSize loop
					wait until (falling_edge(clk_int));
					ok1_command <= CWriteToBTPipeIn;
					ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= hi_inout;
				end loop;
				for j in 1 to BlockDelayStates loop
					wait until (falling_edge(clk_int));
					ok1_command <= (others => '0');
					ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
				end loop;
			end loop;
			
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
			wait until (falling_edge(clk_int)); wait until (falling_edge(clk_int)); wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int)); hi_out(0) <= '0'; j := 0;

		elsif (hi_in(7 downto 4) = DReadFromBlockPipeOut) then
			wait until (falling_edge(clk_int)); hi_out(0) <= '1';
			ep := hi_inout(7 downto 0);
			tmp_slv8 := hi_inout(15 downto 8);
			BlockDelayStates := CONV_INTEGER(tmp_slv8);
			wait until (falling_edge(clk_int)); tmp_slv31(15 downto 0) := hi_inout;
			wait until (falling_edge(clk_int)); tmp_slv31(30 downto 16) := hi_inout(14 downto 0);
			pipeLength := CONV_INTEGER(tmp_slv31);
			wait until (falling_edge(clk_int)); tmp_slv16 := hi_inout;
			blockSize := CONV_INTEGER(tmp_slv16);
			wait until (falling_edge(clk_int)); tmp_slv8 := hi_inout(7 downto 0);
			ReadyCheckDelay := CONV_INTEGER(tmp_slv8);
			tmp_slv8 := hi_inout(15 downto 8);
			PostReadyDelay := CONV_INTEGER(tmp_slv8);
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= ep;
			blockNum := pipeLength/blockSize;
			for i in 1 to blockNum loop
				for j in 1 to ReadyCheckDelay loop
					wait until (falling_edge(clk_int));
				end loop;
				while (ok2(OK_READY) = '0') loop
					wait until (falling_edge(clk_int));
				end loop;
				hi_out(0) <= '0';
				for j in 1 to PostReadyDelay loop
					wait until (falling_edge(clk_int));
				end loop;
				wait until (falling_edge(clk_int)); hi_out(0) <= '1';
				ok1(OK_TI_BLOCKSTROBE) <= '1';
				wait until (falling_edge(clk_int)); ok1(OK_TI_BLOCKSTROBE) <= '0';
				for j in 1 to blockSize loop
					ok1_command <= CReadFromPipeOut;
					wait until (falling_edge(clk_int));
					if (i = pipeLength) then
						ok1_command <= (others => '0');
					end if;
					hi_dataout <= ok2(OK_DATAOUT_END downto OK_DATAOUT_START);
				end loop;
				for j in 1 to BlockDelayStates loop
					ok1_command <= (others => '0');
					wait until (falling_edge(clk_int)); hi_dataout <= (others => '0');
				end loop;
			end loop;
			wait until (falling_edge(clk_int));
			ok1_command <= (others => '0');
			ok1(OK_TI_ADDR_END downto OK_TI_ADDR_START) <= (others => '0');
			ok1(OK_TI_DATAIN_END downto OK_TI_DATAIN_START) <= (others => '0');
			wait until (falling_edge(clk_int)); wait until (falling_edge(clk_int)); wait until (falling_edge(clk_int));
			wait until (falling_edge(clk_int)); hi_out(0) <= '0';

		elsif (hi_in(7 downto 4) = DNOP) then
			wait until (falling_edge(clk_int));
		else
			report "Unsupport hi_addr sent";
		end if;
end process;
end arch;
