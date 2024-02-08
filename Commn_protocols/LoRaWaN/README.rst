COMMUNICATING WITH LORA USING LOPY ON ZEPHYR RTOS

Source code used is :

/home/sameer/ZEPHYR/zephyrproject/zephyr/samples/drivers/lora/

send/src/main.c

recv/src/main.c

prj.conf contains

CONFIG_LOG=y

CONFIG_SPI=y

CONFIG_GPIO=y

CONFIG_LORA=y

CONFIG_PRINTK=y

CONFIG_LORA_LOG_LEVEL_DBG=y

CONFIG_HEAP_MEM_POOL_SIZE=2048

CONFIG_MAIN_STACK_SIZE=8182

Create a directory “boards” and add the file **esp32_devkitc_wroom.overlay:**

&spi2 {

      status = "okay";

      lora: sx1272@0 {

          compatible = "semtech,sx1272";

          reg = <0>;

          label = "sx1272";

          reset-gpios = <&gpio 18 GPIO_ACTIVE_HIGH>;

  dio-gpios = <&gpio 23 (GPIO_PULL_DOWN | GPIO_ACTIVE_HIGH)>,<&gpio 23 (GPIO_PULL_DOWN | GPIO_ACTIVE_HIGH)>,<&gpio 23 (GPIO_PULL_DOWN | GPIO_ACTIVE_HIGH)>;

          spi-max-frequency = <1000000>;

          power-amplifier-output = "pa-boost";

      };

   };

   / {

      aliases {

         lora0 = &lora;

    };

   };

There are two SPIs on ESP-32 HSPI and VSPI. Zephyr denotes these as SPI2 and SPI3

Refer the file for more information:

/home/sameer/ZEPHYR/zephyrproject/zephyr/boards/xtensa/esp32_devkitc_wroom/esp32_devkitc_wroom.dts

We will compile the example for ESP32 DevKitC WROOM board

We need to modify the Pins for the SPI that are connected from ESP-32 to Semtech SX1272.

/home/sameer/ZEPHYR/zephyrproject/zephyr/boards/xtensa/esp32_devkitc_wroom/esp32_devkitc_wroom-pinctrl.dtsi

spim2_default: spim2_default {

               group1 {

                       pinmux = <SPIM2_MISO_GPIO19>,

                                <SPIM2_SCLK_GPIO5>,

                                <SPIM2_CSEL_GPIO17>;

               };

               group2 {

                       pinmux = <SPIM2_MOSI_GPIO27>;

                       output-low;

               };

       };

MISO, MOSI and CSEL for SPI are specified in the pin control file. We need to modify either SPI2 or SPI3 and then specify the corresponding SPI in the overlay file.

Use the configurations of pins as mentioned below in the overlay file

([“semtech,sx1272 (on spi bus) — Zephyr Project Documentation”](zotero://select/groups/2348753/items/QZFKC5BL))

Please note that the ESP32 version used in LoPy does not require a reset connection. make sure that the overlay file does not contain the reset pin configuration.

The DIO pins are wired together . refer the links below:

([Network](zotero://select/groups/2348753/items/76V98CZH))

([“Big ESP32 / SX127x topic part 1 - End Devices (Nodes)”, 2017](zotero://select/groups/2348753/items/JZA5YXYR))

LMIC configuration is :  
const lmic_pinmap lmic_pins = {  
.mosi = 27, // specify this in pinctl.dtsi  
.miso = 19, // specify this in pinctl.dtsi  
.sck = 5, // specify this in pinctl.dtsi  
.nss = 17, // NOT REQUIRED  
.rxtx = LMIC_UNUSED_PIN, // NOT REQUIRED  
.rst = 18, // Overlay file  
.dio = {23, 23, 23}, //workaround to use 1 pin for all 3 radio dio pins specify in overlay file  
};

All DIO are wired through a diode bridge to a single pin on the esp32 . This way you get events from all pins without sacrificing additional IO on the esp32 but you'll have to check the registers to see which pin triggered the interrupt.

Compile examples with

west build -p always -b esp32_devkitc_wroom receive/

west build -p always -b esp32_devkitc_wroom send/

west flash

try with

minicom -D /dev/ttyUSB0

Sample output: (recv)

I (190) boot: Loaded app from partition at offset 0x10000

I (190) boot: Disabling RNG early entropy source...

[00:00:01.541,000] <inf> sx127x: SX127x version 0x22 found

\*\*\* Booting Zephyr OS build zephyr-v3.5.0-3019-gcb677febb142 \*\*\*

[00:00:01.552,000] <inf> lora_receive: Synchronous reception

[00:00:02.167,000] <inf> lora_receive: Received data: helloworld (RSSI:-129dBm, SNR:-9dBm)

[00:00:03.457,000] <inf> lora_receive: Received data: helloworld (RSSI:-129dBm, SNR:-9dBm)

[00:00:04.748,000] <inf> lora_receive: Received data: helloworld (RSSI:-128dBm, SNR:-9dBm)

[00:00:06.039,000] <inf> lora_receive: Received data: helloworld (RSSI:-129dBm, SNR:-9dBm)

[00:00:06.039,000] <inf> lora_receive: Asynchronous reception

Sample output: (send)

0:00:01.540,000] <inf> sx127x: SX127x version 0x22 found

\*\*\* Booting Zephyr OS build zephyr-v3.5.0-3019-gcb677febb142 \*\*\*

[00:00:01.553,000] <dbg> sx12xx_common: sx12xx_lora_send: Expected air time of 10 bytes = 289ms

[00:00:01.842,000] <inf> lora_send: Data sent!

[00:00:02.844,000] <dbg> sx12xx_common: sx12xx_lora_send: Expected air time of 10 bytes = 289ms

[00:00:03.133,000] <inf> lora_send: Data sent!
