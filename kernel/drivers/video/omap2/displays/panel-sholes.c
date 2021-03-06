#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

#include <mach/display.h>
#include <mach/dma.h>
#include <asm/atomic.h>

#define DEBUG
#ifdef DEBUG
#define DBG(format, ...) (printk(KERN_DEBUG "sholes-panel: " format, ## __VA_ARGS__))
#else
#define DBG(format, ...)
#endif

#define EDISCO_CMD_SOFT_RESET		0x01
#define EDISCO_CMD_ENTER_SLEEP_MODE	0x10
#define EDISCO_CMD_EXIT_SLEEP_MODE	0x11
#define EDISCO_CMD_SET_DISPLAY_ON	0x29
#define EDISCO_CMD_SET_DISPLAY_OFF	0x28
#define EDISCO_CMD_SET_COLUMN_ADDRESS	0x2A
#define EDISCO_CMD_SET_PAGE_ADDRESS	0x2B
#define EDISCO_CMD_SET_TEAR_ON		0x35
#define EDISCO_CMD_SET_TEAR_SCANLINE	0x44

#define EDISCO_CMD_VC   0
#define EDISCO_VIDEO_VC 1

#define EDISCO_LONG_WRITE	0x29
#define EDISCO_SHORT_WRITE_1	0x23
#define EDISCO_SHORT_WRITE_0	0x13

#define PANEL_OFF 0x0
#define PANEL_ENABLED 0x1
#define PANEL_UPDATED 0x2
#define PANEL_ON 0x3

static struct omap_video_timings sholes_panel_timings = {
	.x_res		= 480,
	.y_res		= 854,
	.hfp		= 44,
	.hsw		= 2,
	.hbp		= 38,
	.vfp		= 2,
	.vsw		= 1,
	.vbp		= 1,
	.w		= 46,
	.h 		= 82,
};

struct sholes_data {
	struct work_struct work;
	struct omap_dss_device *dssdev;
	wait_queue_head_t wait;
	atomic_t state;
};

static bool is_updated_or_off(struct sholes_data *data, int *state)
{
	/* if state is updated or off, return with lock held */
	*state = atomic_cmpxchg(&data->state, PANEL_UPDATED, PANEL_ON);
	if (*state == PANEL_UPDATED || *state == PANEL_OFF)
		return true;
	return false;
}


static void sholes_panel_display_on(struct work_struct *work)
{
	struct sholes_data *sholes_data = container_of(work, struct sholes_data,
						       work);
	u8 data;
	int state;

	wait_event(sholes_data->wait, is_updated_or_off(sholes_data, &state));

	if (state == PANEL_OFF) {
		DBG("cancel panel on\n");
		return;
	}

	DBG("panel on\n");
	sholes_data->dssdev->sync(sholes_data->dssdev);
	data = EDISCO_CMD_SET_DISPLAY_ON;
	dsi_bus_lock(); 
	dsi_vc_dcs_write(EDISCO_CMD_VC, &data, 1);
	dsi_bus_unlock();
}

static int sholes_panel_probe(struct omap_dss_device *dssdev)
{
	struct sholes_data *data;

	DBG("probe\n");
	dssdev->ctrl.pixel_size = 24;
	dssdev->panel.config = OMAP_DSS_LCD_TFT;
	dssdev->panel.config = OMAP_DSS_LCD_TFT;
	dssdev->panel.timings = sholes_panel_timings;
	data = kmalloc(sizeof(struct sholes_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	INIT_WORK(&data->work, sholes_panel_display_on);
	init_waitqueue_head(&data->wait);
#ifdef CONFIG_FB_OMAP2_MTD_LOGO
	atomic_set(&data->state, PANEL_OFF);
#else
	atomic_set(&data->state, PANEL_ON);
#endif
	data->dssdev = dssdev;
	dssdev->data = data;
	return 0;
}

static void sholes_panel_remove(struct omap_dss_device *dssdev)
{
	struct sholes_data *sholes_data = dssdev->data;

	atomic_set(&sholes_data->state, PANEL_OFF);
	wake_up(&sholes_data->wait);
	cancel_work_sync(&sholes_data->work);
	kfree(dssdev->data);
	return;
}

static int sholes_panel_enable(struct omap_dss_device *dssdev)
{
	u8 data[7];
	struct sholes_data *sholes_data = dssdev->data;
	int ret;

	DBG("enable\n");
	if (dssdev->platform_enable) {
		ret = dssdev->platform_enable(dssdev);
		if (ret)
			return ret;
	}

	/* turn of mcs register acces protection */
	data[0] = 0xb2;
	data[1] = 0x00;
	ret = dsi_vc_write(EDISCO_CMD_VC, EDISCO_SHORT_WRITE_1, data, 2);

	/* enable lane setting and test registers*/
	data[0] = 0xef;
	data[1] = 0x01;
	data[2] = 0x01;
	ret = dsi_vc_write(EDISCO_CMD_VC, EDISCO_LONG_WRITE, data, 3);

	/* 2nd param 61 = 1 line; 63 = 2 lanes */
	data[0] = 0xef;
	data[1] = 0x60;
	data[2] = 0x63;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 3);

	/* 2nd param 0 = WVGA; 1 = WQVGA */
	data[0] = 0xb3;
	data[1] = 0x00;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 2);

	/* Set dynamic backlight control and PWM; D[7:4] = PWM_DIV[3:0];*/
	/* D[3]=0 (PWM OFF);
	 * D[2]=0 (auto BL control OFF);
	 * D[1]=0 (Grama correction On);
	 * D[0]=0 (Enhanced Image Correction OFF) */
	data[0] = 0xb4;
	data[1] = 0x1f;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 2);

	/* set page, column address */
	data[0] = EDISCO_CMD_SET_PAGE_ADDRESS;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = (dssdev->panel.timings.y_res - 1) >> 8;
	data[4] = (dssdev->panel.timings.y_res - 1) & 0xff;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 5);
	if (ret)
		goto error;

	data[0] = EDISCO_CMD_SET_COLUMN_ADDRESS;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = (dssdev->panel.timings.x_res - 1) >> 8;
	data[4] = (dssdev->panel.timings.x_res - 1) & 0xff;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 5);
	if (ret)
		goto error;

	/* turn it on */
	data[0] = EDISCO_CMD_EXIT_SLEEP_MODE;
	//ret = dsi_vc_write(EDISCO_CMD_VC, EDISCO_SHORT_WRITE_0, data, 1);
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 1);

	mdelay(200);

	if (atomic_cmpxchg(&sholes_data->state, PANEL_OFF, PANEL_ENABLED) ==
	    PANEL_OFF) {
		DBG("panel enabled\n");
		schedule_work(&sholes_data->work);
	}
	return 0;
error:
	return -EINVAL;
}

static void sholes_panel_disable(struct omap_dss_device *dssdev)
{
	u8 data[1];
	struct sholes_data *sholes_data = dssdev->data;

	DBG("sholes_panel_ctrl_disable\n");
	atomic_set(&sholes_data->state, PANEL_OFF);
	wake_up(&sholes_data->wait);
	cancel_work_sync(&sholes_data->work);
	DBG("panel off\n");

	data[0] = EDISCO_CMD_SET_DISPLAY_OFF;
	dsi_vc_dcs_write(EDISCO_CMD_VC, data, 1);

	data[0] = EDISCO_CMD_ENTER_SLEEP_MODE;
	dsi_vc_dcs_write(EDISCO_CMD_VC, data, 1);
	msleep(120);

	if (dssdev->platform_disable)
		dssdev->platform_disable(dssdev);
}

static void sholes_panel_setup_update(struct omap_dss_device *dssdev,
				      u16 x, u16 y, u16 w, u16 h)
{

	u8 data[5];
	int ret;
	struct sholes_data *sholes_data = dssdev->data;

	if (atomic_cmpxchg(&sholes_data->state, PANEL_ENABLED, PANEL_UPDATED)
	    == PANEL_ENABLED) {
		DBG("panel updated\n");
		wake_up(&((struct sholes_data *)dssdev->data)->wait);
	}

	/* set page, column address */
	data[0] = EDISCO_CMD_SET_PAGE_ADDRESS;
	data[1] = y >> 8;
	data[2] = y & 0xff;
	data[3] = (y + h - 1) >> 8;
	data[4] = (y + h - 1) & 0xff;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 5);
	if (ret)
		return;

	data[0] = EDISCO_CMD_SET_COLUMN_ADDRESS;
	data[1] = x >> 8;
	data[2] = x & 0xff;
	data[3] = (x + w - 1) >> 8;
	data[4] = (x + w - 1) & 0xff;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 5);
	if (ret)
		return;
}

static int sholes_panel_enable_te(struct omap_dss_device *dssdev, bool enable)
{
	u8 data[3];
	int ret;

	data[0] = EDISCO_CMD_SET_TEAR_ON;
	data[1] = 0x00;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 2);
	if (ret)
		goto error;

	data[0] = EDISCO_CMD_SET_TEAR_SCANLINE;
	data[1] = 0x03;
	data[2] = 0x00;
	ret = dsi_vc_dcs_write(EDISCO_CMD_VC, data, 3);
	if (ret)
		goto error;

	DBG("edisco_ctrl_enable_te \n");
	return 0;

error:
	return -EINVAL;
}

static int sholes_panel_rotate(struct omap_dss_device *display, u8 rotate)
{
	return 0;
}

static int sholes_panel_mirror(struct omap_dss_device *display, bool enable)
{
	return 0;
}

static int sholes_panel_run_test(struct omap_dss_device *display, int test_num)
{
	return 0;
}

static int sholes_panel_suspend(struct omap_dss_device *dssdev)
{
	sholes_panel_disable(dssdev);
	return 0;
}

static int sholes_panel_resume(struct omap_dss_device *dssdev)
{
	return sholes_panel_enable(dssdev);
}

static struct omap_dss_driver sholes_panel_driver = {
	.probe = sholes_panel_probe,
	.remove = sholes_panel_remove,

	.enable = sholes_panel_enable,
	.disable = sholes_panel_disable,
	.suspend = sholes_panel_suspend,
	.resume = sholes_panel_resume,
	.setup_update = sholes_panel_setup_update,
	.enable_te = sholes_panel_enable_te,
	.set_rotate = sholes_panel_rotate,
	.set_mirror = sholes_panel_mirror,
	.run_test = sholes_panel_run_test,

	.driver = {
		.name = "sholes-panel",
		.owner = THIS_MODULE,
	},
};


static int __init sholes_panel_init(void)
{
	DBG("sholes_panel_init\n");
	omap_dss_register_driver(&sholes_panel_driver);
	return 0;
}

static void __exit sholes_panel_exit(void)
{
	DBG("sholes_panel_exit\n");

	omap_dss_unregister_driver(&sholes_panel_driver);
}

module_init(sholes_panel_init);
module_exit(sholes_panel_exit);

MODULE_AUTHOR("Rebecca Schultz Zavin <rebecca@android.com>");
MODULE_DESCRIPTION("Sholes Panel Driver");
MODULE_LICENSE("GPL");
