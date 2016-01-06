/* linux/arch/arm/mach-exynos/include/mach/asv-exynos3470.h
*
* Copyright (c) 2012 Samsung Electronics Co., Ltd.
*              http://www.samsung.com/
*
* EXYNOS3470 - Adoptive Support Voltage Header file
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_EXYNOS3470_ASV_H
#define __ASM_ARCH_EXYNOS3470_ASV_H __FILE__

#define ARM_DVFS_LEVEL_NR		(15)
#define ARM_ASV_GRP_NR			(16)
#define ARM_MAX_VOLT			(1500000)

#define INT_DVFS_LEVEL_NR		(5)
#define INT_ASV_GRP_NR			(16)
#define INT_MAX_VOLT			(975000)

#define G3D_DVFS_LEVEL_NR		(10)
#define G3D_ASV_GRP_NR			(16)
#define G3D_MAX_VOLT			(1500000)

#define MIF_DVFS_LEVEL_NR		(5)
#define MIF_ASV_GRP_NR			(16)
#define MIF_MAX_VOLT			(1050000)

static unsigned int (*refer_table_get_asv)[ARM_ASV_GRP_NR];
/* carmen range */
static unsigned int refer_table_get_asv_rev[2][ARM_ASV_GRP_NR] = {
	/*0  1   2   3   4   5   6   7   8   9  10  11  12  13  14*/
	{ 0, 0,  2,  3,  4,  6,  8, 10, 13, 16, 20, 25, 33, 40, 999},
	{ 0, 0, 19, 24, 27, 30, 32, 34, 36, 38, 40, 42, 44, 48, 999},
};

/* carmen plus range : we select this table in asv framework */
static unsigned int refer_table_get_asv_rev2[2][ARM_ASV_GRP_NR] = {
	/*0  1   2   3   4   5   6   7   8   9  10  11  12  13  14*/
	{ 0, 2,  2,  2,  3,  4,  6,  8, 10, 13, 16, 20, 25, 33, 40},
	{ 0, 17, 19, 24, 27, 30, 32, 34, 36, 38, 40, 42, 44, 48, 999},
};

/* ABB setting value */

static unsigned int arm_asv_abb_info[ARM_DVFS_LEVEL_NR][ARM_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5        ASV6        ASV7        ASV8        ASV9       ASV10       ASV11       ASV12       ASV13       ASV14 */
	{ 1600000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 1500000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 1400000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L1 */
	{ 1300000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L2 */
	{ 1200000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L3 */
	{ 1100000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L4 */
	{ 1000000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L5 */
	{  900000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L6 */
	{  800000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L7 */
	{  700000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X120,   ABB_X120},		/* L8 */
	{  600000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L9 */
	{  500000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L10 */
	{  400000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L11 */
};

static unsigned int int_asv_abb_info[INT_DVFS_LEVEL_NR][INT_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5       ASV6        ASV7        ASV8      ASV9       ASV10       ASV11       ASV12       ASV13     ASV14 */
	{ 267000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L0 */
	{ 200000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110},	/* L1 */
	{ 160000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},	/* L2 */
	{ 133000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},	/* L3 */
	{ 100000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},	/* L4 */
};

static unsigned int mif_asv_abb_info[MIF_DVFS_LEVEL_NR][MIF_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5        ASV6        ASV7        ASV8        ASV9       ASV10       ASV11       ASV12       ASV13       ASV14 */
	{ 400000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L1 */
	{ 267000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L2 */
	{ 200000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L3 */
};

static unsigned g3d_asv_abb_info[G3D_DVFS_LEVEL_NR][G3D_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5        ASV6        ASV7        ASV8    ASV9       ASV10       ASV11     ASV12       ASV13       ASV14 */
	{ 640000,   ABB_X070,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},
	{ 533000,   ABB_X070,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 450000,   ABB_X070,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 440000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L1 */
	{ 340000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110, ABB_X110, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L2 */
	{ 300000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110, ABB_X110, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L2 */
	{ 266000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L3 */
	{ 225000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L3 */
	{ 160000,   ABB_X120, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L4 */
	{ 150000,   ABB_X120, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L4 */
};

/* ASV setting value */
static unsigned int arm_asv_volt_info[ARM_DVFS_LEVEL_NR][ARM_ASV_GRP_NR + 1] = {
	/*  FREQ     ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6     ASV7     ASV8     ASV9    ASV10    ASV11    ASV12    ASV13    ASV14 */
	{ 1600000, 1450000, 1450000, 1350000, 1350000, 1350000, 1350000, 1400000, 1400000, 1300000, 1300000, 1250000, 1250000, 1250000, 1200000, 1200000},	/* L0 */
	{ 1500000, 1300000, 1325000, 1325000, 1325000, 1312500, 1312500, 1300000, 1287500, 1275000, 1250000, 1225000, 1212500, 1200000, 1175000, 1175000},	/* L0 */
	{ 1400000, 1300000, 1300000, 1300000, 1287500, 1275000, 1250000, 1250000, 1212500, 1187500, 1162500, 1162500, 1137500, 1137500, 1112500, 1112500},	/* L1 */
	{ 1300000, 1300000, 1275000, 1250000, 1225000, 1200000, 1175000, 1150000, 1125000, 1125000, 1100000, 1100000, 1075000, 1075000, 1050000, 1050000},	/* L2 */
	{ 1200000, 1250000, 1225000, 1200000, 1175000, 1150000, 1125000, 1100000, 1075000, 1075000, 1050000, 1050000, 1025000, 1025000, 1000000, 1000000},	/* L3 */
	{ 1100000, 1150000, 1187500, 1162500, 1137500, 1112500, 1087500, 1050000, 1037500, 1037500, 1012500, 1012500,  987500,  987500,  962500,  962500},	/* L4 */
	{ 1000000, 1100000, 1150000, 1125000, 1100000, 1075000, 1050000, 1025000, 1000000, 1000000,  975000,  975000,  950000,  950000,  925000,  925000},	/* L5 */
	{  900000, 1050000, 1112500, 1087500, 1062500, 1037500, 1012500,  975000,  962500,  962500,  937500,  937500,  912500,  912500,  887500,  887500},	/* L6 */
	{  800000, 1000000, 1075000, 1050000, 1025000, 1000000,  975000,  950000,  925000,  925000,  900000,  900000,  875000,  875000,  850000,  850000},	/* L7 */
	{  700000,  950000, 1037500, 1012500,  987500,  962500,  937500,  900000,  887500,  887500,  862500,  862500,  850000,  850000,  850000,  850000},	/* L8 */
	{  600000,  900000, 1000000,  975000,  950000,  925000,  900000,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L9 */
	{  500000,  900000,  962500,  937500,  912500,  887500,  862500,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L10 */
	{  400000,  900000,  937500,  912500,  887500,  862500,  850000,  825000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L11 */
	{  300000,  900000,  937500,  912500,  887500,  862500,  850000,  800000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},
	{  200000,  900000,  937500,  912500,  887500,  862500,  850000,  775000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},
};

static unsigned int int_asv_volt_info[INT_DVFS_LEVEL_NR][INT_ASV_GRP_NR + 1] = {
	/*  FREQ    ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6     ASV7     ASV8    ASV9    ASV10   ASV11   ASV12   ASV13   ASV14 */
	{ 267000, 1225000, 1100000, 1075000, 1075000, 1050000, 1050000, 1025000, 1000000, 1000000, 975000, 975000, 950000, 950000, 925000, 925000},	/* L0 */
	{ 200000, 1225000, 1000000,  975000,  975000,  950000,  950000,  925000,  900000,  900000, 875000, 875000, 850000, 850000, 850000, 850000},	/* L1 */
	{ 160000, 1050000,  950000,  925000,  925000,  900000,  900000,  875000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000},	/* L2 */
	{ 133000,  937500,  925000,  900000,  900000,  875000,  875000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000},	/* L3 */
	{ 100000,  912500,  925000,  900000,  900000,  875000,  875000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000},	/* L4 */
};

static unsigned int mif_asv_volt_info[MIF_DVFS_LEVEL_NR][MIF_ASV_GRP_NR + 1] = {
	/*  FREQ     ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6     ASV7     ASV8     ASV9    ASV10    ASV11    ASV12    ASV13    ASV14 */
	{ 400000, 1075000, 1025000, 1000000, 1000000,  975000,  975000,  950000,  950000,  925000,  925000,  900000,  900000,  900000,  900000,  900000},	/* L1 */
	{ 267000,  962500,  925000,  900000,  900000,  875000,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L2 */
	{ 200000,  912500,  850000,  825000,  825000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000},	/* L3 */
};

static unsigned g3d_asv_volt_info[G3D_DVFS_LEVEL_NR][G3D_ASV_GRP_NR + 1] = {
	/*  FREQ    ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6    ASV7    ASV8    ASV9    ASV10   ASV11   ASV12   ASV13   ASV14 */
        { 640000, 1125000, 1125000, 1125000, 1025000, 1025000, 1000000, 1000000, 975000, 975000, 950000, 950000, 950000, 950000, 950000, 950000},               /* L0 */
	{ 533000, 1050000, 1050000, 1050000, 1025000, 1025000, 1000000, 1000000, 975000, 975000, 950000, 950000, 950000, 950000, 950000, 950000},		/* L0 */
	{ 450000, 1050000,  975000,  975000,  950000,  950000,  925000,  925000, 900000, 900000, 875000, 875000, 875000, 875000, 875000, 875000},		/* L0 with ISP */
	{ 440000, 1000000,  975000,  975000,  950000,  950000,  925000,  925000, 900000, 900000, 875000, 875000, 875000, 875000, 875000, 875000},		/* L1 */
	{ 340000,  950000,  925000,  925000,  900000,  900000,  875000,  875000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L2 */
	{ 300000,  950000,  925000,  925000,  900000,  900000,  875000,  875000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L1 with ISP */
	{ 266000,  925000,  875000,  875000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L3 */
	{ 225000,  925000,  875000,  875000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L2 with ISP */
	{ 160000,  925000,  850000,  850000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L4 */
	{ 150000,  925000,  850000,  850000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L3 with ISP */
};

/* This asv table is exynos3470 revision2 table */

static unsigned int arm_asv_abb_info_rev2[ARM_DVFS_LEVEL_NR][ARM_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5        ASV6        ASV7        ASV8        ASV9       ASV10       ASV11       ASV12       ASV13       ASV14 */
	{ 1600000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 1500000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 1400000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L1 */
	{ 1300000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L2 */
	{ 1200000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L3 */
	{ 1100000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L4 */
	{ 1000000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L5 */
	{  900000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L6 */
	{  800000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L7 */
	{  700000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X120,   ABB_X120},		/* L8 */
	{  600000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L9 */
	{  500000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L10 */
	{  400000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L11 */
};

static unsigned int int_asv_abb_info_rev2[INT_DVFS_LEVEL_NR][INT_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5       ASV6        ASV7        ASV8      ASV9       ASV10       ASV11       ASV12       ASV13     ASV14 */
	{ 267000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L0 */
	{ 200000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110},	/* L1 */
	{ 160000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},	/* L2 */
	{ 133000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},	/* L3 */
	{ 100000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110,   ABB_X110,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},	/* L4 */
};

static unsigned int mif_asv_abb_info_rev2[MIF_DVFS_LEVEL_NR][MIF_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5        ASV6        ASV7        ASV8        ASV9       ASV10       ASV11       ASV12       ASV13       ASV14 */
	{ 400000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L1 */
	{ 267000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L2 */
	{ 200000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},	/* L3 */
};

static unsigned g3d_asv_abb_info_rev2[G3D_DVFS_LEVEL_NR][G3D_ASV_GRP_NR + 1] = {
	/*  FREQ       ASV0       ASV1         ASV2        ASV3        ASV4        ASV5        ASV6        ASV7        ASV8    ASV9       ASV10       ASV11     ASV12       ASV13       ASV14 */
	{ 640000,   ABB_X070,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},
	{ 533000,   ABB_X070,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 450000,   ABB_X070,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L0 */
	{ 440000, ABB_BYPASS,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060,   ABB_X060, ABB_X060, ABB_X060, ABB_X060, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS},		/* L1 */
	{ 340000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110, ABB_X110, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L2 */
	{ 300000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_X110, ABB_X110, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L2 */
	{ 266000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L3 */
	{ 225000, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L3 */
	{ 160000,   ABB_X120, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L4 */
	{ 150000,   ABB_X120, ABB_BYPASS, ABB_BYPASS,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110,   ABB_X110, ABB_X120, ABB_X120, ABB_X120,   ABB_X120,   ABB_X120,   ABB_X120},		/* L4 */
};

/* ASV setting value */
static unsigned int arm_asv_volt_info_rev2[ARM_DVFS_LEVEL_NR][ARM_ASV_GRP_NR + 1] = {
	/*  FREQ     ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6     ASV7     ASV8     ASV9    ASV10    ASV11    ASV12    ASV13    ASV14 */
	{ 1600000, 1450000, 1450000, 1350000, 1350000, 1350000, 1350000, 1400000, 1300000, 1300000, 1300000, 1250000, 1250000, 1250000, 1200000, 1200000},
	{ 1500000, 1300000, 1325000, 1325000, 1325000, 1312500, 1312500, 1300000, 1287500, 1275000, 1250000, 1225000, 1212500, 1200000, 1175000, 1175000},	/* L0 */
	{ 1400000, 1300000, 1300000, 1300000, 1287500, 1275000, 1250000, 1200000, 1212500, 1187500, 1162500, 1162500, 1137500, 1137500, 1112500, 1112500},	/* L1 */
	{ 1300000, 1300000, 1275000, 1250000, 1225000, 1200000, 1175000, 1150000, 1125000, 1125000, 1100000, 1100000, 1075000, 1075000, 1050000, 1050000},	/* L2 */
	{ 1200000, 1250000, 1225000, 1200000, 1175000, 1150000, 1125000, 1100000, 1075000, 1075000, 1050000, 1050000, 1025000, 1025000, 1000000, 1000000},	/* L3 */
	{ 1100000, 1150000, 1187500, 1162500, 1137500, 1112500, 1087500, 1050000, 1037500, 1037500, 1012500, 1012500,  987500,  987500,  962500,  962500},	/* L4 */
	{ 1000000, 1100000, 1150000, 1125000, 1100000, 1075000, 1050000, 1025000, 1000000, 1000000,  975000,  975000,  950000,  950000,  925000,  925000},	/* L5 */
	{  900000, 1050000, 1112500, 1087500, 1062500, 1037500, 1012500,  975000,  962500,  962500,  937500,  937500,  912500,  912500,  887500,  887500},	/* L6 */
	{  800000, 1000000, 1075000, 1050000, 1025000, 1000000,  975000,  950000,  925000,  925000,  900000,  900000,  875000,  875000,  850000,  850000},	/* L7 */
	{  700000,  950000, 1037500, 1012500,  987500,  962500,  937500,  925000,  887500,  887500,  862500,  862500,  850000,  850000,  850000,  850000},	/* L8 */
	{  600000,  900000, 1000000,  975000,  950000,  925000,  900000,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L9 */
	{  500000,  900000,  962500,  937500,  912500,  887500,  862500,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L10 */
	{  400000,  900000,  937500,  912500,  887500,  862500,  850000,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L11 */
        {  300000,  900000,  937500,  912500,  887500,  862500,  850000,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},
        {  200000,  900000,  937500,  912500,  887500,  862500,  850000,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},
};

static unsigned int int_asv_volt_info_rev2[INT_DVFS_LEVEL_NR][INT_ASV_GRP_NR + 1] = {
	/*  FREQ    ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6     ASV7     ASV8    ASV9    ASV10   ASV11   ASV12   ASV13   ASV14 */
	{ 267000, 1225000, 1100000, 1075000, 1075000, 1050000, 1050000, 1025000, 1000000, 1000000, 975000, 975000, 950000, 950000, 925000, 925000},	/* L0 */
	{ 200000, 1225000, 1000000,  975000,  975000,  950000,  950000,  925000,  900000,  900000, 875000, 875000, 850000, 850000, 850000, 850000},	/* L1 */
	{ 160000, 1050000,  950000,  925000,  925000,  900000,  900000,  875000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000},	/* L2 */
	{ 133000,  937500,  925000,  900000,  900000,  875000,  875000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000},	/* L3 */
	{ 100000,  912500,  925000,  900000,  900000,  875000,  875000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000},	/* L4 */
};

static unsigned int mif_asv_volt_info_rev2[MIF_DVFS_LEVEL_NR][MIF_ASV_GRP_NR + 1] = {
	/*  FREQ     ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6     ASV7     ASV8     ASV9    ASV10    ASV11    ASV12    ASV13    ASV14 */
	{ 400000, 1075000, 1025000, 1000000, 1000000,  975000,  975000,  950000,  950000,  925000,  925000,  900000,  900000,  900000,  900000,  900000},	/* L1 */
	{ 267000,  962500,  925000,  900000,  900000,  875000,  875000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000,  850000},	/* L2 */
	{ 200000,  912500,  850000,  825000,  825000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000,  800000},	/* L3 */
};

static unsigned g3d_asv_volt_info_rev2[G3D_DVFS_LEVEL_NR][G3D_ASV_GRP_NR + 1] = {
	/*  FREQ    ASV0     ASV1     ASV2     ASV3     ASV4     ASV5     ASV6    ASV7    ASV8    ASV9    ASV10   ASV11   ASV12   ASV13   ASV14 */
	{ 640000, 1125000, 1125000, 1125000, 1025000, 1025000, 1000000, 1000000, 975000, 975000, 950000, 950000, 950000, 950000, 950000, 950000},
	{ 533000, 1050000, 1050000, 1050000, 1025000, 1025000, 1000000, 1000000, 975000, 975000, 950000, 950000, 950000, 950000, 950000, 950000},		/* L0 */
	{ 450000, 1050000,  975000,  975000,  950000,  950000,  925000,  925000, 900000, 900000, 875000, 875000, 875000, 875000, 875000, 875000},		/* L0 with ISP */
	{ 440000, 1000000,  975000,  975000,  950000,  950000,  925000,  925000, 900000, 900000, 875000, 875000, 875000, 875000, 875000, 875000},		/* L1 */
	{ 340000,  950000,  925000,  925000,  900000,  900000,  875000,  875000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L2 */
	{ 300000,  950000,  925000,  925000,  900000,  900000,  875000,  875000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L1 with ISP */
	{ 266000,  925000,  875000,  875000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L3 */
	{ 225000,  925000,  875000,  875000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L2 with ISP */
	{ 160000,  925000,  850000,  850000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L4 */
	{ 150000,  925000,  850000,  850000,  850000,  850000,  850000,  850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000, 850000},		/* L3 with ISP */
};
#endif /* EXYNOS3470_ASV_H */
