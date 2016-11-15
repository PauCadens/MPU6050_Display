-------------------------------------------------------------------------------
-- mpu6050_stub.vhd
-------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VCOMPONENTS.ALL;

entity mpu6050_stub is
  port (
    USB_Uart_sout : out std_logic;
    USB_Uart_sin : in std_logic;
    RESET : in std_logic;
    LEDs_4Bits_TRI_O : out std_logic_vector(3 downto 0);
    CLK_66MHZ : in std_logic;
    CDCE913_SDA : inout std_logic;
    CDCE913_SCL : inout std_logic;
    LCD_IO_PIN : inout std_logic_vector(6 downto 0)
  );
end mpu6050_stub;

architecture STRUCTURE of mpu6050_stub is

  component mpu6050 is
    port (
      USB_Uart_sout : out std_logic;
      USB_Uart_sin : in std_logic;
      RESET : in std_logic;
      LEDs_4Bits_TRI_O : out std_logic_vector(3 downto 0);
      CLK_66MHZ : in std_logic;
      CDCE913_SDA : inout std_logic;
      CDCE913_SCL : inout std_logic;
      LCD_IO_PIN : inout std_logic_vector(6 downto 0)
    );
  end component;

  attribute BOX_TYPE : STRING;
  attribute BOX_TYPE of mpu6050 : component is "user_black_box";

begin

  mpu6050_i : mpu6050
    port map (
      USB_Uart_sout => USB_Uart_sout,
      USB_Uart_sin => USB_Uart_sin,
      RESET => RESET,
      LEDs_4Bits_TRI_O => LEDs_4Bits_TRI_O,
      CLK_66MHZ => CLK_66MHZ,
      CDCE913_SDA => CDCE913_SDA,
      CDCE913_SCL => CDCE913_SCL,
      LCD_IO_PIN => LCD_IO_PIN
    );

end architecture STRUCTURE;

