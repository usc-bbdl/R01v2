--------------------------------------------------------------------------
-- FrontPanel Library Module Declarations (VHDL)
--
-- Copyright (c) 2004-2009 Opal Kelly Incorporated
-- $Rev: 4 $ $Date: 2014-05-20 16:57:47 -0700 (Tue, 20 May 2014) $
--------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity okHost_XEM5010 is
	port (
		hi_in     : in std_logic_vector(7 downto 0);
		hi_out    : out	std_logic_vector(1 downto 0);
		hi_inout  : inout	std_logic_vector(15 downto 0);
		ti_clk    : out std_logic;
		ok1       : out std_logic_vector(30 downto 0);
		ok2       : in std_logic_vector(16 downto 0));
end entity;
architecture arch_okHost_XEM5010 of okHost_XEM5010 is
	component okHost port (
		hi_in    : in std_logic_vector(7 downto 0);
		hi_out   : out std_logic_vector(1 downto 0);
		hi_inout : inout std_logic_vector(15 downto 0);
		ti_clk   : out std_logic;
		ok1      : out std_logic_vector(30 downto 0);
		ok2      : in std_logic_vector(16 downto 0));
	end component;
begin
		h1: okHost port map (
		    hi_in=>hi_in, hi_out=>hi_out, hi_inout=>hi_inout,
		    ti_clk=>ti_clk, ok1=>ok1, ok2=>ok2);
end;


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
package FRONTPANEL is
	component okHost port (
		hi_in    : in std_logic_vector(7 downto 0);
		hi_out   : out std_logic_vector(1 downto 0);
		hi_inout : inout std_logic_vector(15 downto 0);
		ti_clk   : out std_logic;
		ok1      : out std_logic_vector(30 downto 0);
		ok2      : in std_logic_vector(16 downto 0));
	end component;

	component okHost_XEM5010 port (
		hi_in     : in std_logic_vector(7 downto 0);
		hi_out    : out	std_logic_vector(1 downto 0);
		hi_inout  : inout	std_logic_vector(15 downto 0);
		ti_clk    : out std_logic;
		ok1       : out std_logic_vector(30 downto 0);
		ok2       : in std_logic_vector(16 downto 0));
	end component;

	component okWireIn port (
		ok1        : in std_logic_vector(30 downto 0);
		ep_addr    : in std_logic_vector(7 downto 0);
		ep_dataout : out std_logic_vector(15 downto 0));
	end component;

	component okWireOut port (
		ok1       : in std_logic_vector(30 downto 0);
		ok2       : out std_logic_vector(16 downto 0);
		ep_addr   : in std_logic_vector(7 downto 0);
		ep_datain : in std_logic_vector(15 downto 0));
	end component;

	component okTriggerIn port (
		ok1        : in std_logic_vector(30 downto 0);
		ep_addr    : in std_logic_vector(7 downto 0);
		ep_clk     : in std_logic;
		ep_trigger : out std_logic_vector(15 downto 0));
	end component;

	component okTriggerOut port (
		ok1        : in std_logic_vector(30 downto 0);
		ok2        : out std_logic_vector(16 downto 0);
		ep_addr    : in std_logic_vector(7 downto 0);
		ep_clk     : in std_logic;
		ep_trigger : in std_logic_vector(15 downto 0));
	end component;

	component okPipeIn port (
		ok1        : in std_logic_vector(30 downto 0);
		ok2        : out std_logic_vector(16 downto 0);
		ep_addr    : in std_logic_vector(7 downto 0);
		ep_write   : out std_logic;
		ep_dataout : out std_logic_vector(15 downto 0));
	end component;

	component okPipeOut port (
		ok1        : in std_logic_vector(30 downto 0);
		ok2        : out std_logic_vector(16 downto 0);
		ep_addr    : in std_logic_vector(7 downto 0);
		ep_read    : out std_logic;
		ep_datain  : in std_logic_vector(15 downto 0));
	end component;
	
	component okBTPipeIn port (
		ok1            : in std_logic_vector(30 downto 0);
		ok2            : out std_logic_vector(16 downto 0);
		ep_addr        : in std_logic_vector(7 downto 0);
		ep_write       : out std_logic;
		ep_blockstrobe : out std_logic;
		ep_dataout     : out std_logic_vector(15 downto 0);
		ep_ready       : in std_logic);
	end component;

	component okBTPipeOut port (
		ok1            : in std_logic_vector(30 downto 0);
		ok2            : out std_logic_vector(16 downto 0);
		ep_addr        : in std_logic_vector(7 downto 0);
		ep_read        : out std_logic;
		ep_blockstrobe : out std_logic;
		ep_datain      : in std_logic_vector(15 downto 0);
		ep_ready       : in std_logic);
	end component;

	component okWireOR
	generic (N : integer := 1);
	port (
		ok2   : out std_logic_vector(16 downto 0);
		ok2s  : in  std_logic_vector(N*17-1 downto 0));
	end component;
end FRONTPANEL;

