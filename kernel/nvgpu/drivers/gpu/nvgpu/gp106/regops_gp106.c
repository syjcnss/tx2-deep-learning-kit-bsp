/*
 * Tegra GP106 GPU Debugger Driver Register Ops
 *
 * Copyright (c) 2016, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/slab.h>
#include <linux/err.h>
#include <linux/bsearch.h>
#include <uapi/linux/nvgpu.h>

#include "gk20a/gk20a.h"
#include "gk20a/dbg_gpu_gk20a.h"
#include "gk20a/regops_gk20a.h"
#include "regops_gp106.h"

static const struct regop_offset_range gp106_global_whitelist_ranges[] = {
	{ 0x000004f0,   1},
	{ 0x00001a00,   3},
	{ 0x00002800, 128},
	{ 0x00009400,   1},
	{ 0x00009410,   1},
	{ 0x00009480,   1},
	{ 0x00020200,  24},
	{ 0x00021c04,   3},
	{ 0x00021c14,   3},
	{ 0x00021c24,  71},
	{ 0x00021d44,   1},
	{ 0x00021d4c,   1},
	{ 0x00021d54,   1},
	{ 0x00021d5c,   1},
	{ 0x00021d68,  19},
	{ 0x00021dbc,  16},
	{ 0x00022430,   7},
	{ 0x00022450,   1},
	{ 0x0002245c,   2},
	{ 0x00070000,   5},
	{ 0x000840a8,   1},
	{ 0x00084b5c,   1},
	{ 0x000870a8,   1},
	{ 0x000884e0,   1},
	{ 0x000884f4,   1},
	{ 0x0008e00c,   1},
	{ 0x00100c18,   3},
	{ 0x00100c84,   1},
	{ 0x0010a0a8,   1},
	{ 0x0010a4f0,   1},
	{ 0x0013c808,   2},
	{ 0x0013cc14,   1},
	{ 0x0013cc34,   1},
	{ 0x0013cc54,   1},
	{ 0x0013cc74,   1},
	{ 0x0013cc94,   1},
	{ 0x0013ccb4,   1},
	{ 0x0013ec18,   1},
	{ 0x00140028,   1},
	{ 0x00140280,   1},
	{ 0x001402a0,   1},
	{ 0x00140350,   1},
	{ 0x00140480,   1},
	{ 0x001404a0,   1},
	{ 0x00140550,   1},
	{ 0x00140680,   1},
	{ 0x001406a0,   1},
	{ 0x00140750,   1},
	{ 0x00142028,   1},
	{ 0x00142280,   1},
	{ 0x001422a0,   1},
	{ 0x00142350,   1},
	{ 0x00142480,   1},
	{ 0x001424a0,   1},
	{ 0x00142550,   1},
	{ 0x00142680,   1},
	{ 0x001426a0,   1},
	{ 0x00142750,   1},
	{ 0x00144028,   1},
	{ 0x00144280,   1},
	{ 0x00144350,   1},
	{ 0x00144480,   1},
	{ 0x00144550,   1},
	{ 0x00144680,   1},
	{ 0x00144750,   1},
	{ 0x00146028,   1},
	{ 0x00146280,   1},
	{ 0x00146350,   1},
	{ 0x00146480,   1},
	{ 0x00146550,   1},
	{ 0x00146680,   1},
	{ 0x00146750,   1},
	{ 0x00148028,   1},
	{ 0x00148280,   1},
	{ 0x00148350,   1},
	{ 0x00148480,   1},
	{ 0x00148550,   1},
	{ 0x00148680,   1},
	{ 0x00148750,   1},
	{ 0x0014a028,   1},
	{ 0x0014a280,   1},
	{ 0x0014a350,   1},
	{ 0x0014a480,   1},
	{ 0x0014a550,   1},
	{ 0x0014a680,   1},
	{ 0x0014a750,   1},
	{ 0x0014c028,   1},
	{ 0x0014c280,   1},
	{ 0x0014c350,   1},
	{ 0x0014c480,   1},
	{ 0x0014c550,   1},
	{ 0x0014c680,   1},
	{ 0x0014c750,   1},
	{ 0x0014e028,   1},
	{ 0x0014e280,   1},
	{ 0x0014e350,   1},
	{ 0x0014e480,   1},
	{ 0x0014e550,   1},
	{ 0x0014e680,   1},
	{ 0x0014e750,   1},
	{ 0x00150028,   1},
	{ 0x00150280,   1},
	{ 0x00150350,   1},
	{ 0x00150480,   1},
	{ 0x00150550,   1},
	{ 0x00150680,   1},
	{ 0x00150750,   1},
	{ 0x00152028,   1},
	{ 0x00152280,   1},
	{ 0x00152350,   1},
	{ 0x00152480,   1},
	{ 0x00152550,   1},
	{ 0x00152680,   1},
	{ 0x00152750,   1},
	{ 0x00154028,   1},
	{ 0x00154280,   1},
	{ 0x00154350,   1},
	{ 0x00154480,   1},
	{ 0x00154550,   1},
	{ 0x00154680,   1},
	{ 0x00154750,   1},
	{ 0x00156028,   1},
	{ 0x00156280,   1},
	{ 0x00156350,   1},
	{ 0x00156480,   1},
	{ 0x00156550,   1},
	{ 0x00156680,   1},
	{ 0x00156750,   1},
	{ 0x0017e028,   1},
	{ 0x0017e280,   1},
	{ 0x0017e294,   1},
	{ 0x0017e29c,   2},
	{ 0x0017e2ac,   1},
	{ 0x0017e350,   1},
	{ 0x0017e39c,   1},
	{ 0x0017e480,   1},
	{ 0x0017e4a0,   1},
	{ 0x0017e550,   1},
	{ 0x0017e680,   1},
	{ 0x0017e6a0,   1},
	{ 0x0017e750,   1},
	{ 0x00180040,  41},
	{ 0x001800ec,   1},
	{ 0x001800f8,   7},
	{ 0x00180240,  41},
	{ 0x001802ec,   1},
	{ 0x001802f8,   7},
	{ 0x00180440,  41},
	{ 0x001804ec,   1},
	{ 0x001804f8,   7},
	{ 0x00180640,  41},
	{ 0x001806ec,   1},
	{ 0x001806f8,   7},
	{ 0x00180840,  41},
	{ 0x001808ec,   1},
	{ 0x001808f8,   7},
	{ 0x00180a40,  41},
	{ 0x00180aec,   1},
	{ 0x00180af8,   7},
	{ 0x00180c40,  41},
	{ 0x00180cec,   1},
	{ 0x00180cf8,   7},
	{ 0x00180e40,  41},
	{ 0x00180eec,   1},
	{ 0x00180ef8,   7},
	{ 0x00181040,  41},
	{ 0x001810ec,   1},
	{ 0x001810f8,   7},
	{ 0x00181240,  41},
	{ 0x001812ec,   1},
	{ 0x001812f8,   7},
	{ 0x00181440,  41},
	{ 0x001814ec,   1},
	{ 0x001814f8,   7},
	{ 0x00181640,  41},
	{ 0x001816ec,   1},
	{ 0x001816f8,   7},
	{ 0x00181840,  41},
	{ 0x001818ec,   1},
	{ 0x001818f8,   7},
	{ 0x00181a40,  41},
	{ 0x00181aec,   1},
	{ 0x00181af8,   7},
	{ 0x00181c40,  41},
	{ 0x00181cec,   1},
	{ 0x00181cf8,   7},
	{ 0x00181e40,  41},
	{ 0x00181eec,   1},
	{ 0x00181ef8,   7},
	{ 0x00182040,  41},
	{ 0x001820ec,   1},
	{ 0x001820f8,   7},
	{ 0x00182240,  41},
	{ 0x001822ec,   1},
	{ 0x001822f8,   7},
	{ 0x00182440,  41},
	{ 0x001824ec,   1},
	{ 0x001824f8,   7},
	{ 0x00182640,  41},
	{ 0x001826ec,   1},
	{ 0x001826f8,   7},
	{ 0x00182840,  41},
	{ 0x001828ec,   1},
	{ 0x001828f8,   7},
	{ 0x00182a40,  41},
	{ 0x00182aec,   1},
	{ 0x00182af8,   7},
	{ 0x00182c40,  41},
	{ 0x00182cec,   1},
	{ 0x00182cf8,   7},
	{ 0x00182e40,  41},
	{ 0x00182eec,   1},
	{ 0x00182ef8,   7},
	{ 0x00183040,  41},
	{ 0x001830ec,   1},
	{ 0x001830f8,   7},
	{ 0x00183240,  41},
	{ 0x001832ec,   1},
	{ 0x001832f8,   7},
	{ 0x00183440,  41},
	{ 0x001834ec,   1},
	{ 0x001834f8,   7},
	{ 0x00183640,  41},
	{ 0x001836ec,   1},
	{ 0x001836f8,   7},
	{ 0x00183840,  41},
	{ 0x001838ec,   1},
	{ 0x001838f8,   7},
	{ 0x00183a40,  41},
	{ 0x00183aec,   1},
	{ 0x00183af8,   7},
	{ 0x00183c40,  41},
	{ 0x00183cec,   1},
	{ 0x00183cf8,   7},
	{ 0x00183e40,  41},
	{ 0x00183eec,   1},
	{ 0x00183ef8,   7},
	{ 0x00184040,  41},
	{ 0x001840ec,   1},
	{ 0x001840f8,   7},
	{ 0x00184240,  41},
	{ 0x001842ec,   1},
	{ 0x001842f8,   7},
	{ 0x00184440,  41},
	{ 0x001844ec,   1},
	{ 0x001844f8,   7},
	{ 0x00184640,  41},
	{ 0x001846ec,   1},
	{ 0x001846f8,   7},
	{ 0x00184840,  41},
	{ 0x001848ec,   1},
	{ 0x001848f8,   7},
	{ 0x00184a40,  41},
	{ 0x00184aec,   1},
	{ 0x00184af8,   7},
	{ 0x00184c40,  41},
	{ 0x00184cec,   1},
	{ 0x00184cf8,   7},
	{ 0x00184e40,  41},
	{ 0x00184eec,   1},
	{ 0x00184ef8,   7},
	{ 0x00185040,  41},
	{ 0x001850ec,   1},
	{ 0x001850f8,   7},
	{ 0x00185240,  41},
	{ 0x001852ec,   1},
	{ 0x001852f8,   7},
	{ 0x00185440,  41},
	{ 0x001854ec,   1},
	{ 0x001854f8,   7},
	{ 0x00185640,  41},
	{ 0x001856ec,   1},
	{ 0x001856f8,   7},
	{ 0x00185840,  41},
	{ 0x001858ec,   1},
	{ 0x001858f8,   7},
	{ 0x00185a40,  41},
	{ 0x00185aec,   1},
	{ 0x00185af8,   7},
	{ 0x00185c40,  41},
	{ 0x00185cec,   1},
	{ 0x00185cf8,   7},
	{ 0x00185e40,  41},
	{ 0x00185eec,   1},
	{ 0x00185ef8,   7},
	{ 0x001a0040,  41},
	{ 0x001a00ec,   1},
	{ 0x001a00f8,   7},
	{ 0x001a0240,  41},
	{ 0x001a02ec,   1},
	{ 0x001a02f8,   7},
	{ 0x001a0440,  41},
	{ 0x001a04ec,   1},
	{ 0x001a04f8,   7},
	{ 0x001a0640,  41},
	{ 0x001a06ec,   1},
	{ 0x001a06f8,   7},
	{ 0x001a0840,  41},
	{ 0x001a08ec,   1},
	{ 0x001a08f8,   7},
	{ 0x001a0a40,  41},
	{ 0x001a0aec,   1},
	{ 0x001a0af8,   7},
	{ 0x001a0c40,  41},
	{ 0x001a0cec,   1},
	{ 0x001a0cf8,   7},
	{ 0x001a0e40,  41},
	{ 0x001a0eec,   1},
	{ 0x001a0ef8,   7},
	{ 0x001a1040,  41},
	{ 0x001a10ec,   1},
	{ 0x001a10f8,   7},
	{ 0x001a1240,  41},
	{ 0x001a12ec,   1},
	{ 0x001a12f8,   7},
	{ 0x001a1440,  41},
	{ 0x001a14ec,   1},
	{ 0x001a14f8,   7},
	{ 0x001a1640,  41},
	{ 0x001a16ec,   1},
	{ 0x001a16f8,   7},
	{ 0x001a1840,  41},
	{ 0x001a18ec,   1},
	{ 0x001a18f8,   7},
	{ 0x001a1a40,  41},
	{ 0x001a1aec,   1},
	{ 0x001a1af8,   7},
	{ 0x001a1c40,  41},
	{ 0x001a1cec,   1},
	{ 0x001a1cf8,   7},
	{ 0x001a1e40,  41},
	{ 0x001a1eec,   1},
	{ 0x001a1ef8,   7},
	{ 0x001a2040,  41},
	{ 0x001a20ec,   1},
	{ 0x001a20f8,   7},
	{ 0x001a2240,  41},
	{ 0x001a22ec,   1},
	{ 0x001a22f8,   7},
	{ 0x001a2440,  41},
	{ 0x001a24ec,   1},
	{ 0x001a24f8,   7},
	{ 0x001a2640,  41},
	{ 0x001a26ec,   1},
	{ 0x001a26f8,   7},
	{ 0x001a2840,  41},
	{ 0x001a28ec,   1},
	{ 0x001a28f8,   7},
	{ 0x001a2a40,  41},
	{ 0x001a2aec,   1},
	{ 0x001a2af8,   7},
	{ 0x001a2c40,  41},
	{ 0x001a2cec,   1},
	{ 0x001a2cf8,   7},
	{ 0x001a2e40,  41},
	{ 0x001a2eec,   1},
	{ 0x001a2ef8,   7},
	{ 0x001a3040,  41},
	{ 0x001a30ec,   1},
	{ 0x001a30f8,   7},
	{ 0x001a3240,  41},
	{ 0x001a32ec,   1},
	{ 0x001a32f8,   7},
	{ 0x001a3440,  41},
	{ 0x001a34ec,   1},
	{ 0x001a34f8,   7},
	{ 0x001a3640,  41},
	{ 0x001a36ec,   1},
	{ 0x001a36f8,   7},
	{ 0x001a3840,  41},
	{ 0x001a38ec,   1},
	{ 0x001a38f8,   7},
	{ 0x001a3a40,  41},
	{ 0x001a3aec,   1},
	{ 0x001a3af8,   7},
	{ 0x001a3c40,  41},
	{ 0x001a3cec,   1},
	{ 0x001a3cf8,   7},
	{ 0x001a3e40,  41},
	{ 0x001a3eec,   1},
	{ 0x001a3ef8,   7},
	{ 0x001a4040,  41},
	{ 0x001a40ec,   1},
	{ 0x001a40f8,   7},
	{ 0x001a4240,  41},
	{ 0x001a42ec,   1},
	{ 0x001a42f8,   7},
	{ 0x001a4440,  41},
	{ 0x001a44ec,   1},
	{ 0x001a44f8,   7},
	{ 0x001a4640,  41},
	{ 0x001a46ec,   1},
	{ 0x001a46f8,   7},
	{ 0x001a4840,  41},
	{ 0x001a48ec,   1},
	{ 0x001a48f8,   7},
	{ 0x001a4a40,  41},
	{ 0x001a4aec,   1},
	{ 0x001a4af8,   7},
	{ 0x001a4c40,  41},
	{ 0x001a4cec,   1},
	{ 0x001a4cf8,   7},
	{ 0x001a4e40,  41},
	{ 0x001a4eec,   1},
	{ 0x001a4ef8,   7},
	{ 0x001a5040,  41},
	{ 0x001a50ec,   1},
	{ 0x001a50f8,   7},
	{ 0x001a5240,  41},
	{ 0x001a52ec,   1},
	{ 0x001a52f8,   7},
	{ 0x001a5440,  41},
	{ 0x001a54ec,   1},
	{ 0x001a54f8,   7},
	{ 0x001a5640,  41},
	{ 0x001a56ec,   1},
	{ 0x001a56f8,   7},
	{ 0x001a5840,  41},
	{ 0x001a58ec,   1},
	{ 0x001a58f8,   7},
	{ 0x001a5a40,  41},
	{ 0x001a5aec,   1},
	{ 0x001a5af8,   7},
	{ 0x001a5c40,  41},
	{ 0x001a5cec,   1},
	{ 0x001a5cf8,   7},
	{ 0x001a5e40,  41},
	{ 0x001a5eec,   1},
	{ 0x001a5ef8,   7},
	{ 0x001b0040,  41},
	{ 0x001b00ec,   1},
	{ 0x001b00f8,   7},
	{ 0x001b0240,  41},
	{ 0x001b02ec,   1},
	{ 0x001b02f8,   7},
	{ 0x001b0440,  41},
	{ 0x001b04ec,   1},
	{ 0x001b04f8,   7},
	{ 0x001b0640,  41},
	{ 0x001b06ec,   1},
	{ 0x001b06f8,   7},
	{ 0x001b0840,  41},
	{ 0x001b08ec,   1},
	{ 0x001b08f8,   7},
	{ 0x001b0a40,  41},
	{ 0x001b0aec,   1},
	{ 0x001b0af8,   7},
	{ 0x001b0c40,  41},
	{ 0x001b0cec,   1},
	{ 0x001b0cf8,   7},
	{ 0x001b0e40,  41},
	{ 0x001b0eec,   1},
	{ 0x001b0ef8,   7},
	{ 0x001b4000,   1},
	{ 0x001b4008,   1},
	{ 0x001b4010,   3},
	{ 0x001b4020,   3},
	{ 0x001b4030,   3},
	{ 0x001b4040,   3},
	{ 0x001b4050,   3},
	{ 0x001b4060,   4},
	{ 0x001b4074,   7},
	{ 0x001b4094,   3},
	{ 0x001b40a4,   1},
	{ 0x001b4100,   6},
	{ 0x001b4124,   2},
	{ 0x001b8000,   1},
	{ 0x001b8008,   1},
	{ 0x001b8010,   3},
	{ 0x001b8200,   1},
	{ 0x001b8208,   1},
	{ 0x001b8210,   3},
	{ 0x001b8400,   1},
	{ 0x001b8408,   1},
	{ 0x001b8410,   3},
	{ 0x001b8600,   1},
	{ 0x001b8608,   1},
	{ 0x001b8610,   3},
	{ 0x001b8800,   1},
	{ 0x001b8808,   1},
	{ 0x001b8810,   3},
	{ 0x001b8a00,   1},
	{ 0x001b8a08,   1},
	{ 0x001b8a10,   3},
	{ 0x001bc000,   1},
	{ 0x001bc008,   1},
	{ 0x001bc010,   3},
	{ 0x001bc200,   1},
	{ 0x001bc208,   1},
	{ 0x001bc210,   3},
	{ 0x001bc400,   1},
	{ 0x001bc408,   1},
	{ 0x001bc410,   3},
	{ 0x001bc600,   1},
	{ 0x001bc608,   1},
	{ 0x001bc610,   3},
	{ 0x001bc800,   1},
	{ 0x001bc808,   1},
	{ 0x001bc810,   3},
	{ 0x001bca00,   1},
	{ 0x001bca08,   1},
	{ 0x001bca10,   3},
	{ 0x001bd218,   1},
	{ 0x001be000,   1},
	{ 0x001be008,   1},
	{ 0x001be010,   3},
	{ 0x001be218,   1},
	{ 0x001bf218,   1},
	{ 0x001c0218,   1},
	{ 0x001c1218,   1},
	{ 0x001c80a8,   1},
	{ 0x001c9100,   1},
	{ 0x001cc0a8,   1},
	{ 0x001cd100,   1},
	{ 0x00400500,   1},
	{ 0x0040415c,   1},
	{ 0x00404468,   1},
	{ 0x00404498,   1},
	{ 0x00405800,   1},
	{ 0x00405840,   2},
	{ 0x00405850,   1},
	{ 0x00405908,   1},
	{ 0x00405a00,   1},
	{ 0x00405b40,   1},
	{ 0x00405b50,   1},
	{ 0x00406024,   5},
	{ 0x00407010,   1},
	{ 0x00407808,   1},
	{ 0x0040803c,   1},
	{ 0x00408804,   1},
	{ 0x0040880c,   1},
	{ 0x00408900,   2},
	{ 0x00408910,   1},
	{ 0x00408944,   1},
	{ 0x00408984,   1},
	{ 0x004090a8,   1},
	{ 0x004098a0,   1},
	{ 0x00409b00,   1},
	{ 0x0041000c,   1},
	{ 0x00410110,   1},
	{ 0x00410184,   1},
	{ 0x0041040c,   1},
	{ 0x00410510,   1},
	{ 0x00410584,   1},
	{ 0x0041080c,   1},
	{ 0x00410910,   1},
	{ 0x00410984,   1},
	{ 0x00410c0c,   1},
	{ 0x00410d10,   1},
	{ 0x00410d84,   1},
	{ 0x0041100c,   1},
	{ 0x00411110,   1},
	{ 0x00411184,   1},
	{ 0x0041140c,   1},
	{ 0x00411510,   1},
	{ 0x00411584,   1},
	{ 0x0041180c,   1},
	{ 0x00411910,   1},
	{ 0x00411984,   1},
	{ 0x00411c0c,   1},
	{ 0x00411d10,   1},
	{ 0x00411d84,   1},
	{ 0x0041200c,   1},
	{ 0x00412110,   1},
	{ 0x00412184,   1},
	{ 0x0041240c,   1},
	{ 0x00412510,   1},
	{ 0x00412584,   1},
	{ 0x0041280c,   1},
	{ 0x00412910,   1},
	{ 0x00412984,   1},
	{ 0x00412c0c,   1},
	{ 0x00412d10,   1},
	{ 0x00412d84,   1},
	{ 0x00418000,   1},
	{ 0x00418008,   1},
	{ 0x00418380,   2},
	{ 0x00418400,   2},
	{ 0x004184a0,   1},
	{ 0x00418604,   1},
	{ 0x00418680,   1},
	{ 0x00418704,   1},
	{ 0x00418714,   1},
	{ 0x00418800,   1},
	{ 0x0041881c,   1},
	{ 0x00418830,   1},
	{ 0x00418884,   1},
	{ 0x004188b0,   1},
	{ 0x004188c8,   3},
	{ 0x004188fc,   1},
	{ 0x00418b04,   1},
	{ 0x00418c04,   1},
	{ 0x00418c10,   8},
	{ 0x00418c88,   1},
	{ 0x00418d00,   1},
	{ 0x00418e00,   1},
	{ 0x00418e08,   1},
	{ 0x00418e34,   1},
	{ 0x00418e40,   4},
	{ 0x00418e58,  16},
	{ 0x00418f08,   1},
	{ 0x00419000,   1},
	{ 0x0041900c,   1},
	{ 0x00419018,   1},
	{ 0x00419854,   1},
	{ 0x00419864,   1},
	{ 0x00419a04,   2},
	{ 0x00419a14,   1},
	{ 0x00419ab0,   1},
	{ 0x00419ab8,   3},
	{ 0x00419c0c,   1},
	{ 0x00419c8c,   2},
	{ 0x00419d00,   1},
	{ 0x00419d08,   2},
	{ 0x00419e00,  11},
	{ 0x00419e34,   2},
	{ 0x00419e44,  11},
	{ 0x00419e74,  10},
	{ 0x00419ea4,   1},
	{ 0x00419eac,   2},
	{ 0x00419ee8,   1},
	{ 0x00419ef0,  28},
	{ 0x00419f70,   1},
	{ 0x00419f78,   2},
	{ 0x00419f98,   2},
	{ 0x00419fdc,   1},
	{ 0x0041a02c,   2},
	{ 0x0041a0a0,   1},
	{ 0x0041a0a8,   1},
	{ 0x0041a890,   2},
	{ 0x0041a8a0,   3},
	{ 0x0041a8b0,   2},
	{ 0x0041b014,   1},
	{ 0x0041b0cc,   1},
	{ 0x0041b1dc,   1},
	{ 0x0041b214,   1},
	{ 0x0041b2cc,   1},
	{ 0x0041b3dc,   1},
	{ 0x0041b414,   1},
	{ 0x0041b4cc,   1},
	{ 0x0041b5dc,   1},
	{ 0x0041be0c,   3},
	{ 0x0041becc,   1},
	{ 0x0041bfdc,   1},
	{ 0x0041c054,   1},
	{ 0x0041c2b0,   1},
	{ 0x0041c2b8,   3},
	{ 0x0041c40c,   1},
	{ 0x0041c48c,   2},
	{ 0x0041c500,   1},
	{ 0x0041c508,   2},
	{ 0x0041c600,  11},
	{ 0x0041c634,   2},
	{ 0x0041c644,  11},
	{ 0x0041c674,  10},
	{ 0x0041c6a4,   1},
	{ 0x0041c6ac,   2},
	{ 0x0041c6e8,   1},
	{ 0x0041c6f0,  28},
	{ 0x0041c770,   1},
	{ 0x0041c778,   2},
	{ 0x0041c798,   2},
	{ 0x0041c7dc,   1},
	{ 0x0041c854,   1},
	{ 0x0041cab0,   1},
	{ 0x0041cab8,   3},
	{ 0x0041cc0c,   1},
	{ 0x0041cc8c,   2},
	{ 0x0041cd00,   1},
	{ 0x0041cd08,   2},
	{ 0x0041ce00,  11},
	{ 0x0041ce34,   2},
	{ 0x0041ce44,  11},
	{ 0x0041ce74,  10},
	{ 0x0041cea4,   1},
	{ 0x0041ceac,   2},
	{ 0x0041cee8,   1},
	{ 0x0041cef0,  28},
	{ 0x0041cf70,   1},
	{ 0x0041cf78,   2},
	{ 0x0041cf98,   2},
	{ 0x0041cfdc,   1},
	{ 0x0041d054,   1},
	{ 0x0041d2b0,   1},
	{ 0x0041d2b8,   3},
	{ 0x0041d40c,   1},
	{ 0x0041d48c,   2},
	{ 0x0041d500,   1},
	{ 0x0041d508,   2},
	{ 0x0041d600,  11},
	{ 0x0041d634,   2},
	{ 0x0041d644,  11},
	{ 0x0041d674,  10},
	{ 0x0041d6a4,   1},
	{ 0x0041d6ac,   2},
	{ 0x0041d6e8,   1},
	{ 0x0041d6f0,  28},
	{ 0x0041d770,   1},
	{ 0x0041d778,   2},
	{ 0x0041d798,   2},
	{ 0x0041d7dc,   1},
	{ 0x0041d854,   1},
	{ 0x0041dab0,   1},
	{ 0x0041dab8,   3},
	{ 0x0041dc0c,   1},
	{ 0x0041dc8c,   2},
	{ 0x0041dd00,   1},
	{ 0x0041dd08,   2},
	{ 0x0041de00,  11},
	{ 0x0041de34,   2},
	{ 0x0041de44,  11},
	{ 0x0041de74,  10},
	{ 0x0041dea4,   1},
	{ 0x0041deac,   2},
	{ 0x0041dee8,   1},
	{ 0x0041def0,  28},
	{ 0x0041df70,   1},
	{ 0x0041df78,   2},
	{ 0x0041df98,   2},
	{ 0x0041dfdc,   1},
	{ 0x0041e054,   1},
	{ 0x0041e2b0,   1},
	{ 0x0041e2b8,   3},
	{ 0x0041e40c,   1},
	{ 0x0041e48c,   2},
	{ 0x0041e500,   1},
	{ 0x0041e508,   2},
	{ 0x0041e600,  11},
	{ 0x0041e634,   2},
	{ 0x0041e644,  11},
	{ 0x0041e674,  10},
	{ 0x0041e6a4,   1},
	{ 0x0041e6ac,   2},
	{ 0x0041e6e8,   1},
	{ 0x0041e6f0,  28},
	{ 0x0041e770,   1},
	{ 0x0041e778,   2},
	{ 0x0041e798,   2},
	{ 0x0041e7dc,   1},
	{ 0x00500384,   1},
	{ 0x005004a0,   1},
	{ 0x00500604,   1},
	{ 0x00500680,   1},
	{ 0x00500714,   1},
	{ 0x0050081c,   1},
	{ 0x00500884,   1},
	{ 0x005008b0,   1},
	{ 0x005008c8,   3},
	{ 0x005008fc,   1},
	{ 0x00500b04,   1},
	{ 0x00500c04,   1},
	{ 0x00500c10,   8},
	{ 0x00500c88,   1},
	{ 0x00500d00,   1},
	{ 0x00500e08,   1},
	{ 0x00500f08,   1},
	{ 0x00501000,   1},
	{ 0x0050100c,   1},
	{ 0x00501018,   1},
	{ 0x00501854,   1},
	{ 0x00501ab0,   1},
	{ 0x00501ab8,   3},
	{ 0x00501c0c,   1},
	{ 0x00501c8c,   2},
	{ 0x00501d00,   1},
	{ 0x00501d08,   2},
	{ 0x00501e00,  11},
	{ 0x00501e34,   2},
	{ 0x00501e44,  11},
	{ 0x00501e74,  10},
	{ 0x00501ea4,   1},
	{ 0x00501eac,   2},
	{ 0x00501ee8,   1},
	{ 0x00501ef0,  28},
	{ 0x00501f70,   1},
	{ 0x00501f78,   2},
	{ 0x00501f98,   2},
	{ 0x00501fdc,   1},
	{ 0x0050202c,   2},
	{ 0x005020a0,   1},
	{ 0x005020a8,   1},
	{ 0x00502890,   2},
	{ 0x005028a0,   3},
	{ 0x005028b0,   2},
	{ 0x00503014,   1},
	{ 0x005030cc,   1},
	{ 0x005031dc,   1},
	{ 0x00503214,   1},
	{ 0x005032cc,   1},
	{ 0x005033dc,   1},
	{ 0x00503414,   1},
	{ 0x005034cc,   1},
	{ 0x005035dc,   1},
	{ 0x00503e14,   1},
	{ 0x00503ecc,   1},
	{ 0x00503fdc,   1},
	{ 0x00504054,   1},
	{ 0x005042b0,   1},
	{ 0x005042b8,   3},
	{ 0x0050440c,   1},
	{ 0x0050448c,   2},
	{ 0x00504500,   1},
	{ 0x00504508,   2},
	{ 0x00504600,  11},
	{ 0x00504634,   2},
	{ 0x00504644,  11},
	{ 0x00504674,  10},
	{ 0x005046a4,   1},
	{ 0x005046ac,   2},
	{ 0x005046e8,   1},
	{ 0x005046f0,  28},
	{ 0x00504770,   1},
	{ 0x00504778,   2},
	{ 0x00504798,   2},
	{ 0x005047dc,   1},
	{ 0x00504854,   1},
	{ 0x00504ab0,   1},
	{ 0x00504ab8,   3},
	{ 0x00504c0c,   1},
	{ 0x00504c8c,   2},
	{ 0x00504d00,   1},
	{ 0x00504d08,   2},
	{ 0x00504e00,  11},
	{ 0x00504e34,   2},
	{ 0x00504e44,  11},
	{ 0x00504e74,  10},
	{ 0x00504ea4,   1},
	{ 0x00504eac,   2},
	{ 0x00504ee8,   1},
	{ 0x00504ef0,  28},
	{ 0x00504f70,   1},
	{ 0x00504f78,   2},
	{ 0x00504f98,   2},
	{ 0x00504fdc,   1},
	{ 0x00505054,   1},
	{ 0x005052b0,   1},
	{ 0x005052b8,   3},
	{ 0x0050540c,   1},
	{ 0x0050548c,   2},
	{ 0x00505500,   1},
	{ 0x00505508,   2},
	{ 0x00505600,  11},
	{ 0x00505634,   2},
	{ 0x00505644,  11},
	{ 0x00505674,  10},
	{ 0x005056a4,   1},
	{ 0x005056ac,   2},
	{ 0x005056e8,   1},
	{ 0x005056f0,  28},
	{ 0x00505770,   1},
	{ 0x00505778,   2},
	{ 0x00505798,   2},
	{ 0x005057dc,   1},
	{ 0x00505854,   1},
	{ 0x00505ab0,   1},
	{ 0x00505ab8,   3},
	{ 0x00505c0c,   1},
	{ 0x00505c8c,   2},
	{ 0x00505d00,   1},
	{ 0x00505d08,   2},
	{ 0x00505e00,  11},
	{ 0x00505e34,   2},
	{ 0x00505e44,  11},
	{ 0x00505e74,  10},
	{ 0x00505ea4,   1},
	{ 0x00505eac,   2},
	{ 0x00505ee8,   1},
	{ 0x00505ef0,  28},
	{ 0x00505f70,   1},
	{ 0x00505f78,   2},
	{ 0x00505f98,   2},
	{ 0x00505fdc,   1},
	{ 0x00506054,   1},
	{ 0x005062b0,   1},
	{ 0x005062b8,   3},
	{ 0x0050640c,   1},
	{ 0x0050648c,   2},
	{ 0x00506500,   1},
	{ 0x00506508,   2},
	{ 0x00506600,  11},
	{ 0x00506634,   2},
	{ 0x00506644,  11},
	{ 0x00506674,  10},
	{ 0x005066a4,   1},
	{ 0x005066ac,   2},
	{ 0x005066e8,   1},
	{ 0x005066f0,  28},
	{ 0x00506770,   1},
	{ 0x00506778,   2},
	{ 0x00506798,   2},
	{ 0x005067dc,   1},
	{ 0x00508384,   1},
	{ 0x005084a0,   1},
	{ 0x00508604,   1},
	{ 0x00508680,   1},
	{ 0x00508714,   1},
	{ 0x0050881c,   1},
	{ 0x00508884,   1},
	{ 0x005088b0,   1},
	{ 0x005088c8,   2},
	{ 0x00508b04,   1},
	{ 0x00508c04,   1},
	{ 0x00508c10,   8},
	{ 0x00508c88,   1},
	{ 0x00508d00,   1},
	{ 0x00508e08,   1},
	{ 0x00508f08,   1},
	{ 0x00509000,   1},
	{ 0x0050900c,   1},
	{ 0x00509018,   1},
	{ 0x00509854,   1},
	{ 0x00509ab0,   1},
	{ 0x00509ab8,   3},
	{ 0x00509c0c,   1},
	{ 0x00509c8c,   2},
	{ 0x00509d00,   1},
	{ 0x00509d08,   2},
	{ 0x00509e00,  11},
	{ 0x00509e34,   2},
	{ 0x00509e44,  11},
	{ 0x00509e74,  10},
	{ 0x00509ea4,   1},
	{ 0x00509eac,   2},
	{ 0x00509ee8,   1},
	{ 0x00509ef0,  28},
	{ 0x00509f70,   1},
	{ 0x00509f78,   2},
	{ 0x00509f98,   2},
	{ 0x00509fdc,   1},
	{ 0x0050a02c,   2},
	{ 0x0050a0a0,   1},
	{ 0x0050a0a8,   1},
	{ 0x0050a890,   2},
	{ 0x0050a8a0,   3},
	{ 0x0050a8b0,   2},
	{ 0x0050b014,   1},
	{ 0x0050b0cc,   1},
	{ 0x0050b1dc,   1},
	{ 0x0050b214,   1},
	{ 0x0050b2cc,   1},
	{ 0x0050b3dc,   1},
	{ 0x0050b414,   1},
	{ 0x0050b4cc,   1},
	{ 0x0050b5dc,   1},
	{ 0x0050be14,   1},
	{ 0x0050becc,   1},
	{ 0x0050bfdc,   1},
	{ 0x0050c054,   1},
	{ 0x0050c2b0,   1},
	{ 0x0050c2b8,   3},
	{ 0x0050c40c,   1},
	{ 0x0050c48c,   2},
	{ 0x0050c500,   1},
	{ 0x0050c508,   2},
	{ 0x0050c600,  11},
	{ 0x0050c634,   2},
	{ 0x0050c644,  11},
	{ 0x0050c674,  10},
	{ 0x0050c6a4,   1},
	{ 0x0050c6ac,   2},
	{ 0x0050c6e8,   1},
	{ 0x0050c6f0,  28},
	{ 0x0050c770,   1},
	{ 0x0050c778,   2},
	{ 0x0050c798,   2},
	{ 0x0050c7dc,   1},
	{ 0x0050c854,   1},
	{ 0x0050cab0,   1},
	{ 0x0050cab8,   3},
	{ 0x0050cc0c,   1},
	{ 0x0050cc8c,   2},
	{ 0x0050cd00,   1},
	{ 0x0050cd08,   2},
	{ 0x0050ce00,  11},
	{ 0x0050ce34,   2},
	{ 0x0050ce44,  11},
	{ 0x0050ce74,  10},
	{ 0x0050cea4,   1},
	{ 0x0050ceac,   2},
	{ 0x0050cee8,   1},
	{ 0x0050cef0,  28},
	{ 0x0050cf70,   1},
	{ 0x0050cf78,   2},
	{ 0x0050cf98,   2},
	{ 0x0050cfdc,   1},
	{ 0x0050d054,   1},
	{ 0x0050d2b0,   1},
	{ 0x0050d2b8,   3},
	{ 0x0050d40c,   1},
	{ 0x0050d48c,   2},
	{ 0x0050d500,   1},
	{ 0x0050d508,   2},
	{ 0x0050d600,  11},
	{ 0x0050d634,   2},
	{ 0x0050d644,  11},
	{ 0x0050d674,  10},
	{ 0x0050d6a4,   1},
	{ 0x0050d6ac,   2},
	{ 0x0050d6e8,   1},
	{ 0x0050d6f0,  28},
	{ 0x0050d770,   1},
	{ 0x0050d778,   2},
	{ 0x0050d798,   2},
	{ 0x0050d7dc,   1},
	{ 0x0050d854,   1},
	{ 0x0050dab0,   1},
	{ 0x0050dab8,   3},
	{ 0x0050dc0c,   1},
	{ 0x0050dc8c,   2},
	{ 0x0050dd00,   1},
	{ 0x0050dd08,   2},
	{ 0x0050de00,  11},
	{ 0x0050de34,   2},
	{ 0x0050de44,  11},
	{ 0x0050de74,  10},
	{ 0x0050dea4,   1},
	{ 0x0050deac,   2},
	{ 0x0050dee8,   1},
	{ 0x0050def0,  28},
	{ 0x0050df70,   1},
	{ 0x0050df78,   2},
	{ 0x0050df98,   2},
	{ 0x0050dfdc,   1},
	{ 0x0050e054,   1},
	{ 0x0050e2b0,   1},
	{ 0x0050e2b8,   3},
	{ 0x0050e40c,   1},
	{ 0x0050e48c,   2},
	{ 0x0050e500,   1},
	{ 0x0050e508,   2},
	{ 0x0050e600,  11},
	{ 0x0050e634,   2},
	{ 0x0050e644,  11},
	{ 0x0050e674,  10},
	{ 0x0050e6a4,   1},
	{ 0x0050e6ac,   2},
	{ 0x0050e6e8,   1},
	{ 0x0050e6f0,  28},
	{ 0x0050e770,   1},
	{ 0x0050e778,   2},
	{ 0x0050e798,   2},
	{ 0x0050e7dc,   1},
	{ 0x00510384,   1},
	{ 0x005104a0,   1},
	{ 0x00510604,   1},
	{ 0x00510680,   1},
	{ 0x00510714,   1},
	{ 0x0051081c,   1},
	{ 0x00510884,   1},
	{ 0x005108b0,   1},
	{ 0x005108c8,   2},
	{ 0x00510b04,   1},
	{ 0x00510c04,   1},
	{ 0x00510c10,   8},
	{ 0x00510c88,   1},
	{ 0x00510d00,   1},
	{ 0x00510e08,   1},
	{ 0x00510f08,   1},
	{ 0x00511000,   1},
	{ 0x0051100c,   1},
	{ 0x00511018,   1},
	{ 0x00511854,   1},
	{ 0x00511ab0,   1},
	{ 0x00511ab8,   3},
	{ 0x00511c0c,   1},
	{ 0x00511c8c,   2},
	{ 0x00511d00,   1},
	{ 0x00511d08,   2},
	{ 0x00511e00,  11},
	{ 0x00511e34,   2},
	{ 0x00511e44,  11},
	{ 0x00511e74,  10},
	{ 0x00511ea4,   1},
	{ 0x00511eac,   2},
	{ 0x00511ee8,   1},
	{ 0x00511ef0,  28},
	{ 0x00511f70,   1},
	{ 0x00511f78,   2},
	{ 0x00511f98,   2},
	{ 0x00511fdc,   1},
	{ 0x0051202c,   2},
	{ 0x005120a0,   1},
	{ 0x005120a8,   1},
	{ 0x00512890,   2},
	{ 0x005128a0,   3},
	{ 0x005128b0,   2},
	{ 0x00513014,   1},
	{ 0x005130cc,   1},
	{ 0x005131dc,   1},
	{ 0x00513214,   1},
	{ 0x005132cc,   1},
	{ 0x005133dc,   1},
	{ 0x00513414,   1},
	{ 0x005134cc,   1},
	{ 0x005135dc,   1},
	{ 0x00513e14,   1},
	{ 0x00513ecc,   1},
	{ 0x00513fdc,   1},
	{ 0x00514054,   1},
	{ 0x005142b0,   1},
	{ 0x005142b8,   3},
	{ 0x0051440c,   1},
	{ 0x0051448c,   2},
	{ 0x00514500,   1},
	{ 0x00514508,   2},
	{ 0x00514600,  11},
	{ 0x00514634,   2},
	{ 0x00514644,  11},
	{ 0x00514674,  10},
	{ 0x005146a4,   1},
	{ 0x005146ac,   2},
	{ 0x005146e8,   1},
	{ 0x005146f0,  28},
	{ 0x00514770,   1},
	{ 0x00514778,   2},
	{ 0x00514798,   2},
	{ 0x005147dc,   1},
	{ 0x00514854,   1},
	{ 0x00514ab0,   1},
	{ 0x00514ab8,   3},
	{ 0x00514c0c,   1},
	{ 0x00514c8c,   2},
	{ 0x00514d00,   1},
	{ 0x00514d08,   2},
	{ 0x00514e00,  11},
	{ 0x00514e34,   2},
	{ 0x00514e44,  11},
	{ 0x00514e74,  10},
	{ 0x00514ea4,   1},
	{ 0x00514eac,   2},
	{ 0x00514ee8,   1},
	{ 0x00514ef0,  28},
	{ 0x00514f70,   1},
	{ 0x00514f78,   2},
	{ 0x00514f98,   2},
	{ 0x00514fdc,   1},
	{ 0x00515054,   1},
	{ 0x005152b0,   1},
	{ 0x005152b8,   3},
	{ 0x0051540c,   1},
	{ 0x0051548c,   2},
	{ 0x00515500,   1},
	{ 0x00515508,   2},
	{ 0x00515600,  11},
	{ 0x00515634,   2},
	{ 0x00515644,  11},
	{ 0x00515674,  10},
	{ 0x005156a4,   1},
	{ 0x005156ac,   2},
	{ 0x005156e8,   1},
	{ 0x005156f0,  28},
	{ 0x00515770,   1},
	{ 0x00515778,   2},
	{ 0x00515798,   2},
	{ 0x005157dc,   1},
	{ 0x00515854,   1},
	{ 0x00515ab0,   1},
	{ 0x00515ab8,   3},
	{ 0x00515c0c,   1},
	{ 0x00515c8c,   2},
	{ 0x00515d00,   1},
	{ 0x00515d08,   2},
	{ 0x00515e00,  11},
	{ 0x00515e34,   2},
	{ 0x00515e44,  11},
	{ 0x00515e74,  10},
	{ 0x00515ea4,   1},
	{ 0x00515eac,   2},
	{ 0x00515ee8,   1},
	{ 0x00515ef0,  28},
	{ 0x00515f70,   1},
	{ 0x00515f78,   2},
	{ 0x00515f98,   2},
	{ 0x00515fdc,   1},
	{ 0x00516054,   1},
	{ 0x005162b0,   1},
	{ 0x005162b8,   3},
	{ 0x0051640c,   1},
	{ 0x0051648c,   2},
	{ 0x00516500,   1},
	{ 0x00516508,   2},
	{ 0x00516600,  11},
	{ 0x00516634,   2},
	{ 0x00516644,  11},
	{ 0x00516674,  10},
	{ 0x005166a4,   1},
	{ 0x005166ac,   2},
	{ 0x005166e8,   1},
	{ 0x005166f0,  28},
	{ 0x00516770,   1},
	{ 0x00516778,   2},
	{ 0x00516798,   2},
	{ 0x005167dc,   1},
	{ 0x00518384,   1},
	{ 0x005184a0,   1},
	{ 0x00518604,   1},
	{ 0x00518680,   1},
	{ 0x00518714,   1},
	{ 0x0051881c,   1},
	{ 0x00518884,   1},
	{ 0x005188b0,   1},
	{ 0x005188c8,   2},
	{ 0x00518b04,   1},
	{ 0x00518c04,   1},
	{ 0x00518c10,   8},
	{ 0x00518c88,   1},
	{ 0x00518d00,   1},
	{ 0x00518e08,   1},
	{ 0x00518f08,   1},
	{ 0x00519000,   1},
	{ 0x0051900c,   1},
	{ 0x00519018,   1},
	{ 0x00519854,   1},
	{ 0x00519ab0,   1},
	{ 0x00519ab8,   3},
	{ 0x00519c0c,   1},
	{ 0x00519c8c,   2},
	{ 0x00519d00,   1},
	{ 0x00519d08,   2},
	{ 0x00519e00,  11},
	{ 0x00519e34,   2},
	{ 0x00519e44,  11},
	{ 0x00519e74,  10},
	{ 0x00519ea4,   1},
	{ 0x00519eac,   2},
	{ 0x00519ee8,   1},
	{ 0x00519ef0,  28},
	{ 0x00519f70,   1},
	{ 0x00519f78,   2},
	{ 0x00519f98,   2},
	{ 0x00519fdc,   1},
	{ 0x0051a02c,   2},
	{ 0x0051a0a0,   1},
	{ 0x0051a0a8,   1},
	{ 0x0051a890,   2},
	{ 0x0051a8a0,   3},
	{ 0x0051a8b0,   2},
	{ 0x0051b014,   1},
	{ 0x0051b0cc,   1},
	{ 0x0051b1dc,   1},
	{ 0x0051b214,   1},
	{ 0x0051b2cc,   1},
	{ 0x0051b3dc,   1},
	{ 0x0051b414,   1},
	{ 0x0051b4cc,   1},
	{ 0x0051b5dc,   1},
	{ 0x0051be14,   1},
	{ 0x0051becc,   1},
	{ 0x0051bfdc,   1},
	{ 0x0051c054,   1},
	{ 0x0051c2b0,   1},
	{ 0x0051c2b8,   3},
	{ 0x0051c40c,   1},
	{ 0x0051c48c,   2},
	{ 0x0051c500,   1},
	{ 0x0051c508,   2},
	{ 0x0051c600,  11},
	{ 0x0051c634,   2},
	{ 0x0051c644,  11},
	{ 0x0051c674,  10},
	{ 0x0051c6a4,   1},
	{ 0x0051c6ac,   2},
	{ 0x0051c6e8,   1},
	{ 0x0051c6f0,  28},
	{ 0x0051c770,   1},
	{ 0x0051c778,   2},
	{ 0x0051c798,   2},
	{ 0x0051c7dc,   1},
	{ 0x0051c854,   1},
	{ 0x0051cab0,   1},
	{ 0x0051cab8,   3},
	{ 0x0051cc0c,   1},
	{ 0x0051cc8c,   2},
	{ 0x0051cd00,   1},
	{ 0x0051cd08,   2},
	{ 0x0051ce00,  11},
	{ 0x0051ce34,   2},
	{ 0x0051ce44,  11},
	{ 0x0051ce74,  10},
	{ 0x0051cea4,   1},
	{ 0x0051ceac,   2},
	{ 0x0051cee8,   1},
	{ 0x0051cef0,  28},
	{ 0x0051cf70,   1},
	{ 0x0051cf78,   2},
	{ 0x0051cf98,   2},
	{ 0x0051cfdc,   1},
	{ 0x0051d054,   1},
	{ 0x0051d2b0,   1},
	{ 0x0051d2b8,   3},
	{ 0x0051d40c,   1},
	{ 0x0051d48c,   2},
	{ 0x0051d500,   1},
	{ 0x0051d508,   2},
	{ 0x0051d600,  11},
	{ 0x0051d634,   2},
	{ 0x0051d644,  11},
	{ 0x0051d674,  10},
	{ 0x0051d6a4,   1},
	{ 0x0051d6ac,   2},
	{ 0x0051d6e8,   1},
	{ 0x0051d6f0,  28},
	{ 0x0051d770,   1},
	{ 0x0051d778,   2},
	{ 0x0051d798,   2},
	{ 0x0051d7dc,   1},
	{ 0x0051d854,   1},
	{ 0x0051dab0,   1},
	{ 0x0051dab8,   3},
	{ 0x0051dc0c,   1},
	{ 0x0051dc8c,   2},
	{ 0x0051dd00,   1},
	{ 0x0051dd08,   2},
	{ 0x0051de00,  11},
	{ 0x0051de34,   2},
	{ 0x0051de44,  11},
	{ 0x0051de74,  10},
	{ 0x0051dea4,   1},
	{ 0x0051deac,   2},
	{ 0x0051dee8,   1},
	{ 0x0051def0,  28},
	{ 0x0051df70,   1},
	{ 0x0051df78,   2},
	{ 0x0051df98,   2},
	{ 0x0051dfdc,   1},
	{ 0x0051e054,   1},
	{ 0x0051e2b0,   1},
	{ 0x0051e2b8,   3},
	{ 0x0051e40c,   1},
	{ 0x0051e48c,   2},
	{ 0x0051e500,   1},
	{ 0x0051e508,   2},
	{ 0x0051e600,  11},
	{ 0x0051e634,   2},
	{ 0x0051e644,  11},
	{ 0x0051e674,  10},
	{ 0x0051e6a4,   1},
	{ 0x0051e6ac,   2},
	{ 0x0051e6e8,   1},
	{ 0x0051e6f0,  28},
	{ 0x0051e770,   1},
	{ 0x0051e778,   2},
	{ 0x0051e798,   2},
	{ 0x0051e7dc,   1},
	{ 0x00520384,   1},
	{ 0x005204a0,   1},
	{ 0x00520604,   1},
	{ 0x00520680,   1},
	{ 0x00520714,   1},
	{ 0x0052081c,   1},
	{ 0x00520884,   1},
	{ 0x005208b0,   1},
	{ 0x005208c8,   2},
	{ 0x00520b04,   1},
	{ 0x00520c04,   1},
	{ 0x00520c10,   8},
	{ 0x00520c88,   1},
	{ 0x00520d00,   1},
	{ 0x00520e08,   1},
	{ 0x00520f08,   1},
	{ 0x00521000,   1},
	{ 0x0052100c,   1},
	{ 0x00521018,   1},
	{ 0x00521854,   1},
	{ 0x00521ab0,   1},
	{ 0x00521ab8,   3},
	{ 0x00521c0c,   1},
	{ 0x00521c8c,   2},
	{ 0x00521d00,   1},
	{ 0x00521d08,   2},
	{ 0x00521e00,  11},
	{ 0x00521e34,   2},
	{ 0x00521e44,  11},
	{ 0x00521e74,  10},
	{ 0x00521ea4,   1},
	{ 0x00521eac,   2},
	{ 0x00521ee8,   1},
	{ 0x00521ef0,  28},
	{ 0x00521f70,   1},
	{ 0x00521f78,   2},
	{ 0x00521f98,   2},
	{ 0x00521fdc,   1},
	{ 0x0052202c,   2},
	{ 0x005220a0,   1},
	{ 0x005220a8,   1},
	{ 0x00522890,   2},
	{ 0x005228a0,   3},
	{ 0x005228b0,   2},
	{ 0x00523014,   1},
	{ 0x005230cc,   1},
	{ 0x005231dc,   1},
	{ 0x00523214,   1},
	{ 0x005232cc,   1},
	{ 0x005233dc,   1},
	{ 0x00523414,   1},
	{ 0x005234cc,   1},
	{ 0x005235dc,   1},
	{ 0x00523e14,   1},
	{ 0x00523ecc,   1},
	{ 0x00523fdc,   1},
	{ 0x00524054,   1},
	{ 0x005242b0,   1},
	{ 0x005242b8,   3},
	{ 0x0052440c,   1},
	{ 0x0052448c,   2},
	{ 0x00524500,   1},
	{ 0x00524508,   2},
	{ 0x00524600,  11},
	{ 0x00524634,   2},
	{ 0x00524644,  11},
	{ 0x00524674,  10},
	{ 0x005246a4,   1},
	{ 0x005246ac,   2},
	{ 0x005246e8,   1},
	{ 0x005246f0,  28},
	{ 0x00524770,   1},
	{ 0x00524778,   2},
	{ 0x00524798,   2},
	{ 0x005247dc,   1},
	{ 0x00524854,   1},
	{ 0x00524ab0,   1},
	{ 0x00524ab8,   3},
	{ 0x00524c0c,   1},
	{ 0x00524c8c,   2},
	{ 0x00524d00,   1},
	{ 0x00524d08,   2},
	{ 0x00524e00,  11},
	{ 0x00524e34,   2},
	{ 0x00524e44,  11},
	{ 0x00524e74,  10},
	{ 0x00524ea4,   1},
	{ 0x00524eac,   2},
	{ 0x00524ee8,   1},
	{ 0x00524ef0,  28},
	{ 0x00524f70,   1},
	{ 0x00524f78,   2},
	{ 0x00524f98,   2},
	{ 0x00524fdc,   1},
	{ 0x00525054,   1},
	{ 0x005252b0,   1},
	{ 0x005252b8,   3},
	{ 0x0052540c,   1},
	{ 0x0052548c,   2},
	{ 0x00525500,   1},
	{ 0x00525508,   2},
	{ 0x00525600,  11},
	{ 0x00525634,   2},
	{ 0x00525644,  11},
	{ 0x00525674,  10},
	{ 0x005256a4,   1},
	{ 0x005256ac,   2},
	{ 0x005256e8,   1},
	{ 0x005256f0,  28},
	{ 0x00525770,   1},
	{ 0x00525778,   2},
	{ 0x00525798,   2},
	{ 0x005257dc,   1},
	{ 0x00525854,   1},
	{ 0x00525ab0,   1},
	{ 0x00525ab8,   3},
	{ 0x00525c0c,   1},
	{ 0x00525c8c,   2},
	{ 0x00525d00,   1},
	{ 0x00525d08,   2},
	{ 0x00525e00,  11},
	{ 0x00525e34,   2},
	{ 0x00525e44,  11},
	{ 0x00525e74,  10},
	{ 0x00525ea4,   1},
	{ 0x00525eac,   2},
	{ 0x00525ee8,   1},
	{ 0x00525ef0,  28},
	{ 0x00525f70,   1},
	{ 0x00525f78,   2},
	{ 0x00525f98,   2},
	{ 0x00525fdc,   1},
	{ 0x00526054,   1},
	{ 0x005262b0,   1},
	{ 0x005262b8,   3},
	{ 0x0052640c,   1},
	{ 0x0052648c,   2},
	{ 0x00526500,   1},
	{ 0x00526508,   2},
	{ 0x00526600,  11},
	{ 0x00526634,   2},
	{ 0x00526644,  11},
	{ 0x00526674,  10},
	{ 0x005266a4,   1},
	{ 0x005266ac,   2},
	{ 0x005266e8,   1},
	{ 0x005266f0,  28},
	{ 0x00526770,   1},
	{ 0x00526778,   2},
	{ 0x00526798,   2},
	{ 0x005267dc,   1},
	{ 0x00528384,   1},
	{ 0x005284a0,   1},
	{ 0x00528604,   1},
	{ 0x00528680,   1},
	{ 0x00528714,   1},
	{ 0x0052881c,   1},
	{ 0x00528884,   1},
	{ 0x005288b0,   1},
	{ 0x005288c8,   2},
	{ 0x00528b04,   1},
	{ 0x00528c04,   1},
	{ 0x00528c10,   8},
	{ 0x00528c88,   1},
	{ 0x00528d00,   1},
	{ 0x00528e08,   1},
	{ 0x00528f08,   1},
	{ 0x00529000,   1},
	{ 0x0052900c,   1},
	{ 0x00529018,   1},
	{ 0x00529854,   1},
	{ 0x00529ab0,   1},
	{ 0x00529ab8,   3},
	{ 0x00529c0c,   1},
	{ 0x00529c8c,   2},
	{ 0x00529d00,   1},
	{ 0x00529d08,   2},
	{ 0x00529e00,  11},
	{ 0x00529e34,   2},
	{ 0x00529e44,  11},
	{ 0x00529e74,  10},
	{ 0x00529ea4,   1},
	{ 0x00529eac,   2},
	{ 0x00529ee8,   1},
	{ 0x00529ef0,  28},
	{ 0x00529f70,   1},
	{ 0x00529f78,   2},
	{ 0x00529f98,   2},
	{ 0x00529fdc,   1},
	{ 0x0052a02c,   2},
	{ 0x0052a0a0,   1},
	{ 0x0052a0a8,   1},
	{ 0x0052a890,   2},
	{ 0x0052a8a0,   3},
	{ 0x0052a8b0,   2},
	{ 0x0052b014,   1},
	{ 0x0052b0cc,   1},
	{ 0x0052b1dc,   1},
	{ 0x0052b214,   1},
	{ 0x0052b2cc,   1},
	{ 0x0052b3dc,   1},
	{ 0x0052b414,   1},
	{ 0x0052b4cc,   1},
	{ 0x0052b5dc,   1},
	{ 0x0052be14,   1},
	{ 0x0052becc,   1},
	{ 0x0052bfdc,   1},
	{ 0x0052c054,   1},
	{ 0x0052c2b0,   1},
	{ 0x0052c2b8,   3},
	{ 0x0052c40c,   1},
	{ 0x0052c48c,   2},
	{ 0x0052c500,   1},
	{ 0x0052c508,   2},
	{ 0x0052c600,  11},
	{ 0x0052c634,   2},
	{ 0x0052c644,  11},
	{ 0x0052c674,  10},
	{ 0x0052c6a4,   1},
	{ 0x0052c6ac,   2},
	{ 0x0052c6e8,   1},
	{ 0x0052c6f0,  28},
	{ 0x0052c770,   1},
	{ 0x0052c778,   2},
	{ 0x0052c798,   2},
	{ 0x0052c7dc,   1},
	{ 0x0052c854,   1},
	{ 0x0052cab0,   1},
	{ 0x0052cab8,   3},
	{ 0x0052cc0c,   1},
	{ 0x0052cc8c,   2},
	{ 0x0052cd00,   1},
	{ 0x0052cd08,   2},
	{ 0x0052ce00,  11},
	{ 0x0052ce34,   2},
	{ 0x0052ce44,  11},
	{ 0x0052ce74,  10},
	{ 0x0052cea4,   1},
	{ 0x0052ceac,   2},
	{ 0x0052cee8,   1},
	{ 0x0052cef0,  28},
	{ 0x0052cf70,   1},
	{ 0x0052cf78,   2},
	{ 0x0052cf98,   2},
	{ 0x0052cfdc,   1},
	{ 0x0052d054,   1},
	{ 0x0052d2b0,   1},
	{ 0x0052d2b8,   3},
	{ 0x0052d40c,   1},
	{ 0x0052d48c,   2},
	{ 0x0052d500,   1},
	{ 0x0052d508,   2},
	{ 0x0052d600,  11},
	{ 0x0052d634,   2},
	{ 0x0052d644,  11},
	{ 0x0052d674,  10},
	{ 0x0052d6a4,   1},
	{ 0x0052d6ac,   2},
	{ 0x0052d6e8,   1},
	{ 0x0052d6f0,  28},
	{ 0x0052d770,   1},
	{ 0x0052d778,   2},
	{ 0x0052d798,   2},
	{ 0x0052d7dc,   1},
	{ 0x0052d854,   1},
	{ 0x0052dab0,   1},
	{ 0x0052dab8,   3},
	{ 0x0052dc0c,   1},
	{ 0x0052dc8c,   2},
	{ 0x0052dd00,   1},
	{ 0x0052dd08,   2},
	{ 0x0052de00,  11},
	{ 0x0052de34,   2},
	{ 0x0052de44,  11},
	{ 0x0052de74,  10},
	{ 0x0052dea4,   1},
	{ 0x0052deac,   2},
	{ 0x0052dee8,   1},
	{ 0x0052def0,  28},
	{ 0x0052df70,   1},
	{ 0x0052df78,   2},
	{ 0x0052df98,   2},
	{ 0x0052dfdc,   1},
	{ 0x0052e054,   1},
	{ 0x0052e2b0,   1},
	{ 0x0052e2b8,   3},
	{ 0x0052e40c,   1},
	{ 0x0052e48c,   2},
	{ 0x0052e500,   1},
	{ 0x0052e508,   2},
	{ 0x0052e600,  11},
	{ 0x0052e634,   2},
	{ 0x0052e644,  11},
	{ 0x0052e674,  10},
	{ 0x0052e6a4,   1},
	{ 0x0052e6ac,   2},
	{ 0x0052e6e8,   1},
	{ 0x0052e6f0,  28},
	{ 0x0052e770,   1},
	{ 0x0052e778,   2},
	{ 0x0052e798,   2},
	{ 0x0052e7dc,   1},
	{ 0x00900100,   1},
	{ 0x00904100,   1},
	{ 0x00908100,   1},
	{ 0x0090c100,   1},
	{ 0x00910100,   1},
	{ 0x00914100,   1},
	{ 0x009a0100,   1},
};


static const u32 gp106_global_whitelist_ranges_count =
	ARRAY_SIZE(gp106_global_whitelist_ranges);

/* context */

/* runcontrol */
static const u32 gp106_runcontrol_whitelist[] = {
};
static const u32 gp106_runcontrol_whitelist_count =
	ARRAY_SIZE(gp106_runcontrol_whitelist);

static const struct regop_offset_range gp106_runcontrol_whitelist_ranges[] = {
};
static const u32 gp106_runcontrol_whitelist_ranges_count =
	ARRAY_SIZE(gp106_runcontrol_whitelist_ranges);


/* quad ctl */
static const u32 gp106_qctl_whitelist[] = {
};
static const u32 gp106_qctl_whitelist_count =
	ARRAY_SIZE(gp106_qctl_whitelist);

static const struct regop_offset_range gp106_qctl_whitelist_ranges[] = {
};
static const u32 gp106_qctl_whitelist_ranges_count =
	ARRAY_SIZE(gp106_qctl_whitelist_ranges);

static const struct regop_offset_range *gp106_get_global_whitelist_ranges(void)
{
	return gp106_global_whitelist_ranges;
}

static int gp106_get_global_whitelist_ranges_count(void)
{
	return gp106_global_whitelist_ranges_count;
}

static const struct regop_offset_range *gp106_get_context_whitelist_ranges(void)
{
	return gp106_global_whitelist_ranges;
}

static int gp106_get_context_whitelist_ranges_count(void)
{
	return gp106_global_whitelist_ranges_count;
}

static const u32 *gp106_get_runcontrol_whitelist(void)
{
	return gp106_runcontrol_whitelist;
}

static int gp106_get_runcontrol_whitelist_count(void)
{
	return gp106_runcontrol_whitelist_count;
}

static const
struct regop_offset_range *gp106_get_runcontrol_whitelist_ranges(void)
{
	return gp106_runcontrol_whitelist_ranges;
}

static int gp106_get_runcontrol_whitelist_ranges_count(void)
{
	return gp106_runcontrol_whitelist_ranges_count;
}

static const u32 *gp106_get_qctl_whitelist(void)
{
	return gp106_qctl_whitelist;
}

static int gp106_get_qctl_whitelist_count(void)
{
	return gp106_qctl_whitelist_count;
}

static const struct regop_offset_range *gp106_get_qctl_whitelist_ranges(void)
{
	return gp106_qctl_whitelist_ranges;
}

static int gp106_get_qctl_whitelist_ranges_count(void)
{
	return gp106_qctl_whitelist_ranges_count;
}

static int gp106_apply_smpc_war(struct dbg_session_gk20a *dbg_s)
{
	/* Not needed on gp106 */
	return 0;
}

void gp106_init_regops(struct gpu_ops *gops)
{
	gops->regops.get_global_whitelist_ranges =
		gp106_get_global_whitelist_ranges;
	gops->regops.get_global_whitelist_ranges_count =
		gp106_get_global_whitelist_ranges_count;

	gops->regops.get_context_whitelist_ranges =
		gp106_get_context_whitelist_ranges;
	gops->regops.get_context_whitelist_ranges_count =
		gp106_get_context_whitelist_ranges_count;

	gops->regops.get_runcontrol_whitelist =
		gp106_get_runcontrol_whitelist;
	gops->regops.get_runcontrol_whitelist_count =
		gp106_get_runcontrol_whitelist_count;

	gops->regops.get_runcontrol_whitelist_ranges =
		gp106_get_runcontrol_whitelist_ranges;
	gops->regops.get_runcontrol_whitelist_ranges_count =
		gp106_get_runcontrol_whitelist_ranges_count;

	gops->regops.get_qctl_whitelist =
		gp106_get_qctl_whitelist;
	gops->regops.get_qctl_whitelist_count =
		gp106_get_qctl_whitelist_count;

	gops->regops.get_qctl_whitelist_ranges =
		gp106_get_qctl_whitelist_ranges;
	gops->regops.get_qctl_whitelist_ranges_count =
		gp106_get_qctl_whitelist_ranges_count;

	gops->regops.apply_smpc_war =
		gp106_apply_smpc_war;
}