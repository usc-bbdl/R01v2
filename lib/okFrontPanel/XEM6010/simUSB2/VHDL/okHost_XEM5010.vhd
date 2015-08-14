library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity okHost_XEM5010 is
	port (
		hi_in    : in std_logic_vector(7 downto 0);
		hi_out   : out std_logic_vector(1 downto 0) := "00";
		hi_inout : inout std_logic_vector(15 downto 0);
		ti_clk   : out std_logic;
		ok1      : out std_logic_vector(30 downto 0) := "0000000000000000000000000000000";
		ok2      : in std_logic_vector(16 downto 0)
	);
end okHost_XEM5010;

architecture arch of okHost_XEM5010 is
	component okHost port (
		hi_in    : in std_logic_vector(7 downto 0);
		hi_out   : out std_logic_vector(1 downto 0);
		hi_inout : inout std_logic_vector(15 downto 0);
		ti_clk   : out std_logic;
		ok1      : out std_logic_vector(30 downto 0);
		ok2      : in std_logic_vector(16 downto 0));
	end component;
begin
	h0 : okHost port map (
		hi_in    => hi_in,
		hi_out   => hi_out,
		hi_inout => hi_inout,
		ti_clk   => ti_clk,
		ok1      => ok1,
		ok2      => ok2
	);
end arch;
