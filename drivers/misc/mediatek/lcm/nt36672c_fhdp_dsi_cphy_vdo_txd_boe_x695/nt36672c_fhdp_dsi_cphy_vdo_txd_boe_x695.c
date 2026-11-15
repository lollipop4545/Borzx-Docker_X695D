/*
 * Reconstructed stock LCM driver for Infinix Note 10 Pro (X695D)
 * Panel: nt36672c_fhdp_dsi_cphy_vdo_txd_boe_x695
 * Extracted 1:1 from stock_v248_boot.img (0xffffff8009a63428)
 */

#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#endif

#include "lcm_drv.h"
#include "disp_dts_gpio.h"

#define FRAME_WIDTH  (1080)
#define FRAME_HEIGHT (2460)

#define LCM_PHYSICAL_WIDTH  (70956)
#define LCM_PHYSICAL_HEIGHT (161622)

#define REGFLAG_DELAY        0xFFFC
#define REGFLAG_UDELAY       0xFFFB
#define REGFLAG_END_OF_TABLE 0xFFFD

static struct LCM_UTIL_FUNCS lcm_util;

#define MDELAY(n) (lcm_util.mdelay(n))
#define UDELAY(n) (lcm_util.udelay(n))

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define read_reg_v2(cmd, buffer, buffer_size) \
	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
	unsigned int cmd;
	unsigned char count;
	unsigned char para_list[120];
};

static struct i2c_client *_lcm_i2c_client;
static bool is_power_suspend;

static int _lcm_i2c_write_bytes(unsigned char addr, unsigned char value)
{
	int ret = 0;
	struct i2c_client *client = _lcm_i2c_client;
	char write_data[2] = { 0 };

	if (client == NULL) {
		pr_err("[LCM][ERROR] _lcm_i2c_client is null\n");
		return -ENODEV;
	}

	write_data[0] = addr;
	write_data[1] = value;
	ret = i2c_master_send(client, write_data, 2);
	if (ret != 2) {
		pr_err("[LCM][ERROR] _lcm_i2c write(0x%02x=0x%02x) failed: %d\n",
		       addr, value, ret);
		return ret < 0 ? ret : -EIO;
	}

	return 0;
}

static int _lcm_i2c_probe(struct i2c_client *client,
			  const struct i2c_device_id *id)
{
	_lcm_i2c_client = client;
	return 0;
}

static int _lcm_i2c_remove(struct i2c_client *client)
{
	_lcm_i2c_client = NULL;
	return 0;
}

static const struct of_device_id _lcm_i2c_of_match[] = {
	{ .compatible = "mediatek,i2c_lcd_bias" },
	{ .compatible = "mediatek,I2C_LCD_BIAS" },
	{},
};

static const struct i2c_device_id _lcm_i2c_id[] = {
	{ "i2c_lcd_bias", 0 },
	{ "I2C_LCD_BIAS", 0 },
	{}
};

static struct i2c_driver _lcm_i2c_driver = {
	.id_table = _lcm_i2c_id,
	.probe = _lcm_i2c_probe,
	.remove = _lcm_i2c_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "i2c_lcd_bias",
		.of_match_table = _lcm_i2c_of_match,
	},
};

static int __init _lcm_i2c_bias_init(void)
{
	return i2c_add_driver(&_lcm_i2c_driver);
}

static void __exit _lcm_i2c_bias_exit(void)
{
	i2c_del_driver(&_lcm_i2c_driver);
}

module_init(_lcm_i2c_bias_init);
module_exit(_lcm_i2c_bias_exit);

static struct LCM_setting_table init_setting[] = {
	{0xFF, 1, {0x10}},
	{0xFB, 1, {0x01}},
	{0xB0, 1, {0x00}},
	{0xC1, 16, {0x89, 0x28, 0x00, 0x14, 0x00, 0xAA, 0x02, 0x0E, 0x00, 0x71, 0x00, 0x07, 0x05, 0x0E, 0x05, 0x16}},
	{0xC2, 2, {0x1B, 0xA0}},
	{0xFF, 1, {0x20}},
	{0xFB, 1, {0x01}},
	{0x01, 1, {0x66}},
	{0x07, 1, {0x3C}},
	{0x1B, 1, {0x01}},
	{0x5C, 1, {0x90}},
	{0x5E, 1, {0xE6}},
	{0x69, 1, {0xF2}},
	{0x95, 1, {0xE5}},
	{0x96, 1, {0xE5}},
	{0xF2, 1, {0x64}},
	{0xF4, 1, {0x64}},
	{0xF6, 1, {0x64}},
	{0xF8, 1, {0x64}},
	{0xFF, 1, {0x24}},
	{0xFB, 1, {0x01}},
	{0x04, 1, {0x22}},
	{0x05, 1, {0x00}},
	{0x06, 1, {0xA3}},
	{0x07, 1, {0xA3}},
	{0x08, 1, {0x0F}},
	{0x09, 1, {0x0F}},
	{0x0A, 1, {0x17}},
	{0x0B, 1, {0x15}},
	{0x0C, 1, {0x13}},
	{0x0D, 1, {0x2D}},
	{0x0E, 1, {0x2C}},
	{0x0F, 1, {0x2F}},
	{0x10, 1, {0x2E}},
	{0x11, 1, {0x29}},
	{0x12, 1, {0x24}},
	{0x13, 1, {0x24}},
	{0x14, 1, {0x0B}},
	{0x15, 1, {0x0C}},
	{0x16, 1, {0x1C}},
	{0x17, 1, {0x01}},
	{0x1C, 1, {0x22}},
	{0x1D, 1, {0x00}},
	{0x1E, 1, {0xA3}},
	{0x1F, 1, {0xA3}},
	{0x20, 1, {0x0F}},
	{0x21, 1, {0x0F}},
	{0x22, 1, {0x17}},
	{0x23, 1, {0x15}},
	{0x24, 1, {0x13}},
	{0x25, 1, {0x2D}},
	{0x26, 1, {0x2C}},
	{0x27, 1, {0x2F}},
	{0x28, 1, {0x2E}},
	{0x29, 1, {0x29}},
	{0x2A, 1, {0x24}},
	{0x2B, 1, {0x24}},
	{0x2D, 1, {0x0B}},
	{0x2F, 1, {0x0C}},
	{0x30, 1, {0x1C}},
	{0x31, 1, {0x01}},
	{0x32, 1, {0x44}},
	{0x33, 1, {0x02}},
	{0x34, 1, {0x00}},
	{0x35, 1, {0x01}},
	{0x36, 1, {0x01}},
	{0x37, 1, {0x01}},
	{0x38, 1, {0x10}},
	{0x3B, 1, {0x04}},
	{0x4E, 1, {0x48}},
	{0x4F, 1, {0x48}},
	{0x53, 1, {0x48}},
	{0x7A, 1, {0x83}},
	{0x7B, 1, {0x93}},
	{0x7D, 1, {0x04}},
	{0x80, 1, {0x04}},
	{0x81, 1, {0x04}},
	{0x82, 1, {0x13}},
	{0x84, 1, {0x31}},
	{0x85, 1, {0x00}},
	{0x86, 1, {0x00}},
	{0x87, 1, {0x00}},
	{0x90, 1, {0x13}},
	{0x92, 1, {0x31}},
	{0x93, 1, {0x00}},
	{0x94, 1, {0x00}},
	{0x95, 1, {0x00}},
	{0x9C, 1, {0xF4}},
	{0x9D, 1, {0x01}},
	{0xA0, 1, {0x13}},
	{0xA2, 1, {0x13}},
	{0xA3, 1, {0x03}},
	{0xA4, 1, {0x04}},
	{0xA5, 1, {0x04}},
	{0xC4, 1, {0x80}},
	{0xC6, 1, {0xC0}},
	{0xC9, 1, {0x00}},
	{0xD9, 1, {0x80}},
	{0xE9, 1, {0x03}},
	{0xFF, 1, {0x25}},
	{0xFB, 1, {0x01}},
	{0x0F, 1, {0x1B}},
	{0x19, 1, {0xE4}},
	{0x21, 1, {0x40}},
	{0x58, 1, {0x0C}},
	{0x59, 1, {0x0A}},
	{0x5C, 1, {0x05}},
	{0x5F, 1, {0x10}},
	{0x66, 1, {0xD8}},
	{0x67, 1, {0x01}},
	{0x68, 1, {0x58}},
	{0x69, 1, {0x10}},
	{0x6B, 1, {0x00}},
	{0x6C, 1, {0x1D}},
	{0x71, 1, {0x1D}},
	{0x77, 1, {0x62}},
	{0x79, 1, {0x90}},
	{0x7E, 1, {0x15}},
	{0x7F, 1, {0x00}},
	{0x84, 1, {0x6D}},
	{0x8D, 1, {0x00}},
	{0xC0, 1, {0xD5}},
	{0xC1, 1, {0x11}},
	{0xC3, 1, {0x00}},
	{0xC4, 1, {0x11}},
	{0xC5, 1, {0x11}},
	{0xC6, 1, {0x11}},
	{0xEF, 1, {0x00}},
	{0xF0, 1, {0x00}},
	{0xF1, 1, {0x04}},
	{0xFF, 1, {0x26}},
	{0xFB, 1, {0x01}},
	{0x00, 1, {0x00}},
	{0x01, 1, {0xF7}},
	{0x02, 1, {0xF7}},
	{0x04, 1, {0xF7}},
	{0x05, 1, {0x08}},
	{0x06, 1, {0x1A}},
	{0x07, 1, {0x1A}},
	{0x08, 1, {0x1A}},
	{0x14, 1, {0x06}},
	{0x15, 1, {0x01}},
	{0x74, 1, {0xAF}},
	{0x81, 1, {0x13}},
	{0x83, 1, {0x03}},
	{0x84, 1, {0x03}},
	{0x85, 1, {0x01}},
	{0x86, 1, {0x03}},
	{0x87, 1, {0x01}},
	{0x88, 1, {0x06}},
	{0x8A, 1, {0x1A}},
	{0x8B, 1, {0x11}},
	{0x8C, 1, {0x24}},
	{0x8E, 1, {0x42}},
	{0x8F, 1, {0x11}},
	{0x90, 1, {0x11}},
	{0x91, 1, {0x11}},
	{0x9A, 1, {0x80}},
	{0x9B, 1, {0x08}},
	{0x9C, 1, {0x00}},
	{0x9D, 1, {0x00}},
	{0x9E, 1, {0x00}},
	{0xFF, 1, {0x27}},
	{0xFB, 1, {0x01}},
	{0x01, 1, {0x9C}},
	{0x20, 1, {0x81}},
	{0x21, 1, {0xDF}},
	{0x25, 1, {0x82}},
	{0x26, 1, {0x13}},
	{0x6E, 1, {0x9A}},
	{0x6F, 1, {0x78}},
	{0x70, 1, {0x00}},
	{0x71, 1, {0x00}},
	{0x72, 1, {0x00}},
	{0x73, 1, {0x00}},
	{0x74, 1, {0x00}},
	{0x75, 1, {0x00}},
	{0x76, 1, {0x00}},
	{0x77, 1, {0x00}},
	{0x7D, 1, {0x09}},
	{0x7E, 1, {0xA5}},
	{0x7F, 1, {0x03}},
	{0x80, 1, {0x23}},
	{0x82, 1, {0x09}},
	{0x83, 1, {0xA5}},
	{0x88, 1, {0x02}},
	{0xFF, 1, {0x2A}},
	{0xFB, 1, {0x01}},
	{0x00, 1, {0x91}},
	{0x03, 1, {0x20}},
	{0x06, 1, {0x0C}},
	{0x07, 1, {0x50}},
	{0x0A, 1, {0x60}},
	{0x0D, 1, {0x40}},
	{0x0E, 1, {0x02}},
	{0x11, 1, {0xF5}},
	{0x15, 1, {0x0F}},
	{0x16, 1, {0x71}},
	{0x19, 1, {0x0F}},
	{0x1A, 1, {0x45}},
	{0x1B, 1, {0x14}},
	{0x1D, 1, {0x36}},
	{0x1E, 1, {0x4D}},
	{0x1F, 1, {0x63}},
	{0x20, 1, {0x4D}},
	{0x28, 1, {0xF7}},
	{0x29, 1, {0x06}},
	{0x2A, 1, {0x55}},
	{0x2D, 1, {0x06}},
	{0x2F, 1, {0x01}},
	{0x30, 1, {0x45}},
	{0x31, 1, {0x3E}},
	{0x33, 1, {0x68}},
	{0x34, 1, {0xF9}},
	{0x35, 1, {0x33}},
	{0x36, 1, {0x15}},
	{0x37, 1, {0xF4}},
	{0x38, 1, {0x37}},
	{0x39, 1, {0x11}},
	{0x3A, 1, {0x45}},
	{0xEE, 1, {0x01}},
	{0xF0, 1, {0xFE}},
	{0xFF, 1, {0x2B}},
	{0xFB, 1, {0x01}},
	{0xB7, 1, {0x0B}},
	{0xB8, 1, {0x11}},
	{0xC0, 1, {0x01}},
	{0xFF, 1, {0xE0}},
	{0xFB, 1, {0x01}},
	{0x35, 1, {0x82}},
	{0xFF, 1, {0xF0}},
	{0xFB, 1, {0x01}},
	{0x1C, 1, {0x01}},
	{0x33, 1, {0x01}},
	{0x5A, 1, {0x00}},
	{0xFF, 1, {0xD0}},
	{0xFB, 1, {0x01}},
	{0x53, 1, {0x22}},
	{0x54, 1, {0x02}},
	{0xFF, 1, {0xC0}},
	{0xFB, 1, {0x01}},
	{0x9C, 1, {0x11}},
	{0x9D, 1, {0x11}},
	{0xFF, 1, {0xF0}},
	{0xFB, 1, {0x01}},
	{0xD2, 1, {0x52}},
	{0xFF, 1, {0x10}},
	{0xC0, 1, {0x00}},
	{0x11, 0, {}},
	{REGFLAG_DELAY, 120, {}},
	{0x29, 0, {}},
	{REGFLAG_DELAY, 20, {}},
};

static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28, 0, {}},
	{0x10, 0, {}},
	{REGFLAG_DELAY, 120, {}},
};

static void push_table(struct LCM_setting_table *table,
		       unsigned int count,
		       unsigned char force_update)
{
	unsigned int i;
	unsigned int cmd;

	for (i = 0; i < count; i++) {
		cmd = table[i].cmd;
		switch (cmd) {
		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;
		case REGFLAG_UDELAY:
			UDELAY(table[i].count);
			break;
		case REGFLAG_END_OF_TABLE:
			break;
		default:
			dsi_set_cmdq_V2(cmd, table[i].count,
					table[i].para_list, force_update);
			break;
		}
	}
}

static void lcm_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(struct LCM_UTIL_FUNCS));
}

static void lcm_get_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type = LCM_TYPE_DSI;
	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->physical_width = LCM_PHYSICAL_WIDTH / 1000;
	params->physical_height = LCM_PHYSICAL_HEIGHT / 1000;
	params->physical_width_um = LCM_PHYSICAL_WIDTH;
	params->physical_height_um = LCM_PHYSICAL_HEIGHT;

	params->lcm_if = LCM_INTERFACE_DSI0;
	params->lcm_cmd_if = LCM_INTERFACE_DSI0;

	params->dsi.mode = SYNC_PULSE_VDO_MODE;
	params->dsi.LANE_NUM = LCM_THREE_LANE;
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size = 256;
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active = 10;
	params->dsi.vertical_backporch = 10;
	params->dsi.vertical_frontporch = 54;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 20;
	params->dsi.horizontal_backporch = 22;
	params->dsi.horizontal_frontporch = 230;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	params->dsi.HS_ZERO = 35;
	params->dsi.PLL_CLOCK = 540;
	params->dsi.ssc_disable = 1;
	params->dsi.ssc_range = 4;
	params->dsi.IsCphy = 1;

	params->dsi.customization_esd_check_enable = 1;
	params->dsi.esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
	params->dsi.lcm_int_te_period = 683;

	params->dsi.dynamic_switch_mipi = 1;
	params->dsi.horizontal_frontporch_dyn = 236;
	params->dsi.PLL_CLOCK_dyn = 544;

#ifdef CONFIG_MTK_HIGH_FRAME_RATE
	params->dsi.dfps_enable = 1;
	params->dsi.dfps_default_fps = 9000;
	params->dsi.dfps_def_vact_tim_fps = 9000;
	params->dsi.dfps_num = 2;
	params->dsi.dfps_params[0].level = DFPS_LEVEL0;
	params->dsi.dfps_params[0].fps = 6000;
	params->dsi.dfps_params[0].vact_timing_fps = 9000;
	params->dsi.dfps_params[0].vertical_frontporch = 1321;
	params->dsi.dfps_params[1].level = DFPS_LEVEL1;
	params->dsi.dfps_params[1].fps = 9000;
	params->dsi.dfps_params[1].vact_timing_fps = 9000;
	params->dsi.dfps_params[1].vertical_frontporch = 54;
#endif
}

static void lcm_check_ret(const char *op, int ret)
{
	if (ret)
		pr_err("[LCM][ERROR] %s failed: %d\n", op, ret);
}

static void lcm_init_power(void)
{
	lcm_check_ret("bias_enp1",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCD_BIAS_ENP1));
	MDELAY(5);
	lcm_check_ret("bias_enn1",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCD_BIAS_ENN1));
	lcm_check_ret("i2c_bias_0x00", _lcm_i2c_write_bytes(0x00, 0x0F));
	MDELAY(1);
	lcm_check_ret("i2c_bias_0x01", _lcm_i2c_write_bytes(0x01, 0x0F));
	MDELAY(1);
	lcm_check_ret("i2c_bias_0x03", _lcm_i2c_write_bytes(0x03, 0x03));
	MDELAY(10);
}

static void lcm_suspend_power(void)
{
	lcm_check_ret("bias_enn0",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCD_BIAS_ENN0));
	MDELAY(5);
	lcm_check_ret("bias_enp0",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCD_BIAS_ENP0));
	is_power_suspend = true;
}

static void lcm_resume_power(void)
{
	if (is_power_suspend) {
		lcm_init_power();
		is_power_suspend = false;
	}
}

static void lcm_init(void)
{
	lcm_check_ret("rst_out1_a",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCM_RST_OUT1));
	MDELAY(5);
	lcm_check_ret("rst_out0",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCM_RST_OUT0));
	MDELAY(5);
	lcm_check_ret("rst_out1_b",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCM_RST_OUT1));
	MDELAY(15);

	push_table(init_setting,
		   sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	push_table(lcm_suspend_setting,
		   sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_resume(void)
{
	lcm_check_ret("resume_rst0_a",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCM_RST_OUT0));
	MDELAY(5);
	lcm_check_ret("resume_rst1_a",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCM_RST_OUT1));
	MDELAY(5);
	lcm_check_ret("resume_rst0_b",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCM_RST_OUT0));
	MDELAY(5);
	lcm_check_ret("resume_rst1_b",
		      disp_dts_gpio_select_state(DTS_GPIO_STATE_LCM_RST_OUT1));
	MDELAY(15);

	push_table(init_setting,
		   sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);
}

static unsigned int lcm_compare_id(void)
{
	return 1;
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
	return 1;
}

struct LCM_DRIVER nt36672c_fhdp_dsi_cphy_vdo_txd_boe_x695_lcm_drv = {
	.name = "nt36672c_fhdp_dsi_cphy_vdo_txd_boe_x695",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.compare_id = lcm_compare_id,
	.init_power = lcm_init_power,
	.resume_power = lcm_resume_power,
	.suspend_power = lcm_suspend_power,
	.ata_check = lcm_ata_check,
};
