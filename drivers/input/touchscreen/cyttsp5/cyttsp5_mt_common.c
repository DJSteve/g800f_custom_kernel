/*
 * cyttsp5_mt_common.c
 * Cypress TrueTouch(TM) Standard Product V5 Multi-Touch Reports Module.
 * For use with Cypress Txx5xx parts.
 * Supported parts include:
 * TMA5XX
 *
 * Copyright (C) 2012-2013 Cypress Semiconductor
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, and only version 2, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Contact Cypress Semiconductor at www.cypress.com <ttdrivers@cypress.com>
 *
 */
#define CYTTSP5_DT2W

#include "cyttsp5_regs.h"
#include "cyttsp5_core.h"
#include <linux/input/mt.h>
#ifdef CYTTSP5_DT2W
#include <linux/wakelock.h>
#endif

#define CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP5_MT_B
#define CYTTSP5_TOUCHLOG_ENABLE 0


/*#define REPORT_XY_WHEN_LIFTOFF*/
#define FORCE_SATISFY_PALMPAUSE_FOR_LARGEOBJ
#ifdef CYTTSP5_DT2W
#define MS_TO_NS(x)	(x * 1E6L)

static void cyttsp5_dt2w_timerStart(struct cyttsp5_mt_data *md);
static void cyttsp5_dt2w_timerCancel(struct cyttsp5_mt_data *md);
#endif


#if TOUCH_BOOSTER
static void change_dvfs_lock(struct work_struct *work)
{
	struct cyttsp5_mt_data *md = container_of(work,
		struct cyttsp5_mt_data, work_dvfs_chg.work);

	mutex_lock(&md->dvfs_lock);

	if (md->boost_level == TSP_BOOSTER_LEVEL1) {
		tsp_debug_dbg(false, md->dev, "%s & Off all\n", __func__);

		if (pm_qos_request_active(&md->tsp_cpu_qos))
			pm_qos_remove_request(&md->tsp_cpu_qos);

		if (pm_qos_request_active(&md->tsp_mif_qos))
			pm_qos_remove_request(&md->tsp_mif_qos);

		if (pm_qos_request_active(&md->tsp_int_qos))
			pm_qos_remove_request(&md->tsp_int_qos);
	} else {
		if (pm_qos_request_active(&md->tsp_cpu_qos))
			pm_qos_update_request(&md->tsp_cpu_qos, 800000); /* CPU 800MHz */

		if (pm_qos_request_active(&md->tsp_mif_qos))
			pm_qos_update_request(&md->tsp_mif_qos, 267000); /* MIF 267MHz */

		if (pm_qos_request_active(&md->tsp_int_qos))
			pm_qos_update_request(&md->tsp_int_qos, 160000); /* INT 160MHz */

		tsp_debug_dbg(false, md->dev, "%s called!\n", __func__);
	}

	mutex_unlock(&md->dvfs_lock);
}
static void set_dvfs_off(struct work_struct *work)
{

	struct cyttsp5_mt_data *md = container_of(work,
				struct cyttsp5_mt_data, work_dvfs_off.work);

	mutex_lock(&md->dvfs_lock);

	if (pm_qos_request_active(&md->tsp_cpu_qos))
		pm_qos_remove_request(&md->tsp_cpu_qos);

	if (pm_qos_request_active(&md->tsp_mif_qos))
		pm_qos_remove_request(&md->tsp_mif_qos);

	if (pm_qos_request_active(&md->tsp_int_qos))
		pm_qos_remove_request(&md->tsp_int_qos);

	md->dvfs_lock_status = false;
	mutex_unlock(&md->dvfs_lock);

	tsp_debug_dbg(false, md->dev, "TSP DVFS Off %d\n", md->boost_level);
}

static void set_dvfs_lock(struct cyttsp5_mt_data *md, uint32_t on, bool mode)
{
	if (TSP_BOOSTER_DISABLE == md->boost_level)
		return;

	mutex_lock(&md->dvfs_lock);
	if (on == 0) {
		if (md->dvfs_lock_status) {
			schedule_delayed_work(&md->work_dvfs_off,
				msecs_to_jiffies(TOUCH_BOOSTER_OFF_TIME));
		}
	} else if (on == 1) {
		cancel_delayed_work(&md->work_dvfs_off);
		if (!md->dvfs_lock_status || mode) {
			if (md->dvfs_lock_status && mode) {
				cancel_delayed_work(&md->work_dvfs_chg);

				if (pm_qos_request_active(&md->tsp_cpu_qos))
					pm_qos_update_request(&md->tsp_cpu_qos, 1000000);
				else
					pm_qos_add_request(&md->tsp_cpu_qos, PM_QOS_CPU_FREQ_MIN, 1000000);

				if (pm_qos_request_active(&md->tsp_mif_qos))
					pm_qos_update_request(&md->tsp_mif_qos, 400000);
				else
					pm_qos_add_request(&md->tsp_mif_qos, PM_QOS_BUS_THROUGHPUT, 400000);

				if (pm_qos_request_active(&md->tsp_int_qos))
					pm_qos_update_request(&md->tsp_int_qos, 200000);
				else
					pm_qos_add_request(&md->tsp_int_qos, PM_QOS_DEVICE_THROUGHPUT, 200000);
			} else {
				pm_qos_add_request(&md->tsp_cpu_qos, PM_QOS_CPU_FREQ_MIN,1000000); /* CPU 1000MHz */
				pm_qos_add_request(&md->tsp_mif_qos, PM_QOS_BUS_THROUGHPUT, 400000); /* MIF 400MHz */
				pm_qos_add_request(&md->tsp_int_qos, PM_QOS_DEVICE_THROUGHPUT, 200000); /* INT 200MHz */
			}
			schedule_delayed_work(&md->work_dvfs_chg,
				msecs_to_jiffies(TOUCH_BOOSTER_CHG_TIME));

			tsp_debug_dbg(false, md->dev, "TSP DVFS On %d %d %d\n",
				md->boost_level, md->dvfs_lock_status, mode);

			md->dvfs_lock_status = true;
		}
	} else if (on == 2) {
		if (md->dvfs_lock_status) {
			cancel_delayed_work(&md->work_dvfs_off);
			cancel_delayed_work(&md->work_dvfs_chg);
			schedule_work(&md->work_dvfs_off.work);
		}
	}
	mutex_unlock(&md->dvfs_lock);
}
#endif

#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP5_MT_B
#ifdef SAMSUNG_PALM_MOTION
static void cyttsp5_final_sync(struct input_dev *input, int max_slots,
		int mt_sync_count, unsigned long *ids,
		u16 sumsize, bool palm)
{
	int t;

	for (t = 0; t < max_slots; t++) {
		if (test_bit(t, ids)) {
			input_mt_slot(input, t);
			input_report_abs(input, ABS_MT_SUMSIZE, sumsize);
			input_report_abs(input, ABS_MT_PALM, palm);
			continue;
		}
		input_mt_slot(input, t);
		input_mt_report_slot_state(input, MT_TOOL_FINGER, false);
	}

	input_sync(input);
}
#else
static void cyttsp5_final_sync(struct input_dev *input, int max_slots,
		int mt_sync_count, unsigned long *ids)
{
	int t;

	for (t = 0; t < max_slots; t++) {
		if (test_bit(t, ids))
			continue;
		input_mt_slot(input, t);
		input_mt_report_slot_state(input, MT_TOOL_FINGER, false);
	}

	input_sync(input);
}
#endif

static inline void cyttsp5_input_sync(struct input_dev *input){}

static void cyttsp5_input_report(struct input_dev *input, int sig,
		int t, int type)
{
	unsigned char tool_type = MT_TOOL_FINGER;

	input_mt_slot(input, t);

	if (type == CY_OBJ_STYLUS)
		tool_type = MT_TOOL_PEN;
	input_mt_report_slot_state(input, tool_type, true);
}

static void cyttsp5_report_slot_liftoff(struct cyttsp5_mt_data *md,
		int max_slots)
{
	int t;

	if (md->num_prv_tch == 0)
		return;

	for (t = 0; t < max_slots; t++) {
		input_mt_slot(md->input, t);
		input_mt_report_slot_state(md->input,
			MT_TOOL_FINGER, false);
	}
}

static int cyttsp5_input_register_device(struct input_dev *input,
		int max_slots)
{
	input_mt_init_slots(input, max_slots);
	return input_register_device(input);
}
#endif

static void cyttsp5_mt_lift_all(struct cyttsp5_mt_data *md)
{
	int max = MAX_TOUCH_ID_NUMBER;

#ifdef SAMSUNG_PALM_MOTION
	md->palm = false;
#endif

	if (md->num_prv_tch != 0) {
		tsp_debug_dbg(false, md->dev, "%s:\n", __func__);

		cyttsp5_report_slot_liftoff(md, max);
		input_sync(md->input);
		md->num_prv_tch = 0;
	}
#if TOUCH_BOOSTER
	if (md->touch_pressed_num != 0) {
		tsp_debug_err(false, md->dev, "%s force dvfs off\n", __func__);
		md->touch_pressed_num = 0;
		set_dvfs_lock(md, 0, false);
	}
#endif
}

static void cyttsp5_get_touch_axis(struct cyttsp5_mt_data *md,
	int *axis, int size, int max, u8 *xy_data, int bofs)
{
	int nbyte;
	int next;

	for (nbyte = 0, *axis = 0, next = 0; nbyte < size; nbyte++) {
		dev_vdbg(md->dev,
			"%s: *axis=%02X(%d) size=%d max=%08X xy_data=%p"
			" xy_data[%d]=%02X(%d) bofs=%d\n",
			__func__, *axis, *axis, size, max, xy_data, next,
			xy_data[next], xy_data[next], bofs);
		*axis = *axis + ((xy_data[next] >> bofs) << (nbyte * 8));
		next++;
	}

	*axis &= max - 1;

	dev_vdbg(md->dev,
		"%s: *axis=%02X(%d) size=%d max=%08X xy_data=%p"
		" xy_data[%d]=%02X(%d)\n",
		__func__, *axis, *axis, size, max, xy_data, next,
		xy_data[next], xy_data[next]);
}

static void cyttsp5_get_touch_hdr(struct cyttsp5_mt_data *md,
	struct cyttsp5_touch *touch, u8 *xy_mode)
{
	struct device *dev = md->dev;
	struct cyttsp5_sysinfo *si = md->si;
	enum cyttsp5_tch_hdr hdr;

	for (hdr = CY_TCH_TIME; hdr < CY_TCH_NUM_HDR; hdr++) {
		if (!si->tch_hdr[hdr].report)
			continue;
		cyttsp5_get_touch_axis(md, &touch->hdr[hdr],
			si->tch_hdr[hdr].size,
			si->tch_hdr[hdr].max,
			xy_mode + si->tch_hdr[hdr].ofs,
			si->tch_hdr[hdr].bofs);
		dev_vdbg(dev, "%s: get %s=%04X(%d)\n", __func__,
			cyttsp5_tch_hdr_string[hdr],
			touch->hdr[hdr], touch->hdr[hdr]);
	}
#if CYTTSP5_TOUCHLOG_ENABLE
	tsp_debug_dbg(false, dev,
		"%s: time=%X tch_num=%d lo=%d noise=%d counter=%d\n",
		__func__,
		touch->hdr[CY_TCH_TIME],
		touch->hdr[CY_TCH_NUM],
		touch->hdr[CY_TCH_LO],
		touch->hdr[CY_TCH_NOISE],
		touch->hdr[CY_TCH_COUNTER]);
#endif
}

static void cyttsp5_get_touch(struct cyttsp5_mt_data *md,
	struct cyttsp5_touch *touch, u8 *xy_data)
{
	struct device *dev = md->dev;
	struct cyttsp5_sysinfo *si = md->si;
	enum cyttsp5_tch_abs abs;
	int tmp;
	bool flipped;

	for (abs = CY_TCH_X; abs < CY_TCH_NUM_ABS; abs++) {
		if (!si->tch_abs[abs].report)
			continue;
		cyttsp5_get_touch_axis(md, &touch->abs[abs],
			si->tch_abs[abs].size,
			si->tch_abs[abs].max,
			xy_data + si->tch_abs[abs].ofs,
			si->tch_abs[abs].bofs);
		dev_vdbg(dev, "%s: get %s=%04X(%d)\n", __func__,
			cyttsp5_tch_abs_string[abs],
			touch->abs[abs], touch->abs[abs]);
	}

	if (md->pdata->flags & CY_MT_FLAG_FLIP) {
		tmp = touch->abs[CY_TCH_X];
		touch->abs[CY_TCH_X] = touch->abs[CY_TCH_Y];
		touch->abs[CY_TCH_Y] = tmp;
		flipped = true;
	} else
		flipped = false;

	if (md->pdata->flags & CY_MT_FLAG_INV_X) {
		if (flipped)
			touch->abs[CY_TCH_X] = si->sensing_conf_data.res_y -
				touch->abs[CY_TCH_X];
		else
			touch->abs[CY_TCH_X] = si->sensing_conf_data.res_x -
				touch->abs[CY_TCH_X];
	}
	if (md->pdata->flags & CY_MT_FLAG_INV_Y) {
		if (flipped)
			touch->abs[CY_TCH_Y] = si->sensing_conf_data.res_x -
				touch->abs[CY_TCH_Y];
		else
			touch->abs[CY_TCH_Y] = si->sensing_conf_data.res_y -
				touch->abs[CY_TCH_Y];
	}

	dev_vdbg(dev, "%s: flip=%s inv-x=%s inv-y=%s x=%04X(%d) y=%04X(%d)\n",
		__func__, flipped ? "true" : "false",
		md->pdata->flags & CY_MT_FLAG_INV_X ? "true" : "false",
		md->pdata->flags & CY_MT_FLAG_INV_Y ? "true" : "false",
		touch->abs[CY_TCH_X], touch->abs[CY_TCH_X],
		touch->abs[CY_TCH_Y], touch->abs[CY_TCH_Y]);
}

#define ABS_PARAM(_abs, _param) md->pdata->frmwrk->abs[((_abs) * CY_NUM_ABS_SET) + (_param)]

static inline void print_log(struct device *dev,
		struct cyttsp5_touch *tch, int t)
{
#if CYTTSP5_TOUCHLOG_ENABLE
	if (tch->abs[CY_TCH_E] == CY_EV_LIFTOFF)
		tsp_debug_dbg(false, dev, "%s: t=%d e=%d lift-off\n",
			__func__, t, tch->abs[CY_TCH_E]);
	tsp_debug_dbg(false, dev,
		"%s: t=%d x=%d y=%d z=%d M=%d m=%d o=%d e=%d obj=%d tip=%d\n",
		__func__, t,
		tch->abs[CY_TCH_X],
		tch->abs[CY_TCH_Y],
		tch->abs[CY_TCH_P],
		tch->abs[CY_TCH_MAJ],
		tch->abs[CY_TCH_MIN],
		tch->abs[CY_TCH_OR],
		tch->abs[CY_TCH_E],
		tch->abs[CY_TCH_O],
		tch->abs[CY_TCH_TIP]);
#else//CYTTSP5_TOUCHLOG_ENABLE
#ifdef CONFIG_SAMSUNG_PRODUCT_SHIP
	if (tch->abs[CY_TCH_O] == CY_OBJ_HOVER) {
		if (tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN)
			tsp_debug_info(true, dev, "P [H][%d]\n", t);
		else if (tch->abs[CY_TCH_E] == CY_EV_LIFTOFF)
			tsp_debug_info(true, dev, "R [H][%d]\n", t);
	} else {
		if (tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN)
			tsp_debug_info(true, dev, "P [%d]\n", t);
		else if (tch->abs[CY_TCH_E] == CY_EV_LIFTOFF)
			tsp_debug_info(true, dev, "R [%d]\n", t);
	}
#else
	if (tch->abs[CY_TCH_O] == CY_OBJ_HOVER) {
		if (tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN)
			tsp_debug_info(true, dev,
				"P [H][%d][%d] x=%d y=%d z=%d M=%d m=%d\n",
				t, tch->abs[CY_TCH_O],
				tch->abs[CY_TCH_X], tch->abs[CY_TCH_Y],
				tch->abs[CY_TCH_P], tch->abs[CY_TCH_MAJ],
				tch->abs[CY_TCH_MIN]);
		else if (tch->abs[CY_TCH_E] == CY_EV_LIFTOFF)
			tsp_debug_info(true, dev,
				"R [H][%d][%d] x=%d y=%d z=%d M=%d m=%d\n",
				t, tch->abs[CY_TCH_O],
				tch->abs[CY_TCH_X], tch->abs[CY_TCH_Y],
				tch->abs[CY_TCH_P], tch->abs[CY_TCH_MAJ],
				tch->abs[CY_TCH_MIN]);
#if 0
		else
			tsp_debug_info(true, dev,
				"M [H][%d] x=%d y=%d z=%d M=%d m=%d\n",
				t, tch->abs[CY_TCH_O],
				tch->abs[CY_TCH_X], tch->abs[CY_TCH_Y],
				tch->abs[CY_TCH_P], tch->abs[CY_TCH_MAJ],
				tch->abs[CY_TCH_MIN]);
#endif
	} else {
		if (tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN)
			tsp_debug_info(true, dev,
				"P [%d][%d] x=%d y=%d z=%d M=%d m=%d\n",
				t, tch->abs[CY_TCH_O],
				tch->abs[CY_TCH_X], tch->abs[CY_TCH_Y],
				tch->abs[CY_TCH_P], tch->abs[CY_TCH_MAJ],
				tch->abs[CY_TCH_MIN]);
		else if (tch->abs[CY_TCH_E] == CY_EV_LIFTOFF)
			tsp_debug_info(true, dev,
				"R [%d][%d] x=%d y=%d z=%d M=%d m=%d\n",
				t, tch->abs[CY_TCH_O],
				tch->abs[CY_TCH_X], tch->abs[CY_TCH_Y],
				tch->abs[CY_TCH_P], tch->abs[CY_TCH_MAJ],
				tch->abs[CY_TCH_MIN]);
	}
#endif
#endif//CYTTSP5_TOUCHLOG_ENABLE
}

#ifdef SAMSUNG_TOUCH_MODE
static inline void manage_touch_mode(struct cyttsp5_mt_data *md,
		struct cyttsp5_touch *tch)
{
	if (tch->abs[CY_TCH_O] == CY_OBJ_HOVER) {
		if (tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN) {
		/*
		 * First hover touch event.
		 * F/W should send hover information in the first touch buffer
		 */
			input_mt_slot(md->input, 0);
			input_mt_report_slot_state(md->input, MT_TOOL_FINGER, 1);

			input_report_key(md->input, BTN_TOUCH, 0);
			input_report_key(md->input, BTN_TOOL_FINGER, 1);
			input_report_abs(md->input, ABS_MT_DISTANCE, tch->abs[CY_TCH_P]);
		} else if (tch->abs[CY_TCH_E] == CY_EV_LIFTOFF) {
		/*
		 *  Release hover slot by force.
		 *  This will change event from hover to normal finger immediately
		 */
			input_mt_slot(md->input, 0);
			input_mt_report_slot_state(md->input, MT_TOOL_FINGER, 0);
			input_sync(md->input);
		}
	} else {
		if (tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN) {
			input_report_key(md->input, BTN_TOUCH, 1);
			input_report_key(md->input, BTN_TOOL_FINGER, 1);
		}
	}

	if (md->glove_switch &&
		tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN) {
		input_report_switch(md->input,
					SW_GLOVE, md->glove_enable);
		md->glove_switch = false;
		tsp_debug_dbg(true, md->dev, "%s: glove switch %d\n", __func__,
			md->glove_enable);
	}

	if (tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN) {
		if ((!md->glove_enable && tch->abs[CY_TCH_O] == CY_OBJ_GLOVE) ||
		    (md->glove_enable &&
				(tch->abs[CY_TCH_O] == CY_OBJ_STANDARD_FINGER ||
				tch->abs[CY_TCH_O] == CY_OBJ_STYLUS))) {
			md->glove_enable = !md->glove_enable;
			tsp_debug_dbg(true, md->dev, "%s: ****** glove mode %s\n",
				__func__, md->glove_enable ? "on " : "off");
			input_report_switch(md->input,
				SW_GLOVE, md->glove_enable);
		}
	}
}

void cyttsp5_mt_glove_enable(struct device *dev, bool enable)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md;
	if (cd == NULL)
		return;
	md = &cd->md;
	if (md == NULL)
		return;

	tsp_debug_dbg(true, dev, "%s: %d\n", __func__, enable);
	md->glove_enable = enable;
	md->glove_switch = true;
}
#endif

#ifdef SAMSUNG_PALM_MOTION
static void inline scale_sum_size(struct cyttsp5_mt_data *md,
	u16 *sumsize)
{
//	struct device *dev = md->dev;

	//*sumsize /= 2.5;
	*sumsize *= 2;
	*sumsize /= 5;

	if (*sumsize > 255)
		*sumsize = 255;

//	tsp_debug_dbg(false, dev, "%s: sumsize=%d\n", __func__, *sumsize);
}
#endif

#if defined(REPORT_XY_WHEN_LIFTOFF)
static void inline update_xy_before_liftoff(struct cyttsp5_mt_data *md,
	int t, struct cyttsp5_touch *tch)
{
	int j, sig, value;
	input_mt_slot(md->input, t);
	for (j = 0; j <= CY_ABS_Y_OST; j++) {
		sig = ABS_PARAM(CY_ABS_X_OST + j, CY_SIGNAL_OST);
		value = tch->abs[CY_TCH_X + j];
		input_report_abs(md->input, sig, value);
	}
	input_sync(md->input);
}
#endif

#ifdef FORCE_SATISFY_PALMPAUSE_FOR_LARGEOBJ
static void forceSatisfyPalmPause(struct cyttsp5_mt_data *md,
		struct cyttsp5_touch *tch, int t)
{
	int x, y;

	tsp_debug_dbg(false, md->dev, "%s: \n", __func__);

	switch (t) {
	case 0: x = 10;y = 10; break;
	case 1: x = 700;y = 10; break;
	case 2: x = 700;y = 1200; break;
	case 3: x = 10;y = 1200; break;
	default: x = 10;y = 10; break;
	}
	tch->abs[CY_TCH_T] = t;
	tch->abs[CY_TCH_X] = x;
	tch->abs[CY_TCH_Y] = y;
	tch->abs[CY_TCH_P] = 10;
	tch->abs[CY_TCH_E] = CY_EV_TOUCHDOWN;
	tch->abs[CY_TCH_O] = CY_OBJ_STANDARD_FINGER;
	tch->abs[CY_TCH_MAJ] = 30;
	tch->abs[CY_TCH_MIN] = 30;
	tch->abs[CY_TCH_OR] = 0;
}
#endif

static void cyttsp5_get_mt_touches(struct cyttsp5_mt_data *md,
		struct cyttsp5_touch *tch, int num_cur_tch)
{
	struct device *dev = md->dev;
	struct cyttsp5_sysinfo *si = md->si;
	int sig, value;
	int i, j, t = 0;
	DECLARE_BITMAP(ids, MAX_TOUCH_ID_NUMBER);
	int mt_sync_count = 0;
#ifdef SAMSUNG_PALM_MOTION
	u16 sumsize = 0;
	u16 sum_maj_mnr;
	bool hover = 0;
#endif
#if TOUCH_BOOSTER
	u8 touch_num = 0;
	bool booster_status = false;
#endif

	bitmap_zero(ids, MAX_TOUCH_ID_NUMBER);
	memset(tch->abs, 0, sizeof(tch->abs));

	for (i = 0; i < num_cur_tch; i++) {
#ifdef FORCE_SATISFY_PALMPAUSE_FOR_LARGEOBJ
		if (md->largeobj)
			forceSatisfyPalmPause(md, tch, i);
		else
#endif
		cyttsp5_get_touch(md, tch, si->xy_data +
			(i * si->desc.tch_record_size));

		/*  Discard proximity event */
		if (tch->abs[CY_TCH_O] == CY_OBJ_PROXIMITY) {
			dev_vdbg(dev, "%s: Discarding proximity event\n",
					__func__);
			continue;
		} else if (tch->abs[CY_TCH_O] == CY_OBJ_HOVER) {
			tch->abs[CY_TCH_P] = 0;
		}

		if (tch->abs[CY_TCH_T] < ABS_PARAM(CY_ABS_ID_OST, CY_MIN_OST) ||
			tch->abs[CY_TCH_T] > ABS_PARAM(CY_ABS_ID_OST, CY_MAX_OST)) {
			tsp_debug_err(true, dev, "%s: tch=%d -> bad trk_id=%d max_id=%d\n",
				__func__, i, tch->abs[CY_TCH_T],
				ABS_PARAM(CY_ABS_ID_OST, CY_MAX_OST));
			cyttsp5_input_sync(md->input);
			mt_sync_count++;
			continue;
		}

		/* use 0 based track id's */
		sig = ABS_PARAM(CY_ABS_ID_OST, CY_SIGNAL_OST);
		if (sig != CY_IGNORE_VALUE) {
			t = tch->abs[CY_TCH_T] - ABS_PARAM(CY_ABS_ID_OST, CY_MIN_OST);

			if (t >= MAX_TOUCH_NUMBER) {
				tsp_debug_dbg(true, dev, "%s: t=%d exceeds max touch num\n",
					__func__, t);
				goto cyttsp5_get_mt_touches_pr_tch; // means continue for()
			}

#ifdef SAMSUNG_TOUCH_MODE
			manage_touch_mode(md, tch);
#endif

#if TOUCH_BOOSTER
			if ((tch->abs[CY_TCH_O] != CY_OBJ_HOVER) &&
				(tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN))
				booster_status = true;
#endif

			if (tch->abs[CY_TCH_E] == CY_EV_LIFTOFF) {
#if defined(REPORT_XY_WHEN_LIFTOFF)
				update_xy_before_liftoff(md, t, tch);
#endif
				goto cyttsp5_get_mt_touches_pr_tch;
			}

#ifdef SAMSUNG_TOUCH_MODE
			cyttsp5_input_report(md->input, sig, t,
				((tch->abs[CY_TCH_O] == CY_OBJ_STYLUS)
				 && !md->stylus_enable) ? CY_OBJ_STANDARD_FINGER :
				 tch->abs[CY_TCH_O]);
#else
			cyttsp5_input_report(md->input, sig,
						t, tch->abs[CY_TCH_O]);
#endif
			__set_bit(t, ids);
		}
#if TOUCH_BOOSTER
		if (tch->abs[CY_TCH_O] != CY_OBJ_HOVER)
			touch_num++;
#endif
		/* all devices: position and pressure fields */
		for (j = 0; j <= CY_ABS_W_OST; j++) {
			if (!si->tch_abs[j].report)
				continue;
			sig = ABS_PARAM(CY_ABS_X_OST + j, CY_SIGNAL_OST);
			if (sig == CY_IGNORE_VALUE)
				continue;
			value = tch->abs[CY_TCH_X + j];
			input_report_abs(md->input, sig, value);
		}
		

		/* Get the extended touch fields */
#ifdef SAMSUNG_TOUCH_MODE
		if (tch->abs[CY_TCH_O] == CY_OBJ_HOVER) {
			mt_sync_count++;
			hover = 1;
			goto cyttsp5_get_mt_touches_pr_tch;
		}
#endif
#ifdef SAMSUNG_PALM_MOTION
		sum_maj_mnr = 0;
#endif
		for (j = 0; j < CY_NUM_EXT_TCH_FIELDS; j++) {
			if (!si->tch_abs[j].report)
				continue;
			sig = ABS_PARAM((CY_ABS_MAJ_OST + j), CY_SIGNAL_OST);
			if (sig == CY_IGNORE_VALUE)
				continue;
			value = tch->abs[CY_TCH_MAJ + j];
			input_report_abs(md->input, sig, value);

#ifdef SAMSUNG_PALM_MOTION
			if (sig == ABS_MT_TOUCH_MAJOR || sig == ABS_MT_TOUCH_MINOR) {
				sumsize += value;
				sum_maj_mnr += value;
			}
#endif
		}

		cyttsp5_input_sync(md->input);
		mt_sync_count++;
		
#ifdef CYTTSP5_DT2W
		if ((md->dt2w_status) && !(md->dt2w_keyflag > 0) && (md->dt2w_active))
		{
#if TOUCH_BOOSTER
			touch_num = 0;
			booster_status = false;
#endif
			if ((tch->abs[CY_TCH_O] != CY_OBJ_HOVER) &&
				(tch->abs[CY_TCH_E] == CY_EV_TOUCHDOWN))
			{
				cyttsp5_pumpSensorData();
				md->dt2w_touchCount++;
				tsp_debug_dbg(true, dev, "%s:DTW2 Active! Touchdown detected! # %d\n", __func__, md->dt2w_touchCount);
				if (md->dt2w_touchCount > 1) {
					cyttsp5_dt2w_timerCancel(md);
					if ((abs(md->dt2w_x - tch->abs[CY_TCH_X]) < 80) && (abs(md->dt2w_y - tch->abs[CY_TCH_Y]) < 80))
					{
						cyttsp5_vibrate(60);
						tsp_debug_dbg(true, dev, "%s:DTW2 Active! Initiate Power!\n", __func__);
						md->dt2w_keyflag = 1;
						cyttsp5_presspwr();
					} else {
						md->dt2w_touchCount = 1;
						cyttsp5_dt2w_timerStart(md);
					}
				} else {
					cyttsp5_dt2w_timerStart(md);
				}
				md->dt2w_x = tch->abs[CY_TCH_X];
				md->dt2w_y = tch->abs[CY_TCH_Y];
			}
		}
#endif

cyttsp5_get_mt_touches_pr_tch:
		print_log(dev, tch, t);
	}//for (i = 0; i < num_cur_tch; i++)

#ifdef SAMSUNG_PALM_MOTION
	if (hover)
		sumsize = 1;
	else
		scale_sum_size(md, &sumsize);

	cyttsp5_final_sync(md->input, MAX_TOUCH_ID_NUMBER,
		mt_sync_count, ids,
		sumsize, md->palm);
#else
	cyttsp5_final_sync(md->input, MAX_TOUCH_ID_NUMBER,
		mt_sync_count, ids);
#endif
#if TOUCH_BOOSTER
	if (touch_num != md->touch_pressed_num) {
//		tsp_debug_dbg(false, dev, "%s: touch num = (%d -> %d)\n",
//			__func__, md->touch_pressed_num, touch_num);
		md->touch_pressed_num = touch_num;
	}

	if (!!md->touch_pressed_num)
		set_dvfs_lock(md, 1, booster_status);
	else
		set_dvfs_lock(md, 0, false);
#endif
	md->num_prv_tch = num_cur_tch;

	return;
}

/* read xy_data for all current touches */
static int cyttsp5_xy_worker(struct cyttsp5_mt_data *md)
{
	struct device *dev = md->dev;
	struct cyttsp5_sysinfo *si = md->si;
	struct cyttsp5_touch tch;
	u8 num_cur_tch;

	cyttsp5_get_touch_hdr(md, &tch, si->xy_mode + 3);

	num_cur_tch = tch.hdr[CY_TCH_NUM];
	if (num_cur_tch > MAX_TOUCH_ID_NUMBER) {
		tsp_debug_err(true, dev, "%s: Num touch err detected (n=%d)\n",
			__func__, num_cur_tch);
		num_cur_tch = MAX_TOUCH_ID_NUMBER;
	}

	if (tch.hdr[CY_TCH_LO]) {
		tsp_debug_dbg(true, dev, "%s: Large area detected\n", __func__);
		if (md->pdata->flags & CY_MT_FLAG_NO_TOUCH_ON_LO)
			num_cur_tch = 0;
#ifdef SAMSUNG_PALM_MOTION
		md->palm = true;
#endif
	}

#ifdef FORCE_SATISFY_PALMPAUSE_FOR_LARGEOBJ
	if (!md->largeobj) {
		if (tch.hdr[CY_TCH_LO]) {
			md->largeobj = true;
			cyttsp5_get_mt_touches(md, &tch, 4);
			goto skip_num_cur_tch;
		}
	} else {
		if (!num_cur_tch && !tch.hdr[CY_TCH_LO]) {
			md->largeobj = false;
			cyttsp5_mt_lift_all(md);
		}
		goto skip_num_cur_tch;
	}
#endif

	/* extract xy_data for all currently reported touches */
	dev_vdbg(dev, "%s: extract data num_cur_tch=%d\n", __func__,
		num_cur_tch);
	if (num_cur_tch)
		cyttsp5_get_mt_touches(md, &tch, num_cur_tch);
	else
		cyttsp5_mt_lift_all(md);

#ifdef FORCE_SATISFY_PALMPAUSE_FOR_LARGEOBJ
skip_num_cur_tch:
#endif

	return 0;
}

static void cyttsp5_mt_send_dummy_event(struct cyttsp5_mt_data *md)
{
	unsigned long ids = 0;

	/* for easy wakeup */
	cyttsp5_input_report(md->input, ABS_MT_TRACKING_ID,
			0, CY_OBJ_STANDARD_FINGER);
	cyttsp5_input_sync(md->input);
#ifdef SAMSUNG_PALM_MOTION
	cyttsp5_final_sync(md->input, 0, 1, &ids, 1, 0);
	cyttsp5_report_slot_liftoff(md, 1);
	cyttsp5_final_sync(md->input, 1, 1, &ids, 1, 0);
#else
	cyttsp5_final_sync(md->input, 0, 1, &ids);
	cyttsp5_report_slot_liftoff(md, 1);
	cyttsp5_final_sync(md->input, 1, 1, &ids);
#endif
}

static int cyttsp5_mt_attention(struct device *dev)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;
	int rc;

	if (md->si->xy_mode[2] !=  md->si->desc.tch_report_id)
		return 0;

	mutex_lock(&md->mt_lock);
	if (md->prevent_touch) {
		mutex_unlock(&md->mt_lock);

		tsp_debug_dbg(true, dev, "%s: touch is now prevented\n", __func__);
		return 0;
	}

	/* core handles handshake */
	rc = cyttsp5_xy_worker(md);
	mutex_unlock(&md->mt_lock);
	if (rc < 0)
		tsp_debug_err(true, dev, "%s: xy_worker error r=%d\n", __func__, rc);

	return rc;
}

static int cyttsp5_mt_wake_attention(struct device *dev)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;

	mutex_lock(&md->mt_lock);
	cyttsp5_mt_send_dummy_event(md);
	mutex_unlock(&md->mt_lock);
	return 0;
}

static int cyttsp5_startup_attention(struct device *dev)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;

	mutex_lock(&md->mt_lock);
	cyttsp5_mt_lift_all(md);
	mutex_unlock(&md->mt_lock);

	return 0;
}

void cyttsp5_mt_prevent_touch(struct device *dev, bool prevent)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md;
	if (cd == NULL)
		return;
	md = &cd->md;
	if (md == NULL)
		return;

	tsp_debug_dbg(true, dev, "%s: %d\n", __func__, prevent);

	mutex_lock(&md->mt_lock);
	md->prevent_touch = prevent;
	if (prevent)
		cyttsp5_mt_lift_all(md);
	mutex_unlock(&md->mt_lock);
}

void cyttsp5_mt_stylus_enable(struct device *dev, bool enable)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md;
	if (cd == NULL)
		return;
	md = &cd->md;
	if (md == NULL)
		return;

	tsp_debug_dbg(true, dev, "%s: %d\n", __func__, enable);

	mutex_lock(&md->mt_lock);
	if (md->stylus_enable != enable) {
		md->stylus_enable = enable;
		cyttsp5_mt_lift_all(md);
	}
	mutex_unlock(&md->mt_lock);
}

static int cyttsp5_mt_open(struct input_dev *input)
{
	struct device *dev = input->dev.parent;
#ifdef CYTTSP5_DT2W
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;
	tsp_debug_dbg(true, dev, "%s:Open input device DT2W: %d %d %d\n", __func__, md->dt2w_status,md->dt2w_active,md->dt2w_cover);
	if ((md->dt2w_status) && (md->dt2w_active) && !(md->dt2w_cover))
	{
		tsp_debug_dbg(true, dev, "%s:Touchscreen already active due to DT2W, release wakelock\n", __func__);
		cyttsp5_dt2w_timerCancel(md);
		wake_unlock(&md->dt2w_wake_lock);
		md->dt2w_active = 0;
		return 0;
	}
#endif

	tsp_debug_dbg(true, dev, "%s:\n", __func__);

	/* pm_runtime_get_sync(dev); */

	dev_vdbg(dev, "%s: setup subscriptions\n", __func__);

	/* set up touch call back */
	_cyttsp5_subscribe_attention(dev, CY_ATTEN_IRQ, CY_MODULE_MT,
		cyttsp5_mt_attention, CY_MODE_OPERATIONAL);

	/* set up startup call back */
	_cyttsp5_subscribe_attention(dev, CY_ATTEN_STARTUP, CY_MODULE_MT,
		cyttsp5_startup_attention, 0);

	/* set up wakeup call back */
	_cyttsp5_subscribe_attention(dev, CY_ATTEN_WAKE, CY_MODULE_MT,
		cyttsp5_mt_wake_attention, 0);

	cyttsp5_core_resume(dev);
	return 0;
}
#ifdef CYTTSP5_DT2W
static void cyttsp5_factory_command(struct device *_dev, const char *command, int value);
#endif
static void cyttsp5_mt_close(struct input_dev *input)
{
	struct device *dev = input->dev.parent;
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;
	
#ifdef CYTTSP5_DT2W
	tsp_debug_dbg(true, dev, "%s:Close input device DT2W: %d %d %d\n", __func__, md->dt2w_status,md->dt2w_active,md->dt2w_cover);
	if ((md->dt2w_status) && !(md->dt2w_cover))
	{
		tsp_debug_dbg(true, dev, "%s:Prohibit touchscreen shutdown for DT2W\n", __func__);
		cyttsp5_factory_command(dev, "clear_cover_mode", 0);
		cyttsp5_factory_command(dev, "hover_enable", 0);
		md->dt2w_active = 1;
		md->dt2w_keyflag = 0;
		md->dt2w_touchCount = 0;
		wake_lock(&md->dt2w_wake_lock);
		tsp_debug_dbg(true, dev, "%s:Close input device DT2W complete, hold wakelock: %d %d %d\n", __func__, md->dt2w_status,md->dt2w_active,md->dt2w_cover);
		return;
	}


#endif

	tsp_debug_dbg(true, dev, "%s:\n", __func__);

#if TOUCH_BOOSTER
	if (md->touch_pressed_num != 0) {
		tsp_debug_err(false, md->dev, "%s force dvfs off\n", __func__);
		md->touch_pressed_num = 0;
		set_dvfs_lock(md, 2, false);
	}
#endif
#ifdef SAMSUNG_TOUCH_MODE
	input_report_switch(md->input,
		SW_GLOVE, false);
	md->glove_switch = true;
#endif

	_cyttsp5_unsubscribe_attention(dev, CY_ATTEN_IRQ, CY_MODULE_MT,
		cyttsp5_mt_attention, CY_MODE_OPERATIONAL);

	_cyttsp5_unsubscribe_attention(dev, CY_ATTEN_STARTUP, CY_MODULE_MT,
		cyttsp5_startup_attention, 0);

	mutex_lock(&md->mt_lock);
	md->prevent_touch = 0;
	mutex_unlock(&md->mt_lock);

	/* pm_runtime_put(dev); */
	cyttsp5_core_suspend(dev);
	
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void cyttsp5_mt_early_suspend(struct early_suspend *h)
{
	struct cyttsp5_mt_data *md =
		container_of(h, struct cyttsp5_mt_data, es);
	struct device *dev = md->dev;

	/* pm_runtime_put_sync(dev); */
	cyttsp5_core_suspend(dev);

	mutex_lock(&md->mt_lock);
	md->is_suspended = true;
	mutex_unlock(&md->mt_lock);
}

static void cyttsp5_mt_late_resume(struct early_suspend *h)
{
	struct cyttsp5_mt_data *md =
		container_of(h, struct cyttsp5_mt_data, es);
	struct device *dev = md->dev;

	/*pm_runtime_get(dev); */
	cyttsp5_core_resume(dev);

	mutex_lock(&md->mt_lock);
	md->is_suspended = false;
	mutex_unlock(&md->mt_lock);
}

static void cyttsp5_setup_early_suspend(struct cyttsp5_mt_data *md)
{
	md->es.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	md->es.suspend = cyttsp5_mt_early_suspend;
	md->es.resume = cyttsp5_mt_late_resume;

	register_early_suspend(&md->es);
}
#endif

static int cyttsp5_setup_input_device(struct device *dev)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;
	int signal = CY_IGNORE_VALUE;
	int max_x, max_y, max_p, min, max;
	int max_x_tmp, max_y_tmp;
	int i;
	int rc;

	dev_vdbg(dev, "%s: Initialize event signals\n", __func__);
	set_bit(EV_SYN, md->input->evbit);
	set_bit(EV_ABS, md->input->evbit);
	set_bit(EV_KEY, md->input->evbit);

#ifdef INPUT_PROP_DIRECT
	set_bit(INPUT_PROP_DIRECT, md->input->propbit);
#endif

#ifdef SAMSUNG_TOUCH_MODE
	set_bit(BTN_TOUCH, md->input->keybit);
	set_bit(BTN_TOOL_FINGER, md->input->keybit);
	input_set_capability(md->input, EV_SW, SW_GLOVE);
#endif

	/* If virtualkeys enabled, don't use all screen */
	if (md->pdata->flags & CY_MT_FLAG_VKEYS) {
		max_x_tmp = md->pdata->vkeys_x;
		max_y_tmp = md->pdata->vkeys_y;
	} else {
		max_x_tmp = md->si->sensing_conf_data.res_x;
		max_y_tmp = md->si->sensing_conf_data.res_y;
	}

	/* get maximum values from the sysinfo data */
	if (md->pdata->flags & CY_MT_FLAG_FLIP) {
		max_x = max_y_tmp - 1;
		max_y = max_x_tmp - 1;
	} else {
		max_x = max_x_tmp - 1;
		max_y = max_y_tmp - 1;
	}
	max_p = md->si->sensing_conf_data.max_z;

	/* set event signal capabilities */
	for (i = 0; i < (md->pdata->frmwrk->size / CY_NUM_ABS_SET); i++) {
		signal = ABS_PARAM(i, CY_SIGNAL_OST);
		if (signal != CY_IGNORE_VALUE) {
			set_bit(signal, md->input->absbit);
			min = ABS_PARAM(i, CY_MIN_OST);
			max = ABS_PARAM(i, CY_MAX_OST);
			if (i == CY_ABS_ID_OST) {
				/* shift track ids down to start at 0 */
				max = max - min;
				/*min = min - min;*/
				min = 0;
			} else if (i == CY_ABS_X_OST)
				max = max_x;
			else if (i == CY_ABS_Y_OST)
				max = max_y;
			else if (i == CY_ABS_P_OST)
				max = max_p;
			input_set_abs_params(md->input, signal, min, max,
				ABS_PARAM(i, CY_FUZZ_OST),
				ABS_PARAM(i, CY_FLAT_OST));
			tsp_debug_dbg(true, dev, "%s: register signal=%02X min=%d max=%d\n",
				__func__, signal, min, max);
		}
	}
#if defined(SAMSUNG_TOUCH_MODE)
	input_set_abs_params(md->input, ABS_MT_DISTANCE, 0, 255, 0, 0);
#endif

#if defined(SAMSUNG_PALM_MOTION)
	input_set_abs_params(md->input, signal = ABS_MT_PALM,
		min = 0, max = 1, 0, 0);
	tsp_debug_dbg(true, dev, "%s: register signal=%02X min=%d max=%d\n",
				__func__, signal, min, max);
	input_set_abs_params(md->input, signal = ABS_MT_SUMSIZE,
		min = 0, max = 255, 0, 0);
	tsp_debug_dbg(true, dev, "%s: register signal=%02X min=%d max=%d\n",
				__func__, signal, min, max);
#endif

	rc = cyttsp5_input_register_device(md->input,
			MAX_TOUCH_ID_NUMBER);
	if (rc < 0)
		tsp_debug_err(true, dev, "%s: Error, failed register input device r=%d\n",
			__func__, rc);
	else
		md->input_device_registered = true;

	return rc;
}

static int cyttsp5_setup_input_attention(struct device *dev)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;
	int rc;

	md->si = _cyttsp5_request_sysinfo(dev);
	if (!md->si)
		return -EINVAL;

	rc = cyttsp5_setup_input_device(dev);

	_cyttsp5_unsubscribe_attention(dev, CY_ATTEN_STARTUP, CY_MODULE_MT,
		cyttsp5_setup_input_attention, 0);

	return rc;
}

#ifdef CYTTSP5_DT2W
static void cyttsp5_factory_command(struct device *_dev, const char *command, int value)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(_dev);
	struct cyttsp5_samsung_factory_data *sfd = &cd->sfd;
	struct factory_cmd *factory_cmd_ptr = NULL;
	//char strbuff[FACTORY_CMD_STR_LEN] = "hover_enable";
	char strbuff[FACTORY_CMD_STR_LEN] = {0};
	bool cmd_found = false;
	printk(KERN_INFO "%s: DT2W Factory command to Samsung Factory: %s\n", __func__, command);
	strcpy(strbuff, command); // used internally, can be assumed command will fit
	/* find command */
	list_for_each_entry(factory_cmd_ptr,
			&sfd->factory_cmd_list_head, list) {
		if (!strcmp(strbuff, factory_cmd_ptr->cmd_name)) {
			cmd_found = true;
			break;
		}
	}
	if (cmd_found)
	{
		sfd->factory_cmd_param[0] = value;
		factory_cmd_ptr->cmd_func(sfd);
		printk(KERN_INFO "%s: DT2W Factory command to Samsung Factory Result: %s\n", __func__, sfd->factory_cmd_result);
	}
}

static enum hrtimer_restart cyttsp5_dt2w_hrtimer_callback( struct hrtimer *timer )
{
	struct cyttsp5_mt_data *md = container_of(timer, struct cyttsp5_mt_data, dt2w_timer);
	printk(KERN_INFO "%s: DT2W Timer finished, touch count reset\n", __func__);
	md->dt2w_touchCount = 0;
  	return HRTIMER_NORESTART;
}

static void cyttsp5_dt2w_timerStart(struct cyttsp5_mt_data *md)
{
	if (md->dt2w_timerFlag)
	{
		printk(KERN_INFO "%s: DT2W Timer started.\n", __func__);
		hrtimer_start( &md->dt2w_timer, md->dt2w_ktime, HRTIMER_MODE_REL );	
	}
}

static void cyttsp5_dt2w_timerCancel(struct cyttsp5_mt_data *md)
{
	if (md->dt2w_timerFlag)
	{
		printk(KERN_INFO "%s: DT2W Timer canceled.\n", __func__);
		hrtimer_cancel(&md->dt2w_timer);	
	}
}

static void cyttsp5_dt2w_timerInit(struct cyttsp5_mt_data *md)
{
	unsigned long delay_in_ms = 500L;
	printk(KERN_INFO "%s: Setting up DT2W timer\n", __func__);
	hrtimer_init( &md->dt2w_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
  	md->dt2w_ktime = ktime_set( 0, MS_TO_NS(delay_in_ms) );
  	md->dt2w_timer.function = &cyttsp5_dt2w_hrtimer_callback;
  	md->dt2w_timerFlag = 1;
}

void cyttsp5_dt2w_viewcoverNotify(struct device *_dev ,int value)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(_dev);
	struct cyttsp5_mt_data *md = &cd->md;

	printk(KERN_INFO "%s: DT2W: Notified of view cover status: %d \n", __func__, value);
	md->dt2w_cover = value;
	if (value && md->dt2w_active)
	{
		md->dt2w_active = 0;
		printk(KERN_INFO "%s: DT2W: Release wakelock", __func__);
		wake_unlock(&md->dt2w_wake_lock);
		printk(KERN_INFO "%s: DT2W: View cover closed while panel active, attempt to suspend driver now.\n", __func__);
		cyttsp5_mt_close(md->input);

	}
}
#endif

int cyttsp5_mt_probe(struct device *dev)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;
	struct cyttsp5_platform_data *pdata = dev_get_platdata(dev);
	struct cyttsp5_mt_platform_data *mt_pdata;
	int rc = 0;

	tsp_debug_dbg(false, dev, "%s:\n", __func__);

	if (!pdata || !pdata->mt_pdata) {
		tsp_debug_err(false, dev, "%s: Missing platform data\n", __func__);
		rc = -ENODEV;
		goto error_no_pdata;
	}
	mt_pdata = pdata->mt_pdata;

	mutex_init(&md->mt_lock);
	md->dev = dev;
	md->pdata = mt_pdata;
#if TOUCH_BOOSTER
	mutex_init(&md->dvfs_lock);
	md->touch_pressed_num = 0;
	md->dvfs_lock_status = false;
	md->boost_level = TSP_BOOSTER_LEVEL2;
	INIT_DELAYED_WORK(&md->work_dvfs_off, set_dvfs_off);
	INIT_DELAYED_WORK(&md->work_dvfs_chg, change_dvfs_lock);
#endif

	/* Create the input device and register it. */
	dev_vdbg(dev, "%s: Create the input device and register it\n",
		__func__);
	md->input = input_allocate_device();
	if (!md->input) {
		tsp_debug_err(false, dev,
			"%s: Error, failed to allocate input device\n", __func__);
		rc = -ENOSYS;
		goto error_alloc_failed;
	}

	if (md->pdata->inp_dev_name)
		md->input->name = md->pdata->inp_dev_name;
	else
		md->input->name = CYTTSP5_MT_NAME;
	scnprintf(md->phys, sizeof(md->phys)-1, "%s", CYTTSP5_MT_NAME);
	md->input->phys = md->phys;
	md->input->dev.parent = md->dev;
	md->input->open = cyttsp5_mt_open;
	md->input->close = cyttsp5_mt_close;
	input_set_drvdata(md->input, md);

	/* get sysinfo */
	md->si = _cyttsp5_request_sysinfo(dev);

	if (md->si) {
		rc = cyttsp5_setup_input_device(dev);
		if (rc)
			goto error_init_input;
	} else {
		tsp_debug_err(false, dev,
			"%s: Fail get sysinfo pointer from core p=%p\n",
			__func__, md->si);
		_cyttsp5_subscribe_attention(dev, CY_ATTEN_STARTUP,
			CY_MODULE_MT, cyttsp5_setup_input_attention, 0);
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	cyttsp5_setup_early_suspend(md);
#endif

	tsp_debug_dbg(false, dev, "%s:done\n", __func__);
#ifdef CYTTSP5_DT2W
	md->dt2w_active = 0;
	md->dt2w_keyflag = 0;
	md->dt2w_touchCount = 0;
	md->dt2w_timerFlag = 0;
	md->dt2w_x = 0;
	md->dt2w_y = 0;
	md->dt2w_cover = 0;
	cyttsp5_dt2w_timerInit(md);
	wake_lock_init(&md->dt2w_wake_lock, WAKE_LOCK_SUSPEND, "dt2w_wake_hold");
#endif
	return 0;

error_init_input:
	input_free_device(md->input);
error_alloc_failed:
error_no_pdata:
	tsp_debug_err(false, dev, "%s failed.\n", __func__);
	return rc;
}
EXPORT_SYMBOL(cyttsp5_mt_probe);

int cyttsp5_mt_release(struct device *dev)
{
	struct cyttsp5_core_data *cd = dev_get_drvdata(dev);
	struct cyttsp5_mt_data *md = &cd->md;

#ifdef CONFIG_HAS_EARLYSUSPEND
	/*
	 * This check is to prevent pm_runtime usage_count drop below zero
	 * because of removing the module while in suspended state
	 */
	/*if (md->is_suspended)
		pm_runtime_get_noresume(dev);*/

	unregister_early_suspend(&md->es);
#endif

	if (md->input_device_registered) {
		input_unregister_device(md->input);
	} else {
		input_free_device(md->input);
		_cyttsp5_unsubscribe_attention(dev, CY_ATTEN_STARTUP,
			CY_MODULE_MT, cyttsp5_setup_input_attention, 0);
	}

#ifdef CYTTSP5_DT2W
	cyttsp5_dt2w_timerCancel(md);
	wake_lock_destroy(&md->dt2w_wake_lock);
#endif

	return 0;
}
EXPORT_SYMBOL(cyttsp5_mt_release);
